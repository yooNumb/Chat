#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <string>
#include <arpa/inet.h>
#include <errno.h>
#include <event.h>
#include <map>
#include <vector>
#include "tcpserver.h"
#include "pthread.h"
using namespace std;

#define BUFF_SIZE 128

//监听套接字回掉函数
void listen_cb(int fd,short event,void *arg)
{
	TcpServer *pthis = (TcpServer *)arg;

	struct sockaddr_in cli;
	socklen_t len = sizeof(cli);
	int clientfd = accept(fd,(struct sockaddr*)&cli,&len);
	if(-1 == clientfd)
	{
		cerr<<"accept clientfd fail;errno:"<<errno<<endl;
		throw "";
	}

	//指向存储子线程负载量的map表的迭代器
	map<int,int>::iterator it = pthis->_pthread_num_map.begin();
	int minKey = it->first;//负载量最少的线程
	int minValue = it->second;//最少的负载量
	for(;it!=pthis->_pthread_num_map.end();++it)//在map表查找，second最小的
	{
		if(it->second < minValue)
		{
			minKey = it->first;
			minValue = it->second;
		}
	}
	
	char buff[BUFF_SIZE] = {0};
	sprintf(buff,"%d",clientfd);//将clientfd写到buff中
	
	//将客户端套接字通过socketpair发给子线程
	if(-1 == send(minKey,buff,strlen(buff),0))
	{
		cerr<<"send clientfd fail;errno:"<<errno<<endl;
		return;
	}

	/*
	 * 测试子线程的负载量是否均衡
	it = pthis->_pthread_num_map.begin();
	for(;it!=pthis->_pthread_num_map.end();++it)
	{
		printf("pthread:%d, count:%d\n",it->first,it->second);
	}*/
}
//可读事件的回调函数
void sockpair_0_cb(int fd,short event,void *arg)
{
	//读取管道内容
	TcpServer *pthis = (TcpServer *)arg;
	char buff[BUFF_SIZE] = {0};
	if(recv(fd,buff,BUFF_SIZE-1,0) <= 0)
	{
		cerr<<"recv fail;errno:"<<errno<<endl;
		return;
	}
	int tmp = atoi(buff);//子线程的负载量

	//更新到map表---->fd
	pthis->_pthread_num_map[fd] = tmp;

}

void TcpServer::create_socketpair()
{
	int i = 0;
	for(;i<_pthread_num;i++)
	{
		int arr[2];
		int res = socketpair(AF_LOCAL,SOCK_STREAM,0,arr);
		if(res < 0)
		{
			cerr<<"socket_pair create fail;errno:"<<errno<<endl;
			return;
		}
		//添加到存储sockpair的vector中
		_vector_sockpair.push_back(vec(arr));

		//将管道的0端加入到libevent  ---->sockpair_0_cb
		struct event* socketpair_event = event_new(_base,arr[0],EV_READ|EV_PERSIST,sockpair_0_cb,this);
		if(NULL == socketpair_event)
		{
			cerr<<"socketpair_event fail;errno:"<<errno<<endl;
			return;
		}
		event_add(socketpair_event,NULL);

		//存放管道的0端 把线程负载量初始化为0
		_pthread_num_map.insert(make_pair(arr[0],0));
	}
}

void TcpServer::create_pthread()
{
	for(int i=0;i<_pthread_num;i++)
	{
		_pthread.push_back(new Pthread(_vector_sockpair[i].err[1]));
	}
}

TcpServer::TcpServer(char *ip,short port,int pthread_num)
{
	//初始化libevent
	_base = event_base_new();

	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	assert(listenfd != -1);

	struct sockaddr_in ser,cli;
	ser.sin_family = AF_INET;
	ser.sin_port = htons(port);
	ser.sin_addr.s_addr = inet_addr(ip);

	if(-1 == bind(listenfd,(struct sockaddr*)&ser,sizeof(ser)))
	{
		cout<<"server bind fail;errno:"<<errno<<endl;
	}

	_listen_fd = listenfd;
	_pthread_num = pthread_num;
	listen(_listen_fd,5);//创建内核监听事件表

	//将监听套接字和回掉函数绑定起来
	struct event* listen_event = event_new(_base,_listen_fd,EV_READ|EV_PERSIST,listen_cb,(void *)this);
	if(NULL == listen_event)
	{
		cerr<<"event new fail;errno:"<<errno<<endl;
	}
	//将事件添加到libevent中
	event_add(listen_event,NULL);
}


TcpServer::~TcpServer()
{
}

void TcpServer::run()
{
	create_socketpair();//创建双向管道
	create_pthread();//启动规定个数的子线程

	//添加到libevent上的fd设置成非阻塞的
	event_base_loop(_base,EVLOOP_NONBLOCK);
	event_base_dispatch(_base);//开始反应堆
}
