/*-----------------------------------------sigGen3.c------------------------------------------*/
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char*argv[])
{
	pid_t pid;
	int num=0;
	int loopcount=0;
	pid=atoi(argv[1]);
	printf("send signal proc : %d\n",pid);
	srand(time(NULL));
	int signo[2]={10,12};
	while(num!=2)
	{
		num=signo[rand()%2]; //0~1 rand
		if(loopcount==10)
			num=2;
		printf("input signal num : %d \n",num);
		kill(pid,num);
		loopcount++;
		//sleep(1);
	}
	return 0;
}
