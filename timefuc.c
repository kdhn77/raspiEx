#include <stdio.h>
#include <time.h>
#include <sys/time.h> //gettimeofday()함수에서사용
#include <stdlib.h>

int main(int argc, char **argv)
{
	int i, j;
	time_t UTCtime;
	struct tm *tm;
	char buf[BUFSIZ];
	struct timeval UTCtime_u;

	time(&UTCtime);//UTC현재시간구하기
	printf("time : %u\n",(unsigned)UTCtime);//UTC현재시간출력
	
	gettimeofday(&UTCtime_u,NULL);//UTC현재시간구하기(마이크로초까지)
	printf("gettimeofday : %ld/%d\n",UTCtime_u.tv_sec,UTCtime_u.tv_sec);

	printf("ctime :%s",ctime(&UTCtime)); //현재의시간을문자열로바꿔서출력

	//putenv("TZ=PST3PDT");//환경변수를설정한다.
	//tzset(); //TZ변수를설정한다.
	
	tm=localtime(&UTCtime); // tm=gmtime(&UTCtime);
	printf("asctime : %s", asctime(tm));//현재의시간을tm구조체를이용하여출력
	
	strftime(buf,sizeof(buf),"%A %m %e %H:%M:%S %Y",tm);//사용자정의문자열지정
	printf("strftime : %s\n",buf);
	return 0;
}
