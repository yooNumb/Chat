#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include <json/json.h>
#include "../public.h"
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <termios.h>//passwd
using namespace std;

///////////////////////////////////////////////////
pthread_t tid;
int sockfd;
////////////////////////////////////////////////////

/*void *send_heart(void *arg)
{
	cout<<"The heart-beat sending thread start"<<endl;
	int count = 0;//测试
	while(1)
	{
		send(sockfd,"heart",sizeof("heart"),0);
		sleep(3);//定时3秒

		++count;  //测试：发送15次心跳包就停止发送
		if(count > 15)
			break;
	}
}*/

void *pthread_fun(void *lparg)
{
	while(1)
	{
		char buff[1024] = {0};
		int n = recv(sockfd,buff,1023,0);
		if(n <= 0)
			break;
		cout<<buff<<endl;
	}
}
void Query()
{
	Json::Value root;
	root["msgtype"] = MSG_TYPE_QUERY;

	send(sockfd,root.toStyledString().c_str(),
			strlen(root.toStyledString().c_str()),0);
}

void Quit(char *name)
{

	Json::Value root;
	root["msgtype"] = MSG_TYPE_QUIT;
	root["name"] = name;
	
	send(sockfd,root.toStyledString().c_str(),
			strlen(root.toStyledString().c_str()),0);
}

void Talk(char *name)
{
	/**
	 * for test
	 * */
/*	for(int i=0;i<100000;i++)
	{

		Json::Value json;								
		json["msgtype"] = MSG_TYPE_TALK;
		json["recvname"] = "wy";
		json["msg"] = "hello";
		json["sendname"] = "ww";
		
		send(sockfd,json.toStyledString().c_str(),
				strlen(json.toStyledString().c_str()),0);
	}
*/
	while(1)
	{
		char recvname[20] = {0};
		char msg[128] = {0};
			
		cout<<"msg:";
		gets(msg);
	
		if(strncmp(msg,"end",3) == 0)
			break;

		cout<<"to:";
		gets(recvname);

		Json::Value json;								
		json["msgtype"] = MSG_TYPE_TALK;
		json["recvname"] = recvname;
		json["msg"] = msg;
		json["sendname"] = name;

		send(sockfd,json.toStyledString().c_str(),
				strlen(json.toStyledString().c_str()),0);
		memset(recvname,0,20);
		memset(msg,0,128);
	}
}

void Group(char *name)
{
	while(1)
	{
		char msg[128] = {0};
		cout<<"msg:";
		gets(msg);
		if(strncmp(msg,"end",3) == 0)
			break;

		Json::Value json;										
		json["msgtype"] = MSG_TYPE_GROUP;
		json["msg"] = msg;
		json["from"] = name;

		send(sockfd,json.toStyledString().c_str(),
				strlen(json.toStyledString().c_str()),0);
		memset(msg,0,128);
	}
}
void LoginAfter(char *name)
{
	int choice = 0;
	while(true)
	{
		cout<<"========================"<<endl;
		cout<<"1.talk"<<endl;
		cout<<"2.group talk"<<endl;
		cout<<"3.query_online_name"<<endl;
		cout<<"4.quit"<<endl;
		cout<<"========================"<<endl;
		cout<<"choice:";
		cin>>choice;
		getchar();

		switch(choice)
		{
			case 1:
				Talk(name);
				break;
			case 2:
				Group(name);
				break;
			case 3:
				Query();
				break;
			case 4:
				Quit(name);
				return;
			default:
				break;
		}
	}
}

void Login()
{
	char name[20] = {0};
	char pwd[20] = {0};

	cout<<"input name:";
	gets(name);
	cout<<"input pwd:";

	struct termios oldts,newts;
	tcgetattr(0,&oldts);//初始化一个终端所对应的termios结构
	newts = oldts;
	newts.c_lflag &= ~ECHO;//本地模式与上回显取反//去掉回显功能
	newts.c_lflag &= ~ICANON;
	tcsetattr(0,TCSANOW,&newts);

	int c = 0;
	int count = -1;
	while((c=getchar()) != '\n')
	{
		if(c==19 && count>=0)
		{
			pwd[count--] = 0;
			printf("\033[%dD",1);//光标往前移一个位置
			printf("\033[K");//将当前光标以及往后的字符全部清空
			continue;
		}
		if(c == 19)
		{
			continue;
		}
		pwd[++count] = c;
		printf("*");
	}

	tcsetattr(0,TCSANOW,&oldts);//输入后面命令需要回显
	cout<<endl;

	Json::Value root;
	root["msgtype"] = MSG_TYPE_LOGIN;
	root["name"] = name;
	root["pwd"] = pwd;

	send(sockfd,root.toStyledString().c_str(),
			strlen(root.toStyledString().c_str()),0);

	char buff[1024] = {0};
	recv(sockfd,buff,1023,0);
	cout<<"recv buff"<<buff<<endl;

	Json::Value value;
	Json::Reader reader;
	if(0 == reader.parse(buff,value))
	{
		cerr<<"recv buff format invalid:"<<buff<<endl;
		return;
	}
	string tmp = value["reason"].asString();

	if(strcmp(tmp.c_str(),"login success!")==0)
	{
		pthread_create(&tid,NULL,pthread_fun,NULL);
		LoginAfter(name);
	}
}

void Register()
{
	char name[20] = {0};
	char pwd[20] = {0};

	cout<<"set name:";
	gets(name);
	cout<<"set pwd:";
	gets(pwd);

	Json::Value root;
	root["msgtype"] = MSG_TYPE_REGISTER;
	root["name"] = name;
	root["pwd"] = pwd;

	send(sockfd,root.toStyledString().c_str(),
			strlen(root.toStyledString().c_str()),0);
	
	char buff[128] = {0};
	recv(sockfd,buff,128,0);
	cout<<"recv buff"<<buff<<endl;
}

int main(int argc,char **argv)
{
	if(argc < 3)
	{
		cerr<<"command args is invalid! format: a./out ip port";
	}

	char ip[20] = {0};
	strcpy(ip,argv[1]);
	unsigned short port = atoi(argv[2]);

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == sockfd)
	{
		cerr<<"client socket fail! errno:"<<errno<<endl;
		exit(0);
	}

	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(port);
	ser.sin_addr.s_addr = inet_addr(ip);

	if(-1 == connect(sockfd,(struct sockaddr*)&ser,sizeof(ser)))
	{
		cerr<<"client connect fail! errno:"<<errno<<endl;
		exit(0);
	}
/*
	int ret = pthread_create(&tid,NULL,send_heart,NULL);
	if(ret != 0)
	{
		cout<<"Can not create pthread!";
		exit(1);
	}
*/

	int choice = 0;
	while(1)
	{
		cout<<"========================"<<endl;
		cout<<"1.login"<<endl;
		cout<<"2.register"<<endl;
		cout<<"3.exit"<<endl;
		cout<<"========================"<<endl;
		cout<<"choice:";
		cin>>choice; 
		getchar(); // \n
		
		switch(choice)
		{
			case 1:
				Login();
				break;
			case 2:
				Register();
				break;
			case 3:
				send(sockfd,"end",3,0);
				close(sockfd);
				return 0;
			default:
				cout<<"choice error"<<endl;
				break;
		}
	}
	return 0;
}
