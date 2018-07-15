#include <stdio.h>
#include <termios.h>

int main()
{
	printf("password:");
	fflush(stdout);

	struct termios oldts,newts;
	tcgetattr(0,&oldts);//初始化一个终端所对应的termios结构
	newts = oldts;
	newts.c_lflag &= ~ECHO;//本地模式与上回显取反//去掉回显功能
	newts.c_lflag &= ~ICANON;
	tcsetattr(0,TCSANOW,&newts);

	char passwd[128] = {0};
	//fgets(passwd,128,stdin);
	//	//passwd[strlen(passwd)-1] = 0;
	int c = 0;
	int count = -1;
	while((c=getchar()) != '\n')
	{
		if(c==127 && count>=0)
		{
			passwd[count--] = 0;
			printf("\033[%dD",1);//光标往前移一个位置
			printf("\033[K");//将当前光标以及往后的字符全部清空
			continue;
		}
		if(c == 127)
		{
			continue;
		}
		passwd[++count] = c;
		printf("*");
	}
	tcsetattr(0,TCSANOW,&oldts);//输入后面命令需要回显
	printf("%s",passwd);
	/*
	
	char lt[128] = {0};//存放分割出来的东西							
	int i = 0,num = 0;
	for(;i<strlen(sp->sp_pwdp);++i)										
	{							
		if(sp->sp_pwdp[i] == '$')									
		{															
			num++;			
			if(num == 3)
			{
				break;
			}
		}
		lt[i] = sp->sp_pwdp[i];//存放加密算法的编号和密钥
	}
	char *npw = crypt(passwd,lt);//把passwd按照lt的方式进行加密
	
	//printf("%s\n",npw);						
	if(strcmp(npw,sp->sp_pwdp) != 0)
	{
		printf("su:passwd error\n");
	}*/
}
