#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	char buffer[BUFSIZ];
	int fd[2];
	pipe(fd); 
	pid_t pid;
	pid=fork();
	if(pid==0) //child process
	{
		write(fd[1],"This is letter is from child",BUFSIZ);
		sleep(1);
		exit(0);
	}
	else //parent process
	{
		read(fd[0],buffer,BUFSIZ);
		printf("Output of parent process :: %s \n\n",buffer);
		exit(0);
	}
	exit(0);
}
