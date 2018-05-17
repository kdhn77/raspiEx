#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

static void sigHandler(int sig)
{
	static int count = 0;
	{
		static int count =0;
		{
			count++;
			printf("Caught SIGINT (%d)\n",count);
			return;
		}
	}
	printf("Caught SIGQUIT - that's all forks!\n");
	exit(0);
}

int main(void)
{
	if(signal(SIGINT,sigHandler)==SIG_ERR)
		printf("error");
	if(signal(SIGQUIT, sigHandler)==SIG_ERR)
		printf("error");
	for(;;)
	pause();
}
