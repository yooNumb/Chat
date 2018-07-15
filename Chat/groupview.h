#ifndef GROUPVIEW_H
#define GROUPVIEW_H
#include "mysqlserver.h"
#include "view.h"
#include <string>
#include <vector>
using namespace std;

class GroupModel 
{
public:
    void Getfd();
	vector<int> mfd;
};

class GroupView : public View
{
public:
    void process(int fd,Json::Value root);
    void response();
private:
    string _msg;
	string _sendname;
	int _fd;
    GroupModel _groupModel;
    bool _groupState;
};
#endif
