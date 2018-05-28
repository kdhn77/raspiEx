/*복사할파일을읽어오는동작*/

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
	void *shared_Mem=(void*)0; 
	char buf[BUF_SIZE];
	int shm_id;
	pid_t pid;
	char *shm_addr; 
	char *input_string;
	int *cpsize_addr;
	int status;
	int fd;
	int fd_chk;
	int fd_byte=0;
	
	//main인자값예외처리
	if(argc==1)
	{
		fprintf(stderr,"usage : %s [copyfile]\n");
		exit(EXIT_FAILURE);
	}
	
	//fork
	pid=fork();
	if(pid==-1)
		fprintf(stderr, "Error : fork()!\n");
	
	else if(pid==0)																							//child process
	{ 
		/*execute writeCopy.c*/
		printf("execute writeCopy.c\n");
		execl("./writeCopy","writeCopy",argv[1],NULL);
		printf("faild execute writeCopy.c!\n");
	}
	else 																											//parent process
	{ 
			//복사할대상파일열기
		fd=open(argv[1], O_RDONLY, S_IWUSR | S_IRUSR);
		if(fd == -1)
		{
			fprintf(stderr,"File Open Error\n");
			exit(EXIT_FAILURE);
		}
		//복사할대상파일크기체크
		while(1)
		{
			fd_chk=read(fd,buf,sizeof(buf));
			fd_byte+=fd_chk;
			if(fd_chk==0)
				break;
		}
		printf("fd_byte : %d\n",fd_byte);
		memset(buf,'\0',sizeof(buf));
		close(fd);
	
		//shmget
		shm_id=shmget((key_t)SHM_KEY, (size_t)SHM_SIZE, IPC_CREAT | 0666);
		if(shm_id==-1)
		{
			fprintf(stderr,"readCopy_shmget failed\n");
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
		//integer4byte라서1028번지부터int형주소를잡는다.
		cpsize_addr = (int*)(shared_Mem+BUF_SIZE+4);
		printf("cp_size_addr : %p\n",cpsize_addr);
		
		//복사할파일의데이터를buffer복사한후shm에입력
		//flag로동작 flag:0
		
		//file open
		fd=open(argv[1], O_RDONLY, S_IWUSR | S_IRUSR);
		if(fd == -1)
		{
			fprintf(stderr,"File Open Error\n");
			exit(EXIT_FAILURE);
		}

		input_string=shm_addr+1;
		shm_addr[0]=READ_FLAG;
		
		while(1)
		{
			if(shm_addr[0]==READ_FLAG)
			{
				fd_chk=read(fd,input_string,BUF_SIZE);
				*cpsize_addr=fd_chk;
				shm_addr[0]=WRITE_FLAG;
			}
			if(fd_chk==0)
			{
				shm_addr[0]=BREAK_FLAG;
				break;
			}
		}
		
		printf("data save complete\n");
		
		//wait for child process
		printf("wait for child process\n");
		printf("---------------------------------------------\n");
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

		close(fd);
	}
	exit(EXIT_SUCCESS);
}
	
