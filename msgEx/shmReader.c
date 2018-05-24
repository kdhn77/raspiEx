/* Reader */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/stat.h>

#define SHMSIZE 100

struct my_pid_st{
	long int my_msg_type;
	pid_t pid;
};

	
void sigHandler(int signo)
{
	//SIGUSR1에반응하는핸들러
}

int main(void)
{
	//msg queue관련변수선언
	int running=1;
	int msgid;
	struct my_pid_st mypid;
	long int msg_to_receive=0;
	pid_t writer_pid;
	
	//sharedmemory관련변수선언
	void *shared_Mem=(void*)0;
	int shmid;
	int *shmaddr;
	int i;
	//시그널핸들러등록 공유메모리입력확인을시그널로받음
	signal(SIGUSR1,sigHandler);
	
	msgid=msgget((key_t)1234,0666 |IPC_CREAT);
	if(msgid==-1)
	{
		fprintf(stderr,"msgget failed with error :%d\n",errno);
		exit(EXIT_FAILURE);
	}
	//recieve writer's pid
	if(msgrcv(msgid,(void*)&mypid,sizeof(mypid),msg_to_receive,0)==-1)
	{
		fprintf(stderr,"msgrcv failed with error: %d\n",errno);
		exit(EXIT_FAILURE);
	}
	writer_pid=mypid.pid;
	printf("received Writer's PID : %d\n",writer_pid);
	
	//send mypid
	mypid.my_msg_type = 2;
	mypid.pid=getpid();
	printf("myPID : %d\n",mypid.pid);
	if (msgsnd(msgid, (void *)&mypid, sizeof(mypid), 0) == -1) 
	{
		fprintf(stderr, "msgsnd failed\n");
		exit(EXIT_FAILURE);
	}
	printf("send the myPID to writer\n");
	
	//mypid를보내고writer동작하도록깨움.
	kill(writer_pid,SIGUSR2);
	
	pause();
	//writer가데이터를입력하고시그널을보낸것을받고데이터읽기시작한다.
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
		printf("shmaddr:%p, data:%d\n",shmaddr+i,*(shmaddr+i));
	}

	//4. shmdt
	if(shmdt(shared_Mem)==-1)
	{
		fprintf(stderr,"shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	
	//5. shmctl : IPC_RMID
	if(shmctl(shmid,IPC_RMID,0)==-1)
	{
		fprintf(stderr,"shmctl(IPC_RMID) failed \n");
		exit(EXIT_FAILURE);
	}
	printf("shared memory cleared!\n");
	exit(EXIT_SUCCESS);
}
