#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <map>
#include <vector>
#include <iostream>
#include "pthread.h"
using namespace std;

class vec
{
	public:
		vec(int arr[2])
		{
			err[0] = arr[0];
			err[1] = arr[1];
		}
	private:
		int err[2];
		friend class TcpServer;
};

class TcpServer
{
public:
	TcpServer(char *ip,short port,int pthread_num);
	TcpServer();
	~TcpServer();
	void run();//服务器运行
private:
	int _listen_fd;//监听套接字
	int _pthread_num;//线程个数
	struct event_base* _base;//创建反应堆
	vector<vec> _vector_sockpair; //sockpair vector
	vector<Pthread *> _pthread;//pthread vector
	map<int,int> _pthread_num_map;//存储线程负载量的map表

	void create_socketpair();//创建管道
	void create_pthread();//创建线程

	//监听套接字的回调函数
	friend void listen_cb(int fd,short event,void *arg);
	//双向管道0端回调函数
	friend void sockpair_0_cb(int fd,short event,void *arg);
};

#endif
