//my_kill
//Version of 21-12-2016
//Author: Louis Engelen
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int my_kill(unsigned int pid,int sig);

unsigned int  process_id;
int signum;
int Result;

//Termination signal and PID are main parameters
int main(int argc, char *argv[])
{

	printf("argv[1]=%s\n",argv[1]);
	signum=atoi(argv[1]); //Conversion from char to int
	printf("signum %d\n",signum);
	printf("argv[2]=%s\n", argv[2]);
	process_id=atoi(argv[2]);
	printf("pid %u\n",process_id);

	Result=my_kill(process_id, signum);//Return 0 if OK -1 if not
	printf("\nResult: %d \n",Result);
	return 0;
}

int sig;
unsigned int pid;
int result1;


//my_kill function implementation
int my_kill(unsigned int pid,int sig)

{
	//if kill OK result1=0, if not OK result1=-1
	printf("\nDebut switchcase");
	switch(sig)
		{
		case SIGTERM://Sigterm=15
		printf("\n Case Sigterm");
		result1=kill(pid,sig);
		return result1;
		break;

		case SIGSTOP://Sigstop=19
		printf("\n Case Sigstop");
		result1=kill(pid, sig);
		return result1;
		break;


		case SIGKILL://Sigkill=9
		printf("\n Case Sigkill");
		result1=kill(pid,sig);
		return result1;
		break;

		case SIGQUIT://Sigquit=3
		printf("\n Case SIQUIT");
		result1=kill(pid,sig);
		return result1;
		break;

		case SIGINT://Sigint=2
		printf("\n Case SIGINT");
		result1=kill(pid,sig);
		return result1;
		break;

		case SIGABRT://Sigabrt=6
		printf("\n case SIGABRT");
		result1=kill(pid,sig);
		return result1;
		break;

		default:
		printf("\n Default");
		result1=kill(pid, SIGTERM);
		return result1;
		break;

		}
}
