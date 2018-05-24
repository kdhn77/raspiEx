/* writer */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define SHMSIZE 100

struct my_pid_st{
	long int my_msg_type;
	pid_t pid;
};


void sigHandler(int signo)
{
	//SIGUSR2에반응하는핸들러
}

int main(void)
{
	//msg queue관련변수선언
	int msgid;
	struct my_pid_st mypid;
	long int msg_to_receive=0;
	pid_t reader_pid;
	//shared memory관련변수선언
	void *shared_Mem=(void*)0;
	int shmid;
	int *shmaddr;
	int i;

	
	//시그널핸들러등록 message주고받는거확인을시그널로체크
	
	signal(SIGUSR2,sigHandler);
	
	msgid=msgget((key_t)1234,0666 |IPC_CREAT);
	if(msgid==-1)
	{
		fprintf(stderr,"msgget failed with error :%d\n",errno);
		exit(EXIT_FAILURE);
	}
	mypid.pid=getpid();
	printf("myPID : %d\n",mypid.pid);
	//send mypid
	mypid.my_msg_type = 1;
	if (msgsnd(msgid, (void *)&mypid, sizeof(mypid), 0) == -1) 
	{
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	printf("send the myPID to reader\n");
	
	//reader가신호를보내줄때까지대기
	pause();
	
		//recieve reader's pid
	if(msgrcv(msgid,(void*)&mypid,sizeof(mypid),msg_to_receive,0)==-1)
	{
		fprintf(stderr,"msgrcv failed with error: %d\n",errno);
		exit(EXIT_FAILURE);
	}
	reader_pid=mypid.pid;
	printf("received Reader's PID : %d\n",reader_pid);
	
	//message queue 해제
	if(msgctl(msgid,IPC_RMID,0)==-1)
	{
		fprintf(stderr,"msgctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
	printf("message queue cleared!\n");
	//1. shmget
	shmid = shmget((key_t)1234,sizeof(int)*SHMSIZE,0666 | IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr,"shmget failed\n");
		exit(EXIT_FAILURE);
	}

	//2. shmat
	shared_Mem=shmat(shmid,(void*)0,0);
	if(shared_Mem==(void*)-1)
	{
		fprintf(stderr,"shmat failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Memory attached at 0x%p\n",shared_Mem);
	shmaddr=(int*)shared_Mem;

	//3. memory access
	for(i=0;i<SHMSIZE;i++)
	{
		*(shmaddr+i)=i+1;
		printf("shmaddr:%p, data:%d\n",shmaddr+i,*(shmaddr+i));
	}
	
	//4. shmdt
	if(shmdt(shared_Mem)==-1)
	{
		fprintf(stderr,"shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	//공유메모리데이터입력후reader가읽도로시그널보냄
	kill(reader_pid,SIGUSR1);
	
	exit(EXIT_SUCCESS);
}
