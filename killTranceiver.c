#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int s, sig;
	if(argc !=3 || strcmp(argv[1],"--help")==0)
		printf("error : %s pid sig-num\n",argv[0]);
	sig = getInt(argv[2],0,"sig-num");
	s= kill(getLong(argv[1],0,"pid"),sig);

	if(sig!=0)
	{
		if(s==-1)
		{
			printf("kill\n");
			exit(1);
		}
	}
	else
	{
		if(s==0)
			printf("Process exists and we can send it a signal\n");
		else
		{
			if(errno==EPERM)
				printf("Process exists, but we don't have""Permission to send it a signal\n");
			else if(errno==ESRCH)
				printf("Process does not exits\n");
			else
			{
				printf("kill\n");
				exit(1);
			}
		}
	}
	exit(EXIT_SUCCESS);
}


