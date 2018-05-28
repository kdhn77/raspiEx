#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
volatile sig_atomic_t quitflag; //set nonzerno by signal handler

//one signal handler for SIGINT and SIGQUIT

static void sig_int(int signo)
{
	if(signo==SIGINT)
		printf("\ninterrupt\n");
	else if(signo==SIGQUIT)
		quitflag=1; //set flag main loop
}

int main(void)
{
	sigset_t newmask, oldmask, zeromask;
	
	if(signal(SIGINT,sig_int)==SIG_ERR)
		printf("signal SIGINT error");

	if(signal(SIGQUIT,sig_int)==SIG_ERR)
		printf("signal SIGQUIT error");

	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask,SIGQUIT);

	// Block SIGQUIT and save current signal mask
	
	if(sigprocmask(SIG_BLOCK,&newmask, &oldmask)<0)
		printf("SIG_BLOCK error");

	while(quitflag==0)
		sigsuspend(&zeromask);
	//SIGQUIT has been caught and is now blocked;do whatever
	quitflag=0;
	//Reset signal mask which unblocks SIGQUIT.
	if(sigprocmask(SIG_SETMASK,&oldmask,NULL)<0)
		printf("SIG_SETMASK error");

	exit(0);
}


