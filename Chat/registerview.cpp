#include "registerview.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "public.h"
#include "md5.h"

extern CMySqlServer gMySqlServer;

////////////////////////////////////////////////////////
bool RegisterModel::checkUserAuth(string name)
{
    char sql[1024]={0};
    sprintf(sql, "select * from user;");
    
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
			return true;
        }
    }
    return false;
}
void RegisterModel::addUser(string name,string pwd)
{
	char buff[128] = {0};
	char buff1[128] = {0};

	sprintf(buff,"insert into user values ");

	//将用户的密码用md5加密
	const char *p = pwd.c_str();
	unsigned char encrypt[40] = {0};
	strcpy((char *)encrypt,p);
	unsigned char decrypt[16];
	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5,encrypt,strlen((char *)encrypt));
	MD5Final(&md5,decrypt);

	char buf[33] = {0};
	char tmp[3] = {0};
	for(int i=0;i<16;i++)
	{
		sprintf(tmp,"%02x",decrypt[i]);
		strcat(buf,tmp);
	}
//	printf("md5:%s\n",buf);
	sprintf(buff1,"('%s','%s');",name.c_str(),buf);
	strcat(buff,buff1);
	
	mysql_query(gMySqlServer.mpcon,buff);
}
/////////////////////////////////////////////////////////
void RegisterView::process(int fd,Json::Value root)
{
	_fd = fd;
    _name = root["name"].asString();
    _pwd = root["pwd"].asString();
    
    if(_registerModel.checkUserAuth(_name))
	{
		_registerState = false;
	}
    else
	{
		_registerState = true;
		_registerModel.addUser(_name,_pwd);
	}
}

void RegisterView::response()
{
    Json::Value response;
    response["msgtype"] = "MSG_TYPE_ACK";
    if(_registerState)
    {
        response["reason"] = "register sussess";
    }
    else
    {
        response["reason"] = "register fail";
    }
//	cout<<response.toStyledString()<<endl;
    send(_fd,response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()),0);
}
