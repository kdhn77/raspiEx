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
#define SHM_SIZE  33
#define BUF_SIZE 32
#define SHM_KEY 0x1234

int main()
{
	int fd;
	char *shm_addr; 
	void *shared_Mem=(void*)0; 
	int shm_id;
	int fd_chk;
	char buf[BUF_SIZE];
	memset(buf,'\0',sizeof(buf));
	
	//shmget
	shm_id=shmget((key_t)SHM_KEY, sizeof(char)*SHM_SIZE, IPC_CREAT | 0666);
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
 	shm_addr = (char*)shared_Mem;
	
	//*shm_addr='0';
	printf("%s",*shm_addr);

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
	
	
	exit(EXIT_SUCCESS);
}

