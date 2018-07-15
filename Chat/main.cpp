#include <iostream>
#include <stdlib.h>
#include "tcpserver.h"
using namespace std;

int main(int argc,char **argv)
{
	if(argc < 4)
	{
		cerr<<"command args is invalid! format:.a.out ip port";
		exit(-1);
	}

	char *ip = argv[1];
	short port = atoi(argv[2]);
	int pth_num = atoi(argv[3]);

	TcpServer server(ip,port,pth_num);
	server.run();

	return 0;
}
