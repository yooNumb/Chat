#ifndef LOGINVIEW_H
#define LOGINVIEW_H
#include "mysqlserver.h"
#include "view.h"
#include <string>
using namespace std;

class LoginModel 
{
public:
    bool checkUserAuth(string name,string pwd);
	bool checkOffline_msg(string name);
	void addState(string name,int fd);
	void delOffline(string name);

	string off_msg;
};

class LoginView : public View
{
public:
    void process(int fd,Json::Value root);
    void response();
private:
    string _name;
    string _pwd;
	int _fd;
    LoginModel _loginModel;
    bool _loginState;
};
#endif
