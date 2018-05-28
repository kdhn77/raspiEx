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
#include <fcntl.h>
#include <wait.h>
#define SHM_SIZE  1024
#define SHM_KEY 0x1234


static void sigHandler(int signo)
{
    if(signo==SIGUSR1)
       printf("SIGUSR1\n");
}

int main()
{
	void *shared_Mem=(void*)0; 
	int shm_id;
	int i;
	pid_t pid;
	int *shm_addr; 
	int status;
	
	//fork
	pid=fork();
	if(pid==-1)
		fprintf(stderr, "Error : fork()!\n");
	else if(pid==0)		//child process
	{ 
		signal(SIGUSR1, sigHandler);
		printf("pause child\n");
		//recieve signal
		pause();
		printf("child process woke up!\n");
		
		/*execute reader.c*/
		printf("execute reder.c\n");
		execl("./reader","reader",NULL);
		printf("faild execute reader.c!\n");
	}
	else 	//parent process
	{ 
		//shmget
		shm_id=shmget((key_t)SHM_KEY, sizeof(int)*SHM_SIZE, IPC_CREAT | 0666);
		if(shm_id==-1)
		{
			fprintf(stderr,"shmget failed\n");
			exit(EXIT_FAILURE);
		}
			//shmat
		shared_Mem = shmat(shm_id,(void *)0, 0);
		if(shared_Mem ==(void *)-1)
		{
			fprintf(stderr,"shmat failed\n");
			exit(EXIT_FAILURE);
		}
		printf("Memory attached at 0x%p\n",shared_Mem);
		shm_addr = (int*)shared_Mem;
 	
		//write shm_addr(1~1024)
		for(i=0 ; i<SHM_SIZE ; i++)
		{
			*(shm_addr+i)=i+1;
			if(i==0||i==1023)
				printf("write shm_addr:%p, data:%d\n", shm_addr+i,*(shm_addr+i));
		}
		printf("writing is complete\n");
		
		//kill child process & wait for child process
		printf("wait for child process\n");
		printf("---------------------------------------------\n");
		kill(pid,SIGUSR1); //this pid is child's PID
		wait(&status);
		printf("child process finish!\n");
		
		//detach & IPC_RMID
		if(shmdt(shared_Mem)==-1)
		{
			fprintf(stderr,"shmdt failed\n");
			exit(EXIT_FAILURE);
		}
		if(shmctl(shm_id, IPC_RMID,0)==-1)
		{
			fprintf(stderr,"shmctl(IPC_RMID) failed\n");
			exit(EXIT_FAILURE);
		}
		printf("shared memory detached and IPC_RMID by writer\n");
		printf("parent process finish!\n");
	}
	exit(EXIT_SUCCESS);
}
	
