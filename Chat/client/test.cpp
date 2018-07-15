#include "md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
using namespace std;

int main()
{
	string str = "123";
	const char *p = str.c_str();
	int i;
	unsigned char encrypt[40] = {0};
	strcpy((char *)encrypt,p);//21232f297a57a5a743894a0e4a801fc3
	unsigned char decrypt[16];    
	MD5_CTX md5;
	MD5Init(&md5);         		
	MD5Update(&md5,encrypt,strlen((char *)encrypt));
	MD5Final(&md5,decrypt);        
	printf("加密前:%s\n加密后:",encrypt);
	for(i=0;i<16;i++)
	{
		printf("%02x",decrypt[i]);													}
	return 0;
}
