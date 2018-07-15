#ifndef PTHREAD_H
#define PTHREAD_H
#include <iostream>
#include <map>
#include <stdio.h>
#include <sys/socket.h>
#include <event.h>
using namespace std;

class Pthread
{
	public:
		Pthread(int sockpair_1_fd);
		~Pthread();
	private:
		int _sockpair_1_fd;//双向管道1端
		map<int,int> _heart;//存储用户fd和计数器count
		struct event_base* _base;//libevent
		map<int,struct event *> _event_map;//存储事件map表
		pthread_t _pid;

		friend void sockpair_1_cb(int fd,short event,void *arg);
		friend void client_cb(int fd,short event,void *arg);
		friend void *pthread_run(void *arg);
		friend void *heart_hander(void *arg);
};

#endif
