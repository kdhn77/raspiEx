/*-----------------------------------------sigGen4.c------------------------------------------*/
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

char buf[100];
void sigHandler(int signo)
{
	switch(signo)
	{
	case 2:
		sprintf(buf,"Catch SIGINT\n");
		printf("Catch 2\n");
		break;
	case 10:
		sprintf(buf,"Catch SIGUSR1\n");
		printf("Catch 10\n");
		break;
	case 12:
		sprintf(buf,"Catch SIGUSR2\n");
		printf("Catch 12\n");
		break;
	default:
		printf("I don't know current signal!\n");
		break;
	}
}

int main(void)
{
	int fd, byteCount;
	signal(SIGINT,sigHandler);
	signal(SIGUSR1,sigHandler);
	signal(SIGUSR2,sigHandler);
	fd = open("./log.txt",O_RDWR |O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	
	while(1)
	{
		pause();
		byteCount=write(fd,buf,strlen(buf));
		if(strcmp(buf,"Catch SIGINT\n")==0)
			break;
	}
	close(fd);
	return 0;
}

