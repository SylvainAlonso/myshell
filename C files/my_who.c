// -m -q
//Version of 21-12-2016
//Author: Louis Engelen, 15338, 4MA
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <utmp.h>
#include <sys/utsname.h>
#include <time.h>
#include <getopt.h>

//Functions prototypes
void my_who();
void m_opt();
void q_opt();

int main(int argc, char *argv[])
{
	int mflag=0, qflag=0, opterr=0;
	char *cvalue=NULL;
	int  c;
	printf("enter while\n");
	//Option acquisition
	while((c=getopt(argc,argv,"mq")) != -1)
	{
	printf("debut switch\n");
	switch(c){
	case 'm':
		mflag=1;printf("mflag %d\n",mflag); break;
	case 'q':
		qflag=1;printf("qflag %d\n",qflag); break;
	default:
		printf("error\n"); break;
		}
	}
	//Function select
	if(mflag==1){printf("if1\n");m_opt();}
	if(qflag==1){printf("if2\n");q_opt();}
	if(mflag==0 && qflag==0) {printf("if3\n"); my_who();}

	return 0;
}

//Function without option
void my_who()
{
	struct utmp *info;
	char tmps[80];

	setutent(); //Go back at the beginning of utmp file
	info=getutent();//Read the utmp file
	//Print desired content of utmp file
	while(info!=NULL)
		{
	        if(info->ut_type==7)
        		{
        		printf("%-9s",info->ut_user);
        		printf("%-12s",info->ut_line);
            		time_t *time;
	    		time = &info->ut_time;
	    		strcpy(tmps,ctime(time));
            		printf("%s",tmps);
            		printf(" ("); printf("%s",info->ut_host);printf(")\n");
        		}
       		 info=getutent();
    		}
	endutent();
}
//Print the users logged in and their number
void q_opt()
{
	struct utmp *info;
	int user_cpt=0;

	setutent();
	info=getutent();
	//Print the desired content of utmp file
	while(info!=NULL)
		{
        	if(info->ut_type==7)
        		{
       			printf("%-9s \n",info->ut_user);
			user_cpt++;
        		}
       		info=getutent();
    		}
	printf("Users: %d\n",user_cpt);
	endutent();
}

//Print time of last system boot
void m_opt()
{
	struct utmp *info;
	time_t current_time, start_time;
	struct tm *y2k;
	char date[20];

	setutent();
	info=getutent();
	while(info!=NULL)
		{
		if(info->ut_type==7)
			{
			current_time=time(NULL);
			start_time=info->ut_tv.tv_sec;
			y2k=localtime(&start_time);
			strftime(date,20,"%F %R", y2k);
			printf("System boot %s\n", date);
			}
		}
}
