/***********
//filename: resol.c
//author: zhaosenhua
//version: 1.0
//funciton list: ...
//description: 屏幕分辩率的设置
//Date: 2016/8/8
**************/
#include "global.h"
#include <string.h>
#include <stdio.h>

#define SY_RESOLUTION_FILE    "resol.txt"
char szResol[MAX_RESOL_SIZE][100] = {"","",""};

//字符串去空格
extern char *rtspace(char *str)
{
    char *it = NULL;   //指针最好初始化下
    while( *str == ' ' && *str) ++str;
    it = str;
    while(*str) str++; //指针定到末尾
    while(*(--str) ==' '); //去除最后的空格
    *(++str) = '\0';
    return it;
}

int iSNumber(char c)
{
    if (c - '0' >=0 && c - '0'<=9)
       return 1;
    else
       return 0;
}

//获取系统支持的分辨率
int GetResol()
{
    char szTmp[MAX_BUFF_SIZE] = {0};
    sprintf(szTmp, "xrandr > %s", SY_RESOLUTION_FILE);
    system(szTmp);
    FILE* fp = fopen(SY_RESOLUTION_FILE, "r");
    if (fp == NULL)
       return -1;
    char buf[MAX_BUFF_SIZE] = {0};
    int nline = 0;
    memset(szTmp, 0, MAX_BUFF_SIZE);
    char szvalue[MAX_BUFF_SIZE] = {0};
    int nCount = 0;
    while(fgets(buf, MAX_BUFF_SIZE, fp) != NULL)
    {
        if (strstr(buf, "#") != NULL || strstr(buf, "Screen") != NULL ||
          strstr(buf, "DP1") != NULL )
        {
            continue;
        }
        //if (nline >= 2)
        {
            char * p = strstr(buf, "x");//p为dhcp的出现位置,NULL则为没找到
            if (p && (iSNumber(*(p-1)) == 1)
                && (iSNumber(*(p+1)) == 1))
            {
                //LogInfo("Debug: GetResol findstr find.\n");
                memcpy(szTmp, p-5, 11);
                strcpy(szvalue, rtspace(szTmp));
                //LogInfo("Debug: resol ######## 000 :%s .\n", szvalue);
                sprintf(g_resol[nCount], "%s", szvalue);
                LogInfo("Debug: resol ######## 111 :%s .\n", g_resol[nCount]);
                nCount++;
            }
        }
        //nline++;
    }
    //得到所有支持的分辨率
    //去除横向<1000 和 纵向 <800 的分辨率
    int i=0;
	g_resolCount = 0;
    for (; i<nCount; i++)
    {
        char *delim = "x";
        char *p = NULL;
        char szw[20] = {0};
        char szh[20] = {0};
        char szTmp[MAX_BUFF_SIZE] = {0};
        sprintf(szTmp, "%s", g_resol[i]);
        LogInfo("Debug : resol szTmp 33333 :%s.\n", szTmp);
        p = strtok(szTmp, delim);
        strcpy(szw, p);
        p = strtok(NULL, delim);
        strcpy(szh, p);
        LogInfo("Debug : resol del lower resolution, szw :%s, szh :%s.\n", szw, szh);
        //if (strcmp(szw, "1000") > 0 &&
        //    strcmp(szh, "800") >= 0)
        if ( (strcmp(szw, "1920") == 0 && strcmp(szh, "1080") == 0) || 
			(strcmp(szw, "1920") == 0 && strcmp(szh, "1200") == 0) ||
			 (strcmp(szw, "1440") == 0 && strcmp(szh, "900") == 0) ||
			 (strcmp(szw, "1024") == 0 && strcmp(szh, "768") == 0) ||
			(strcmp(szw, "1280") == 0 && strcmp(szh, "720") == 0) || 
			(strcmp(szw, "1280") == 0 && strcmp(szh, "768") == 0) || 
			(strcmp(szw, "1280") == 0 && strcmp(szh, "1024") == 0) ||
			(strcmp(szw, "1360") == 0 && strcmp(szh, "768") == 0) ||
			(strcmp(szw, "1366") == 0 && strcmp(szh, "768") == 0) ||
			(strcmp(szw, "1600") == 0 && strcmp(szh, "900") == 0) ||
			(strcmp(szw, "1600") == 0 && strcmp(szh, "1080") == 0) ||
			(strcmp(szw, "1368") == 0 && strcmp(szh, "768") == 0))
        {
           char sztmp[100] = {0};
           sprintf(sztmp, "%s", g_resol[i]);
		   LogInfo("Debug: resol ######## sztmp :%s .\n", sztmp);
           strcpy(szResol[g_resolCount], sztmp);
		   LogInfo("Debug: resol ######## g_resolCount :%d .\n", g_resolCount);
           g_resolCount++;
        }
    }
    //g_resolCount = nCount;
    LogInfo("Debug: resol ######## 222 g_resolCount :%d .\n", g_resolCount);
    fclose(fp);
    remove(SY_RESOLUTION_FILE);
}
