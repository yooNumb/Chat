#ifndef VIEW_H
#define VIEW_H
#include <json/json.h>
#include <string>
using namespace std;

class View
{
public:
    virtual void process(int fd,Json::Value root)=0;
    virtual void response()=0;
};

#endif
