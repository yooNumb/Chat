#include "talkview.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "public.h"

extern CMySqlServer gMySqlServer;//数据库对象
extern Redis gMyRedis;///redis对象

////////////////////////////////////////////////////////
bool TalkModel::checkUserAuth(string name)
{
	string redis = gMyRedis.get(name);
//	cout<<"result:"<<redis<<endl;
	
	if(strcmp(redis.c_str(),"notget") == 0)//在redis中没有找到
	{
		///去数据库里取数据
		char sql[1024]={0};
		sprintf(sql, "select * from state;");

    	if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
	    {
    	    return false;
	    }
		
		//从连接处获取select查询的结果 =》  二维表
		gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
		//循环从二维表中取出一个个记录,分别访问它的每一个字段
		while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
		{
			if(strcmp(name.c_str(), gMySqlServer.mrow[0]) == 0)
			{
				//把用户名和clientfd添加到redis中
				gMyRedis.set(gMySqlServer.mrow[0],gMySqlServer.mrow[1]);

				mfd = atoi(gMySqlServer.mrow[1]);
				return true;
			}
		}
    }
	else
	{
		mfd = atoi(redis.c_str());
		return true;
	}
    return false;
}

void TalkModel::addOffline_msg(string name,string msg)
{
	char sql[128] = {0};
	char buff[128] = {0};
	sprintf(sql,"insert into offline_msg values");
	sprintf(buff,"('%s','%s');",name.c_str(),msg.c_str());
	strcat(sql,buff);

	mysql_query(gMySqlServer.mpcon,sql);
}
/////////////////////////////////////////////////////////
void TalkView::process(int fd,Json::Value root)
{
	_fd = fd;
    _name = root["recvname"].asString();
    _msg = root["msg"].asString();
	_sendname = root["sendname"].asString();
    
    if(_talkModel.checkUserAuth(_name))
	{
		_talkState = true;
	}
    else
	{
		_talkState = false;
	}
}

void TalkView::response()
{
    Json::Value response;
    response["msgtype"] = "MSG_TYPE_ACK";
    if(_talkState)
    {
		Json::Value json;
		json["msgtype"] = "MSG_TYPE_TALK";
		json["msg"] = _msg;
		json["from"] = _sendname;

		send(_talkModel.mfd,json.toStyledString().c_str(),
			strlen(json.toStyledString().c_str()),0);
        response["reason"] = "msg send success!";
    }
    else
    {
		Json::Value json;
		json["msgtype"] = "MSG_TYPE_OFFLINE";
		json["msg"] = _msg;
		json["from"] = _sendname;

		_talkModel.addOffline_msg(_name,json.toStyledString().c_str());
        response["reason"] = "user offline!";
    }
//	cout<<response.toStyledString()<<endl;
    send(_fd,response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()),0);
}
