/*복사할데이터를쓰는동작*/

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
#include "shm_define.h"

int main(int argc, char *argv[])
{
	int fd;
	char *shm_addr; 
	void *shared_Mem=(void*)0; 
	int shm_id;
	int fd_chk;
	char buf[BUF_SIZE];
	int *cpsize_addr;
	char *output_string;
	memset(buf,'\0',sizeof(buf));
	sprintf(buf,"copy_%s",argv[1]);
	
	//file open
	fd=open(buf, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd==-1)
		fprintf(stderr," %s open failed!\n",buf);
	memset(buf,'\0',sizeof(buf));
	//shmget
	shm_id=shmget((key_t)SHM_KEY, (size_t)SHM_SIZE, IPC_CREAT | 0666);
	if(shm_id==-1)
	{
		fprintf(stderr,"writeCopy_shmget failed\n");
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
	cpsize_addr = (int*)(shared_Mem+BUF_SIZE+4);
	
	
	//공유메모리를읽어서 파일에쓰기
	//flag : 1
	
	output_string=shm_addr+1; 

	while(1)
	{
		if(shm_addr[0]==WRITE_FLAG)
		{	
				fd_chk=*cpsize_addr;
				write(fd,output_string,fd_chk);
				shm_addr[0]=READ_FLAG;	
		}
		if(shm_addr[0]==BREAK_FLAG)
				break;
	}
	close(fd);
		
	printf("copy finish\n");
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

