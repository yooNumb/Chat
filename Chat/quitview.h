#ifndef QUITVIEW_H
#define RQUITVIEW_H
#include "mysqlserver.h"
#include "redis.h"
#include "view.h"
#include <string>
using namespace std;

class QuitModel 
{
public:
    bool checkUserAuth(int fd);
	void delUserState(int fd);
};

class QuitView : public View
{
public:
    void process(int fd,Json::Value root);
    void response();
private:
    string _name;
	int _fd;
    QuitModel _quitModel;
    bool _quitState;
};
#endif
