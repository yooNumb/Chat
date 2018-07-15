#ifndef QUERYVIEW_H
#define QUERYVIEW_H
#include "mysqlserver.h"
#include "view.h"
#include <string>
#include <vector>
using namespace std;

class QueryModel
{
	public:
		void onlineName();
		vector<char *> _onlinename;
};

class QueryView : public View
{
public:
    void process(int fd,Json::Value root);
    void response();
private:
	QueryModel _queryModel;
	int _fd;
	bool _queryState;
};

#endif
