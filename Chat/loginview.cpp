#include "loginview.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "md5.h"
#include "public.h"


extern CMySqlServer gMySqlServer;

////////////////////////////////////////////////////////
bool LoginModel::checkUserAuth(string name,string pwd)
{
	//将用户输入的密码用md5加密
	const char *p = pwd.c_str();
	MD5_CTX md5;
	unsigned char data[40] = {0};
	strcpy((char *)data,p);
	unsigned char md[16];
	char buf[33] = {0};
	char tmp[3] = {0};
	MD5Init(&md5);
	MD5Update(&md5,data,strlen((char *)data));
	MD5Final(&md5,md);
	for(int i=0;i<16;i++)
	{
		sprintf(tmp,"%02x",md[i]);
		strcat(buf,tmp);
	}

    char sql[1024] = {0};
	char buff[128] = {0};
    sprintf(sql, "select * from user where name=");
	sprintf(buff,"'%s' and pwd = '%s';",name.c_str(),buf);
	strcat(sql,buff);

//	printf("sql:%s\n",sql);

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
			if(strcmp(buf,gMySqlServer.mrow[1]) == 0)
			{
				return true;
			}
        }
    }
    return false;
}
bool LoginModel::checkOffline_msg(string name)
{
    char sql[1024]={0};
    sprintf(sql, "select * from offline_msg;");
    
    if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
    {
        return false;
    }
    
    gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
    while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
    {
        if(strcmp(name.c_str(), gMySqlServer.mrow[0]) == 0)
        {
			off_msg = gMySqlServer.mrow[1];
			return true;
        }
	}
}

void LoginModel::addState(string name,int fd)
{
	char sql[128] = {0};
	char buff[128] = {0};
	sprintf(sql,"insert into state values");
	sprintf(buff,"('%s','%d');",name.c_str(),fd);
	strcat(sql,buff);

	mysql_query(gMySqlServer.mpcon,sql);
}
void LoginModel::delOffline(string name)
{
	char sql[128] = {0};
	char buff[128] = {0};
	sprintf(sql,"delete from offline_msg where name = ");
	sprintf(buff,"'%s';",name.c_str());
	strcat(sql,buff);
	mysql_query(gMySqlServer.mpcon,sql);
}
/////////////////////////////////////////////////////////
void LoginView::process(int fd,Json::Value root)
{
	_fd = fd;
    _name = root["name"].asString();
    _pwd = root["pwd"].asString();
    
    if(_loginModel.checkUserAuth(_name,_pwd))
	{
		_loginState = true;
	}
    else
	{
		_loginState = false;
	}
}

void LoginView::response()
{
    Json::Value response;
    response["msgtype"] = "MSG_TYPE_ACK";
    if(_loginState)
    {
		_loginModel.addState(_name,_fd);
        response["reason"] = "login success!";
    }
    else
    {
        response["reason"] = "login fail:name or pwd invalid!";
    }
	//cout<<response.toStyledString()<<endl;
    send(_fd,response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()),0);

	if(_loginModel.checkOffline_msg(_name))
	{
		send(_fd,_loginModel.off_msg.c_str(),strlen(_loginModel.off_msg.c_str()),0);
		_loginModel.delOffline(_name);
	}
}
