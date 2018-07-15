#ifndef _REDIS_H_
#define _REDIS_H_

#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <hiredis/hiredis.h>
using namespace std;

class Redis
{
	public:
		Redis();
		~Redis();
		//连接redis     ip地址           端口号          
		bool connect(string host, int port);
		
		//获取数据		
		string get(string key);
		
		//存储数据
		void set(string key, string value);

		//删除key存储的值
		void del(string key);
	private:	
		redisContext* _connect;//redis指针
		redisReply* _reply;//获取get结果的指针
};

#endif
