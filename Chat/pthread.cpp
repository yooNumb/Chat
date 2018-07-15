#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <map>
#include <event.h>
#include "controller.h"
#include "pthread.h"
#include "view.h"
#include "public.h"
using namespace std;

extern Controller gController;
static float usetime = 0.0;
#define BUFF_SIZE 256 

void *pthread_run(void *arg);
void event_del(map<int,struct event *>&fdevent,int fd);
void client_cb(int fd,short event,void *arg);//after link
/*
void *heart_hander(void *arg)//用户检测心跳的子线程
{
	cout<<"heart-beat checking thread started!"<<endl;
	Pthread *pthis = (Pthread *)arg;

	while(1)
	{
		map<int,int>::iterator it = pthis->_heart.begin();
		for(;it!=pthis->_heart.end();)
		{
			if(it->second == 5)//说明已经15秒没有接收到心跳包
			{
				cout<<"one client AbortExit!"<<endl;

				int fd = it->first;
				//close(fd);
				event_del(pthis->_event_map,fd);
				pthis->_heart.erase(it++);//从Map表删除该记录
			}
			else if(it->second>=0 && it->second<5)
			{
				it->second += 1;
				++it;
			}
			else
			{
				++it;
			}
		}
		sleep(3);//定时三秒，每三秒遍历一遍map表
	}
}*/

void event_del(map<int,struct event *>&fdevent,int fd)
{
	map<int,struct event *>::iterator it = fdevent.find(fd);
	if(it == fdevent.end())
	{
		cerr<<"client shutdown! errno:"<<errno<<endl;
		return;
	}
	event_free(it->second);
	fdevent.erase(it);
	close(fd);
	cout<<"one client closed!"<<endl;
}
void client_cb(int fd,short event,void *arg)//after link
{
	Pthread* pthis = (Pthread *)arg;

	char buff[BUFF_SIZE] = {0};
	if(recv(fd,buff,BUFF_SIZE-1,0) <= 0)
	{
		Json::Value root;
		root["msgtype"] = MSG_TYPE_QUIT;
		root["name"] = "Abnormal exit!";
		gController.process(fd,root.toStyledString());
		event_del(pthis->_event_map,fd);
		return;
	}
	if(strncmp(buff,"end",3) == 0)
	{
		event_del(pthis->_event_map,fd);
		return;
	}

/*	if(strncmp(buff,"heart",5) == 0)
	{
		map<int,int>::iterator it = pthis->_heart.find(fd);
		it->second = 0;
	}*/

	struct timeval t1,t2;
	gettimeofday(&t1,NULL);//开始时间戳
	gController.process(fd,buff);
	gettimeofday(&t2,NULL);//结束时间戳
	float time = 1000000*(t2.tv_sec-t1.tv_sec)+t2.tv_usec-t1.tv_usec;//差值
	time /= 1000000;
	usetime += time;
	cout<<"time::::::::"<<usetime<<endl;
}

void sockpair_1_cb(int fd,short event,void *arg)
{
	static volatile int b = 0;
	b++;

	Pthread* pthis = (Pthread *)arg;
	char buff[128] = {0};
	if(recv(fd,buff,BUFF_SIZE-1,0) <= 0)
	{
		cerr<<"pthread recv clientfd fail;errno:"<<errno<<endl;
		return;
	}
	int clientfd = atoi(buff);

	//将计数器初始化为0 fd-count添加到监听心跳的map表中
//	pthis->_heart.insert(make_pair(clientfd,0));

	//将clientfd和client_cb绑定起来 添加到子线程的libevent 
	struct event* listen_event = event_new(pthis->_base,clientfd,EV_READ|EV_PERSIST,client_cb,pthis);
	event_add(listen_event,NULL);

	pthis->_event_map.insert(make_pair(clientfd,listen_event));

	//给主线程回复当前监听的客户端数量
	int num = pthis->_event_map.size();
	char sbuff[BUFF_SIZE] = {0};
	sprintf(sbuff,"%d",num);
	if(-1 == send(fd,sbuff,strlen(sbuff),0))
	{
		cerr<<"send fail;errno:"<<errno<<endl;
		return;
	}
//	cout<<b<<endl;
}
Pthread::Pthread(int sockpair_1_fd)
{
	_sockpair_1_fd = sockpair_1_fd;
	_base = event_base_new();

	int res = pthread_create(&_pid,NULL,pthread_run,this);
}

Pthread::~Pthread()
{
}

void *pthread_run(void *arg)
{
	//初始化libevent class fd
	Pthread* pthis = (Pthread *)arg;

	/*pthread_t id;//创建心跳检测线程
	int ret = pthread_create(&id,NULL,heart_hander,pthis);
	if(ret != 0)
		cout<<"Can not create heart-beat checking thread!\n";
	*/

	//绑定
	struct event* listen_event = event_new(pthis->_base,pthis->_sockpair_1_fd,EV_READ|EV_PERSIST,sockpair_1_cb,pthis);
	if(NULL == listen_event)
	{
		cerr<<"event new fail;errno:"<<errno<<endl;
	}
	event_add(listen_event,NULL);//添加事件
	event_base_dispatch(pthis->_base);//开启反应堆
}
