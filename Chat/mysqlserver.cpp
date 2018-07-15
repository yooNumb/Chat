#include "mysqlserver.h"
#include <iostream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>

CMySqlServer::CMySqlServer()
{
	//init mysql connection
	mpcon = mysql_init((MYSQL*)0);
	if(mpcon == NULL)
	{
		cout<<"mysql init error!"<<endl;
		throw "";
	}
	if(!mysql_real_connect(mpcon,"127.0.0.1",
				"root","123456",NULL,3306,NULL,0))
	{
		cout<<"mysql connection error!"<<endl;
		throw "";
	}
	if(mysql_select_db(mpcon,"Chat"))
	{
		cout<<"mysql select db error!"<<endl;
		throw "";
	}
	cout<<"mysql server init success!"<<endl;
}

CMySqlServer::~CMySqlServer()
{
	if(mpres != NULL)
	{
		mysql_free_result(mpres);
	}
	if(mpcon != NULL)
	{
		mysql_close(mpcon);
	}
}
CMySqlServer gMySqlServer;
