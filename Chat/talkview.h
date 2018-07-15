#ifndef TALKVIEW_H
#define TALKVIEW_H
#include "mysqlserver.h"
#include "redis.h"
#include "view.h"
#include <string>
using namespace std;

class TalkModel 
{
public:
    bool checkUserAuth(string name);
	void addOffline_msg(string name,string msg);
	int mfd;
};

class TalkView : public View
{
public:
    void process(int fd,Json::Value root);
    void response();
private:
    string _name;
    string _msg;
	string _sendname;
	int _fd;
    TalkModel _talkModel;
    bool _talkState;
};
#endif
