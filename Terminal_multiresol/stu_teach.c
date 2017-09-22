#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "global.h"
#include <string.h>
#include <errno.h>
#include <sys/types.h>   
#include <sys/wait.h> 
#include <fcntl.h>   
#include <limits.h>
#include "global.h"


#define  FILE_BEGIN_CLASS   "/tmp/begin_class"
#define  FILE_FINISH_CLASS  "/tmp/finish_class"
static pthread_t tid;
static pthread_t tid_finish;

//extern int g_mainExit;
extern int ShenCloud_classLogin();
  
  
#define BUF_SIZE 150   
  
void err_quit(char *msg)   
{   
    perror(msg);   
    LogInfo("detect_process, err_quit popen command error.\n");   
}   
  
int detect_process()
{   
	int nRet = 0;
	FILE* fp;
	int count;
	char buf[BUF_SIZE];
	char command[BUF_SIZE];
	sprintf(command, "ps -ef | grep spicy | grep -v grep | wc -l" );

	if((fp = popen(command,"r")) == NULL)
		err_quit("popen");

	if( (fgets(buf, BUF_SIZE, fp))!= NULL )
	{  
		count = atoi(buf);
		if(count  == 0)
		{
		   LogInfo("detect_process, spicy not found .\n");
		   nRet = 0;
		}
		else
		{
		    nRet = 1;
		}
	}   
	pclose(fp);
	return nRet;
}  


int IsFileExist(const char * file)
{
	if (file == NULL)
	{
		LogInfo("monitor file is NULL.\n");
		return -1;
	}
	else if (access(file, F_OK) < 0)
	{
		return 0;
	}else 
	{
		LogInfo("monitor file exist.\n");
		return 1;
	}
}

int operator_stu()
{
	FILE *fp;
	fp=fopen(FILE_FINISH_CLASS,"r+"); 
	if (fp == NULL)
	{
		LogInfo("del operator_stu /tmp/finish_class file Error:%s\a\n", strerror(errno));
		return -1;
	}
	char szTmp[10] = {0};
	while (fgets(szTmp, 10, fp) != NULL)
	{
		if (strcmp(szTmp, "" ) > 0)
		{
			int nflag = atoi(szTmp);
			if (nflag == 1)
			{
				system("sudo killall -9 spicy");
			}
			if (nflag == 2 || nflag == 3)
			{
				system("sudo poweroff");
			}
		}
		sleep(3);
	}
	fclose(fp);
	return 0;
}
void thrd_monitor(char *cmd)
{
	struct LoginInfo info = {"", "", "", "", 3389, 0, 0, 0}; //add by linyuhua
	
	for (;;)
	{
	    if (g_mainExit)
	    	{
			LogInfo("stu_teacer monitor file thrd exit.\n");
			break;
		}
		if (IsFileExist(FILE_BEGIN_CLASS) == 1)
		{
			//do code;
			g_stu_begin_class = 1;
			printf("thrd_monitor g_stu_begin_class = %d.\n", g_stu_begin_class);
				//---------- add by linyuhua--------
				GetLoginInfo(&info);
				if(!(strstr(info.user, "tec_") || strstr(info.user, "tec-") || strstr(info.user, "teacher") || detect_process())){
						ShenCloud_classLogin();
				}
				//---------------- end -------------
				//ShenCloud_classLogin();
			g_stu_begin_class = 0;
			if( remove(FILE_BEGIN_CLASS) == 0 )
				LogInfo("del /tmp/begin_class file success.\n");
			else
				LogInfo("del /tmp/begin_class file Error:%s\a\n", strerror(errno));
		}
		sleep(5);
	}
}

void thrd_monitor_finish()
{
	for (;;)
	{
	    if (g_mainExit)
	    	{
			LogInfo("stu_teacer monitor file thrd exit.\n");
			break;
		}
		if (IsFileExist(FILE_FINISH_CLASS) == 1)
		{
			//do code;
			g_stu_finish_class = 1;
			printf("thrd_monitor g_stu_finish_class = %d.\n", g_stu_finish_class);
			operator_stu();
			g_stu_finish_class = 0;
			if( remove(FILE_FINISH_CLASS) == 0 )
				LogInfo("del /tmp/finish_class file success.\n");
			else
				LogInfo("del /tmp/finish_class file Error:%s\a\n", strerror(errno));
		}
		sleep(3);
	}
}
	
void start_monitor_file_thrd()
{
	if ( pthread_create(&tid, NULL, thrd_monitor, NULL) != 0 ) {
	//fprintf(stderr,"pthread_create Error:%s\a\n",strerror(errno));
		LogInfo("pthread_create monitor file Error:%s\a\n", strerror(errno));
		return -1;
	}
	if ( pthread_create(&tid_finish, NULL, thrd_monitor_finish, NULL) != 0 ) {
	//fprintf(stderr,"pthread_create Error:%s\a\n",strerror(errno));
		LogInfo("pthread_create thrd_monitor_finish file Error:%s\a\n", strerror(errno));
		return -1;
	}
}
