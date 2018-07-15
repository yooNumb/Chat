#ifndef REGISTERVIEW_H
#define REGISTERVIEW_H
#include "mysqlserver.h"
#include "view.h"
#include <string>
using namespace std;

class RegisterModel 
{
public:
    bool checkUserAuth(string name);
	void addUser(string name,string pwd);
};

class RegisterView : public View
{
public:
    void process(int fd,Json::Value root);
    void response();
private:
    string _name;
    string _pwd;
	int _fd;
    RegisterModel _registerModel;
    bool _registerState;
};
#endif
