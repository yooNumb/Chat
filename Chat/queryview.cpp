#include "queryview.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include "public.h"
#include <sys/socket.h>
#include <iterator>

extern CMySqlServer gMySqlServer;
/////////////////////////////////////////////////////////
void QueryModel::onlineName()
{
	char sql[1024] = {0};
	sprintf(sql,"select * from state;");

	if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
	{
		return;
	}

	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	
	while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
	{
	    _onlinename.push_back(gMySqlServer.mrow[0]);
	}
}

void QueryView::process(int fd,Json::Value root)
{
	_fd = fd;
	_queryModel.onlineName();
	if( !(_queryModel._onlinename.empty()) )
	{
		_queryState = true;
	}
	else
	{
		_queryState = false;
	}
}

void QueryView::response()
{
    Json::Value response;
    response["msgtype"] = "MSG_TYPE_ACK";

    if(_queryState)
    {
		char buff[1024] = {0};
		vector<char *>::iterator it = _queryModel._onlinename.begin();
		for(;it!=_queryModel._onlinename.end();++it)
		{
			strcat(buff,*it);
			strcat(buff," ");
		}
        response["list"] = buff;
		_queryModel._onlinename.clear();
    }
    else
    {
        response["list"] = "no one online";
    }
  //  cout<<response.toStyledString()<<endl;
	send(_fd,response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()),0);
}
