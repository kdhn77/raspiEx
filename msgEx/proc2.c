/* here's the receiver program */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>

#define FILE_NAME "test.bin"

struct my_msg_st {
	long msg_type;
	char name[10];
	int age;
	int id;
};

int main()
{
	int running=1;
	int msgid;
	int fd, size;
	struct my_msg_st some_data;
	long int msg_to_receive=0;
	char buffer[BUFSIZ];
	//first, we set up the message queue
	msgid=msgget((key_t)1234,0666 |IPC_CREAT);

	if(msgid==-1)
	{
		fprintf(stderr,"msgget failed with error :%d\n",errno);
		exit(EXIT_FAILURE);
	}

	/*then the messages are retrived from the queue, until an end message
	 is encountered. Lastly, the message queue is deleted. */
	
	fd = open(FILE_NAME, O_WRONLY | O_CREAT | O_EXCL, 0666);
	if(fd<0)
	{
		fprintf(stderr, "can't open test.bin : %d\n", errno);
		exit(EXIT_FAILURE);
	}
	
	while(running)
	{
		if(msgrcv(msgid,(void*)&some_data,BUFSIZ,msg_to_receive,0)==-1)
		{
			fprintf(stderr,"msgrcv failed with error: %d\n",errno);
			exit(EXIT_FAILURE);
		}
		printf("type:%ld\n",some_data.msg_type);
		printf("name: %s\n", some_data.name);
		printf("age: %d\n", some_data.age);
		printf("id: %d\n", some_data.id);
		
		//sprintf(buffer,"name : %s\nage : %d\nid : %d\n",some_data.name,some_data.age,some_data.id);
		//size = write(fd,buffer,strlen(buffer));
		size = write(fd,&some_data,sizeof(some_data));
		
		if(size<0)
			fprintf(stderr,"Error : write() %d\n",errno);
		if(some_data.msg_type==2)
			running=0;
	}
	close(fd);
	if(msgctl(msgid,IPC_RMID,0)==-1)
	{
		fprintf(stderr,"msgctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}

