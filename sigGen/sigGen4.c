/*-----------------------------------------sigGen4.c------------------------------------------*/
#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

char buf[BUFSIZ];
char tbuf[BUFSIZ];
int chk_sw2=0;
void sigHandler(int signo)
{
	time_t UTCtime;
	struct tm *tm;
	time(&UTCtime);;
	tm=localtime(&UTCtime);
	 strftime(tbuf,sizeof(tbuf),"%Y-%m-%d %H:%M:%S",tm);//yyyy-mm-dd hh:mm:ss
	switch(signo)
	{
	case 2:
		chk_sw2=1;
		sprintf(buf,"%s [SIGINT]\n",tbuf);
		printf("Catch 2\n");
		break;
	case 10:
		sprintf(buf,"%s [SIGUSR1]\n",tbuf);
		printf("Catch 10\n");
		break;
	case 12:
		sprintf(buf,"%s [SIGUSR2]\n",tbuf);
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
		if(chk_sw2==1)
			break;
	}
	close(fd);
	return 0;
}

