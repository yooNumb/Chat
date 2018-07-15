#include "redis.h"
#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <hiredis/hiredis.h>
using namespace std;


Redis::Redis()
{					
	_connect = redisConnect("127.0.0.1", 6379);
	if(_connect != NULL && this->_connect->err)
	{
		printf("connect error: %s\n", this->_connect->errstr);
		throw "";
	}
	cout<<"redis server init success!"<<endl;
}

Redis::~Redis()
{
	this->_connect = NULL;
	this->_reply = NULL;            
}
		
		
//获取数据		
string Redis::get(string key)
{
	//类型强转    执行命令的函数     redis指针  命令     键     
	this->_reply = (redisReply*)redisCommand(this->_connect, "GET %s", key.c_str());
	if(REDIS_REPLY_NIL == this->_reply->type)//判断对应的值是否存在
	{
		cout<<"without key="<<key<<endl;
		return "notget";
	}
	string str = this->_reply->str;//获取到的数据
	freeReplyObject(this->_reply);//释放获取数据的指针

	cout<<"get success! "<<key<<"->"<<str<<endl;
	return str;
}
		
//存储数据
void Redis::set(string key, string value)
{
	//执行命令      redis指针      命令		    键          值
	redisCommand(this->_connect, "SET %s %s", key.c_str(), value.c_str());
	cout<<"set success! "<<key<<"->"<<value<<endl;
}

//删除key存储的值
void Redis::del(string key)
{
	redisCommand(this->_connect,"DEL %s",key.c_str());
}

Redis gMyRedis;
