#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
 
#define BUFSIZE 1024

int main(int argc, char **argv)
{
    int byteCount, fdIn, fdOut;
    char buf[BUFSIZE];
 	struct timeval StartTime, EndTime, ProcTime;
	clock_t clockTime;
	static long clockTicks = 0;
	struct tms t;

    if(argc <3)
    {
        //write(2, "Usage : copy file1 file2\n", 25);
        perror("Usgae : copy file 1 file2");
        return -1;
    }



	clockTicks = sysconf(_SC_CLK_TCK);

	// clock()로 시작 시간 기록
	clockTime = clock();
	if(clockTime == -1)
		printf("Error : Syscall Clock()\n");
	printf("clock() returns : %ld\n",(long) clockTime/CLOCKS_PER_SEC);
	
	// times()로 시작 시간 기록
	if(times(&t) == -1)
		printf("Error : Syscall times()\n");

	printf("times() user   CPU=%.2f\n",(double)t.tms_utime / clockTicks);
	printf("times() system CPU=%.2f\n",(double)t.tms_stime / clockTicks);
	
	// gettimeofday()로 시작 시간 기록
	gettimeofday(&StartTime, NULL);

	 
    // 원본파일 열기
    if((fdIn = open(argv[1], O_RDONLY))<0)
    {
        perror(argv[1]);
        return -1;
    }

    // 새로 생성되는 파일 열기
    if((fdOut = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR))<0)
    {
        perror(argv[2]);
        return -1;
    }

    while((byteCount=read(fdIn,buf,sizeof(buf)))>0)
        write(fdOut, buf, byteCount);

    close(fdIn);
    close(fdOut);
    
	//gettimeofday로 종료 시간 기록
	gettimeofday(&EndTime, NULL);

	clockTime = clock();
	if(clockTime == -1)
		printf("Error : Syscall clock()\n");
	printf("clock() returns : %ld\n",(long) clockTime/CLOCKS_PER_SEC);

	if(times(&t) == -1)
		printf("Error : Syscall times()\n");

	printf("times() user   CPU=%.2f\n",(double)t.tms_utime / clockTicks);
	printf("times() system CPU=%.2f\n",(double)t.tms_stime / clockTicks);



	if((EndTime.tv_usec-StartTime.tv_usec)<0)
	{
		ProcTime.tv_sec  = EndTime.tv_sec - StartTime.tv_sec - 1;
		ProcTime.tv_usec = 1000000 + EndTime.tv_usec - StartTime.tv_usec;
	} 
	else
	{
		ProcTime.tv_sec  = EndTime.tv_sec - StartTime.tv_sec;
		ProcTime.tv_usec = EndTime.tv_usec - StartTime.tv_usec; 
	}

	printf("Copy Time : %ld.%ldusec\n",ProcTime.tv_sec, ProcTime.tv_usec);
    return 0;
}
