/* send */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msg_st {
	 long msg_type;
	 char name[10];
	 int age;
	 int id;
};
int main()
{
	int running = 1;
	struct my_msg_st some_data;
	int msgid;
	char buffer[BUFSIZ];
	
	memset(buffer,0,BUFSIZ);
	
	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
	
	if (msgid == -1) 
	{
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
	while(running) 
	{
		some_data.msg_type = 1;
		
		//input name
		printf("Enter the name: ");
		scanf("%s",some_data.name);
		
		//input age
		printf("Enter the age: ");
		scanf("%d",&some_data.age);
		
		//input id
		printf("Enter the id: ");
		scanf("%d",&some_data.id);
		
		printf("continue? (yes/no) : ");
		scanf("%s",buffer);
		if (strncmp(buffer,"no",2)==0) 
		{
				running = 0;
				some_data.msg_type=2;
		}
		if (msgsnd(msgid, (void *)&some_data, sizeof(some_data), 0) == -1) 
		{
			fprintf(stderr, "msgsnd failed\n");
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}

