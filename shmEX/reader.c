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

#define SHM_SIZE  1024
#define SHM_KEY 0x1234

int main()
{
	int fd;
	int *shm_addr; 
	void *shared_Mem=(void*)0; 
	int shm_id;
	int i;
	char buffer[BUFSIZ];
	memset(buffer,'\0',sizeof(buffer));
	
	//file open
	fd=open("reader.txt", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd==-1)
		fprintf(stderr,"reader.txt open failed!\n");
		
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
	
	//read shm_addr(1~1024)
	for(i=0;i<SHM_SIZE;i++)
	{
		if(i==0||i==1023)
			printf("read shm_addr:%p, data:%d\n", shm_addr+i,*(shm_addr+i));
		sprintf(buffer,"read shm_addr:%p, data:%d\n", shm_addr+i,*(shm_addr+i));
		write(fd,buffer,strlen(buffer));
		memset(buffer,'\0',sizeof(buffer));
	}
	printf("print reader.txt\n");
	printf("shared memory detached by reader\n");
	if(shmdt(shared_Mem)==-1)
	{
		fprintf(stderr,"shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	close(fd);
	printf("---------------------------------------------\n");
	
	exit(EXIT_SUCCESS);
}
