#include "quitview.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "public.h"


extern CMySqlServer gMySqlServer;
extern Redis gMyRedis;

////////////////////////////////////////////////////////
bool QuitModel::checkUserAuth(int fd)
{
    char sql[1024] = {0};
	char buff[10] = {0};
    sprintf(sql, "select * from state;");
    sprintf(buff,"%d",fd);

    if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
    {
        return false;
    }
    
    //从连接处获取select查询的结果 =》  二维表
    gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
    //循环从二维表中取出一个个记录,分别访问它的每一个字段
    while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
    {
        if(strcmp(buff,gMySqlServer.mrow[1]) == 0)
        {
			gMyRedis.del(gMySqlServer.mrow[0]);
			return true;
        }
    }
    return false;
}
void QuitModel::delUserState(int fd)
{
	char sql[128] = {0};
	char buff[128] = {0};
	sprintf(sql,"delete from state where fd = ");
	sprintf(buff,"'%d';",fd);
	strcat(sql,buff);

	mysql_query(gMySqlServer.mpcon,sql);
}
/////////////////////////////////////////////////////////
void QuitView::process(int fd,Json::Value root)
{
	_fd = fd;
    _name = root["name"].asString();
    
    if(_quitModel.checkUserAuth(_fd))
	{
		_quitState = true;
		_quitModel.delUserState(_fd);
	}
    else
	{
		_quitState = false;
	}
}

void QuitView::response()
{
    Json::Value response;
    response["msgtype"] = "MSG_TYPE_ACK";
    if(_quitState)
    {
		response["name"] = _name;
        response["reason"] = "quit success!";
    }
    else
    {
        response["reason"] = "quit fail";
    }
//	cout<<response.toStyledString()<<endl;
    send(_fd,response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()),0);
}
