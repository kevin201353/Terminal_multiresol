/***********
//filename: Loginfo.c
//author: zhaosenhua
//version: 1.0
//funciton list:LogInfo()
//description: 写日志， 记录程序Debug信息
//Date: 2016/6/23
**************/
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "global.h"


#define MAX_BUFFER_SZIE   10240
#define FILE_DEBUG_LOG_PATH    "/var/log/shencloud/shencloud_ui.log"   //日志文件名
static void *thrd_func(void *arg);
static pthread_t tid;

#define   PATH_COOKIES_FILE   "/tmp/cookies.txt"

//extern int g_mainExit;
void LogInfo(const char* ms, ... )
{
	if (g_thinviewlog == 0)
		return;
	char wzLog[MAX_BUFFER_SZIE] = {0};
	char buffer[MAX_BUFFER_SZIE] = {0};
	va_list args;
	va_start(args, ms);
	vsprintf( wzLog ,ms,args);
	va_end(args);
	time_t now;
	time(&now);
	struct tm *local;
	local = localtime(&now);
	/*
	printf("%04d-%02d-%02d %02d:%02d:%02d %s\n", local->tm_year+1900, local->tm_mon + 1,
			local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,
			wzLog);
	*/
	sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d %s\n", local->tm_year+1900, local->tm_mon + 1,
				local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec,
				wzLog);
	FILE* file = fopen(FILE_DEBUG_LOG_PATH,"a+");
	if (file != NULL)
	{
		fwrite(buffer,1,strlen(buffer),file);
		fclose(file);
	}
	return ;
}
  
unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;      
    struct stat statbuff;  
    if(stat(path, &statbuff) < 0)
	{
	    char szTmp[100] = {0};
		sprintf(szTmp, "Error: log get file = %s size failed.\n", path);
		LogInfo(szTmp);
        return filesize;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
}

static char szCmd[100] = {0};

static del_back_file(char *path)
{
	unsigned long shlog_size = get_file_size(path);
	//LogInfo("debug: del_back_file:  file size xxxxxxx = %d.\n", shlog_size);
	int compare_size = (int)shlog_size/(1024 *1024); //mb
	if (compare_size > 10)
	{
        LogInfo("debug: start open back file 00000 .\n", shlog_size);
		remove(path);
		LogInfo("debug: start del back file xxxxxxx .\n", shlog_size);
	}
}

static void *thrd_func(void *arg)
{
    //LogInfo("debug: checking_log New process11 :  PID: %d, TID: %u.\n", getpid(), tid);
    for(;;)
    {
       if (g_mainExit == 1)
	   	  break;
      del_back_file(FILE_DEBUG_LOG_PATH);
	  del_back_file("/var/log/shencloud/spicy.log");
#ifdef ARM
	 sleep(3);
#else
      sleep(3);
#endif
    }
    pthread_exit(NULL); //退出线程
}

void Checking_log()
{
	if (access("/var/log/shencloud", F_OK) != 0)
	{
		system("sudo mkdir /var/log/shencloud");
		system("sudo chmod 777 /var/log/shencloud");
	}
    if (access("/var/log/shencloud/spicy.log", F_OK) != 0)
		system("sudo echo null > /var/log/shencloud/spicy.log");
	if ( pthread_create(&tid, NULL, thrd_func, NULL) !=0 ) {
        printf("Create checking_log thread error!\n");
    }
}

void del_cookies()
{
	if (access(PATH_COOKIES_FILE, F_OK) == 0)
	{
		remove(PATH_COOKIES_FILE);
	}
}
