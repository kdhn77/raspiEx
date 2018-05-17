//--------------sigGen2-----------------
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
void sigHandler(int signo)
{
	static int counter = 1;
	printf("sig no : %d, counter : %d \n",signo,counter);
	counter ++;;
}

int main(int argc, char*argv[])
{
	int fd,byteCount;
	pid_t pid;
	char buf[10];
	int i;
	//시그널핸들러등록
	signal(SIGINT,sigHandler);
	pause();
	//pid값을 read
	fd = open("chkpid.txt",O_RDWR );
	
	byteCount=read(fd,buf,10);
	if(byteCount==0)
		printf("Can't read chkpid.txt file.\n");

	close(fd);
	
	pid=atoi(buf);
	printf("pid : %d\n",pid);
	kill(pid,SIGINT);
	for(i=0;i<4;i++)
	{
		
		pause();
		kill(pid,SIGINT);
	}

	return 0;
}
