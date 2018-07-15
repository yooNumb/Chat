#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <signal.h>
using namespace std;

void sig_fun(int sig)
{
	if(sig == SIGINT)
		exit(0);
}
int main()
{
	int sockfd;
	int count = 0;
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(8000);
	ser.sin_addr.s_addr = inet_addr("127.0.0.1");

	while(1)
	{
		sockfd = socket(AF_INET,SOCK_STREAM,0);

		if(connect(sockfd,(struct sockaddr*)&ser,sizeof(ser)) == -1)
		{
			break;
		}
		count++;
		printf("fd:%d, count:%d\n",sockfd,count);
	}
	signal(SIGINT,sig_fun);
	return 0;
}
