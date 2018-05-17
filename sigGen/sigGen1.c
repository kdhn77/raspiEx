//-----------------sigGen1-------------------
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
	counter ++;
}

int main(int argc, char*argv[])
{
	int fd,byteCount;
	pid_t pid;
	char buf[10];
	int i;
	//시그널핸들러등록
	signal(SIGINT,sigHandler);
	//pid값을 읽어와서 파일에쓰기
	
	pid=getpid();
	fd = open("chkpid.txt",O_RDWR |O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	sprintf(buf,"%d",pid);
	byteCount=write(fd,buf,strlen(buf));
	close(fd);
	
	printf("send signal proc : %d\n",pid);
	pid=atoi(argv[1]);
	for(i=0;i<5;i++)
	{
		kill(pid,SIGINT);
		pause();
	}
	return 0;
}
