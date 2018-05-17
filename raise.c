#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigHandler(int sig)
{
	printf("raise() : I got signal %d\n",sig);
	(void)signal(SIGINT,SIG_DFL);//주석처리하면계속반복함..
}

int main(void)
{
	int count=0;
	signal(SIGINT,sigHandler);
	while(1)
	{
		printf("Hello world\n");
		count++;
		if(count==3)
		{
			raise(SIGINT);
			count=0;
		}
		sleep(1);
	}
}
