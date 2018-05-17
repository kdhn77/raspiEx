/*-----------------------------------------sigGen3.c------------------------------------------*/
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char*argv[])
{
	pid_t pid;
	int num=0;
	pid=atoi(argv[1]);
	printf("send signal proc : %d\n",pid);
	while(num!=2)
	{
		printf("input signal num :");
		scanf("%d",&num);
		kill(pid,num);
		getchar();
	}
	return 0;
}
