#include "groupview.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iterator>
#include "public.h"

extern CMySqlServer gMySqlServer;

////////////////////////////////////////////////////////
void GroupModel::Getfd()
{
    char sql[1024]={0};
    sprintf(sql, "select * from state;");
    
    if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
    {
        return;
    }
    
    gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
 
    while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
    {
		mfd.push_back(atoi(gMySqlServer.mrow[1]));
    }
}
/////////////////////////////////////////////////////////
void GroupView::process(int fd,Json::Value root)
{
	_fd = fd;
    _msg = root["msg"].asString();
	_sendname = root["from"].asString();
    
    _groupModel.Getfd();
	_groupState = true;
}

void GroupView::response()
{
    Json::Value response;
    response["msgtype"] = "MSG_TYPE_ACK";
    if(_groupState)
    {
		vector<int>::iterator it = _groupModel.mfd.begin();
		for(;it!=_groupModel.mfd.end();++it)
		{
			Json::Value json;
			json["msgtype"] = "MSG_TYPE_GROUP";
			json["msg"] = _msg;
			json["from"] = _sendname;

			send(*it,json.toStyledString().c_str(),
			strlen(json.toStyledString().c_str()),0);
		}
		_groupModel.mfd.clear();
        response["reason"] = "msg send success!";
    }
    else
    {
        response["reason"] = "group fail!";
    }
	//cout<<response.toStyledString()<<endl;
    send(_fd,response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()),0);
}
