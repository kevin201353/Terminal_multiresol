/***********
//filename: curlc
//author: zhaosenhua
//version: 1.0
//funciton list: ...
//description: 自定义函数，负责与ovirt-engine ovirt 服务端通信，供其它函数调用
//如：登录 ovirt 服务端，获取 ovirt 参数信息
//Date: 2016/6/24
**************/
#include <stdio.h>
#include <string.h>
#include "global.h"
#include <stdlib.h>

char g_szUrl[MAX_DATA_SIZE] = {0};

volatile int  g_working = 0;
extern int Http_Post2(char *url, char *user, char* password, char* data);

int Ovirt_Login(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
	if (g_working == 1)
		return 0;
	g_working = 1;
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_LOGIN);
    LogInfo("Debug: Ovirt_Login url : %s.\n", g_szUrl);
#if 0
	char * p = strstr(user, "admin@internal");
	if (NULL == p)
	{
		//普通用户，登录获取不到虚拟机列表
		//用admin 用户登录一遍
		Http_Request(g_szUrl, "admin@internal", password);
	}
#endif

    if (Http_Request(g_szUrl, user, password) < 0)
    {
       LogInfo("Debug: Ovirt login failed.\n");
	   g_working = 0;
       return SY_OVIRT_LOGIN_FAILED;
    }
	 g_working = 0;
    return 0;
}

int Ovirt_GetVms(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
	if (g_working == 1)
		return 0;
	g_working = 1;
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_VMS);
  //  printf("Ovirt_getvms url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_getvms url : %s.\n", g_szUrl);
    
    if (Http_Request(g_szUrl, user, password) < 0)
    {
    	//这里增加获取两次虚拟机列表
       LogInfo("Debug: Ovirt_getvms url, http request first failed, start second request.\n");
       if (Http_Request(g_szUrl, user, password) < 0)
	   {
	       printf("Ovirt get vms failed.\n");
	       LogInfo("Debug: Ovirt get vms failed.\n");
		   //g_working = 0;
	       return SY_OVIRT_GETVMS_FAILED;
       	}
    }
	LogInfo("Debug: Ovirt_getvms url, http request success.\n");
	g_working = 0;
    return 0;
}


int Ovirt_GetLongVms(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_VMS);
    LogInfo("Debug: Ovirt_GetLongVms url : %s.\n", g_szUrl);
    if (http_login_request(g_szUrl, user, password) < 0)
    {
       LogInfo("Debug: Ovirt_GetLongVms url, http request first failed, start second request.\n");
       if (http_login_request(g_szUrl, user, password) < 0)
	   {
	       LogInfo("Debug: Ovirt_GetLongVms failed.\n");
	       return SY_OVIRT_GETVMS_FAILED;
       	}
    }
	LogInfo("Debug: Ovirt_GetLongVms url, http request success.\n");
    return 0;
}

int Ovirt_GetVmsTmp(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
	if (g_working == 1)
		return 0;
	g_working = 1;
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_VMS);
  //  printf("Ovirt_getvms url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_getvmsTmp url : %s.\n", g_szUrl);

    if (Http_Request2(g_szUrl, user, password, FILE_OVIRT_INFOTMP_PATH) < 0)
    {
       printf("Ovirt get vms tmp failed.\n");
       LogInfo("Debug: Ovirt get vms tmp failed.\n");
	   g_working = 0;
       return SY_OVIRT_GETVMS_FAILED;
    }
	g_working = 0;
    return 0;
}

int Ovirt_GetVm2(char *url, char *user, char* password, char *vm)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
	if (g_working == 1)
		return 0;
	g_working = 1;
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_VMS);
    strcat(g_szUrl, "/");
    strcat(g_szUrl, vm);
    //printf("Ovirt_GetVm2 url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_GetVm2 url : %s.\n", g_szUrl);
    char szPath[MAX_BUFF_SIZE] = {0};
    strcpy(szPath, "tmp/");
    strcat(szPath, vm);
    strcat(szPath,  ".xml");
    if (Http_Request2(g_szUrl, user, password, szPath) < 0)
    {
       printf("Ovirt get vm2 failed.\n");
       LogInfo("Debug: Ovirt get vm2 failed.\n");
	   g_working = 0;
       return SY_OVIRT_GETVMS_FAILED;
    }
	 g_working = 0;
    return 0;
}

int Ovirt_GetVm3(char *url, char *vm)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
	if (g_working == 1)
		return 0;
	g_working = 1;
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_VMS);
    strcat(g_szUrl, "/");
    strcat(g_szUrl, vm);
    LogInfo("Debug: Ovirt_GetVm3 url : %s.\n", g_szUrl);
    char szPath[MAX_BUFF_SIZE] = {0};
    strcpy(szPath, "tmp/");
    strcat(szPath, vm);
    strcat(szPath,  ".xml");
    if (Http_Request3(g_szUrl, szPath) < 0)
    {
       printf("Ovirt get vm3 failed.\n");
       LogInfo("Debug: Ovirt get vm3 failed.\n");
	   g_working = 0;
       return SY_OVIRT_GETVMS_FAILED;
    }
	g_working = 0;
    return 0;
}


int Ovirt_GetClusters(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
	if (g_working == 1)
		return 0;
	g_working = 1;
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_CLUSTER);
    //printf("Ovirt_getclusters url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_getclusters url : %s.\n", g_szUrl);

    if (Http_Request(g_szUrl, user, password) < 0)
    {
       printf("Ovirt get clusters failed.\n");
       LogInfo("Debug: Ovirt get clusters failed.\n");
	   g_working = 0;
       return SY_OVIRT_GETCLUSTERS_FAILED;
    }
	g_working = 0;
    return 0;
}


int Ovirt_StartVms(char *url, char *user, char* password, char *vm)
{
  if (url == NULL && strlen(url) <= 0)
      return -1;
  if (user == NULL && strlen(user) <= 0)
  {
      return SY_USER_ISNULL;
  }
  LogInfo("Debug: Ovirt_startVms g_working : %d.\n", g_working);
  if (g_working == 1)
		return 0;
	g_working = 1;
  memset(g_szUrl, 0, MAX_DATA_SIZE);
  strcat(g_szUrl, url);
  strcat(g_szUrl, "api/vms/");
  strcat(g_szUrl, vm);
  strcat(g_szUrl, "/");
  strcat(g_szUrl, "start");
  //printf("Ovirt_startVms url :%s.\n", g_szUrl);
  LogInfo("Debug: Ovirt_startVms url : %s.\n", g_szUrl);
  char szData[MAX_BUFF_SIZE] = {0};
  strcpy(szData, "<action></action>");
  if (Http_Post(g_szUrl, user, password, szData) < 0)
  {
     printf("Ovirt StartVms failed.\n");
     LogInfo("Debug: Ovirt StartVms failed.\n");
	 g_working = 0;
     return SY_OVIRT_STARTVMS_FAILED;
  }
  g_working = 0;  
  return 0;
}

int Ovirt_ShutdownVms(char *url, char *user, char* password, char *vm)
{
  if (url == NULL && strlen(url) <= 0)
      return -1;
  if (user == NULL && strlen(user) <= 0)
  {
      return SY_USER_ISNULL;
  }
   if (g_working == 1)
		return 0;
	g_working = 1;
  memset(g_szUrl, 0, MAX_DATA_SIZE);
  strcat(g_szUrl, url);
  strcat(g_szUrl, "api/vms/");
  strcat(g_szUrl, vm);
  strcat(g_szUrl, "/");
  strcat(g_szUrl, "shutdown");
  //printf("Ovirt_ShutdownVms url :%s.\n", g_szUrl);
  LogInfo("Debug: Ovirt_ShutdownVms url : %s.\n", g_szUrl);
  char szData[MAX_BUFF_SIZE] = {0};
  strcpy(szData, "<action></action>");
  if (Http_Post(g_szUrl, user, password, szData) < 0)
  {
     printf("Ovirt ShutdownVms failed.\n");
     LogInfo("Debug: Ovirt ShutdownVms failed.\n");
	 g_working = 0;
     return SY_OVIRT_SHUTDOWNVMS_FAILED;
  }
  g_working = 0;
  return 0;
}

//待机
int Ovirt_SuspendVms(char *url, char *user, char* password, char *vm)
{
  if (url == NULL && strlen(url) <= 0)
      return -1;
  if (user == NULL && strlen(user) <= 0)
  {
      return SY_USER_ISNULL;
  }
   if (g_working == 1)
		return 0;
	g_working = 1;
  memset(g_szUrl, 0, MAX_DATA_SIZE);
  strcat(g_szUrl, url);
  strcat(g_szUrl, "api/vms/");
  strcat(g_szUrl, vm);
  strcat(g_szUrl, "/");
  strcat(g_szUrl, "suspend");
  //printf("Ovirt_SuspendVms url :%s.\n", g_szUrl);
  LogInfo("Debug: Ovirt_SuspendVms url : %s.\n", g_szUrl);
  char szData[MAX_BUFF_SIZE] = {0};
  strcpy(szData, "<action></action>");
  if (Http_Post(g_szUrl, user, password, szData) < 0)
  {
     printf("Ovirt SuspendVms failed.\n");
     LogInfo("Debug: Ovirt SuspendVms failed.\n");
	 g_working = 0;
     return SY_OVIRT_SUSPENDVMS_FAILED;
  }
  g_working = 0;
  return 0;
}

int Ovirt_RebootVms(char *url, char *user, char* password, char *vm)
{
  if (url == NULL && strlen(url) <= 0)
      return -1;
  if (user == NULL && strlen(user) <= 0)
  {
      return SY_USER_ISNULL;
  }
   if (g_working == 1)
		return 0;
	g_working = 1;
  memset(g_szUrl, 0, MAX_DATA_SIZE);
  strcat(g_szUrl, url);
  strcat(g_szUrl, "api/vms/");
  strcat(g_szUrl, vm);
  strcat(g_szUrl, "/");
  strcat(g_szUrl, "reboot");
  //printf("Ovirt_RebootVms url :%s.\n", g_szUrl);
  LogInfo("Debug: Ovirt_RebootVms url : %s.\n", g_szUrl);
  char szData[MAX_BUFF_SIZE] = {0};
  strcpy(szData, "<action></action>");
  if (Http_Post(g_szUrl, user, password, szData) < 0)
  {
     printf("Ovirt RebootVms failed.\n");
     LogInfo("Debug: Ovirt RebootVms failed.\n");
	 g_working = 0;
     return SY_OVIRT_SUSPENDVMS_FAILED;
  }
   g_working = 0;
  return 0;
}


int Ovirt_GetVmTicket(char *url, char *user, char* password, char *vm)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
	 if (g_working == 1)
		return 0;
	g_working = 1;
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, "api/vms/");
    strcat(g_szUrl, vm);
    strcat(g_szUrl, "/");
    strcat(g_szUrl, "ticket");
    //printf("Ovirt_GetVmTicket url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_GetVmTicket url : %s.\n", g_szUrl);
    char szData[MAX_BUFF_SIZE] = {0};
    sprintf(szData, "<action><ticket><expiry>%s</expiry></ticket></action>", "604800");
    //printf("ovirt get vms ticker post data: %s\n", szData);
    if (Http_Post(g_szUrl, user, password, szData) < 0)
    {
        printf("Ovirt GetVmTicket failed.\n");
        LogInfo("Debug: Ovirt GetVmTicket failed.\n");
		g_working = 0;
        return SY_OVIRT_GETVMSTICKET_FAILED;
    }
	g_working = 0;
    printf("ovirt send get vms ticker request success.\n");
    return 0;
}

//test
int Ovirt_allocatevm(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
	 if (g_working == 1)
		return 0;
	g_working = 1;
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    //printf("Ovirt_Login url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_allocatevm url : %s.\n", g_szUrl);

    if (Http_Request2(g_szUrl, user, password, "catevm.xml") < 0)
    {
       printf("Ovirt_allocatevm failed.\n");
       LogInfo("Debug: Ovirt_allocatevm failed.\n");
	   g_working = 0;
       return SY_OVIRT_LOGIN_FAILED;
    }
	g_working = 0;
    return 0;
}
//test end


int Ovirt_ModifyPass(char *url, char *user, char* old_password, char* new_password, char* ret, char* token)
{
	if (url == NULL && strlen(url) <= 0)
	    return -1;
	if (user == NULL && strlen(user) <= 0)
	{
	    return SY_USER_ISNULL;
	}
	if (g_working == 1)
		return 0;
	g_working = 1;
	memset(g_szUrl, 0, MAX_DATA_SIZE);
	strcat(g_szUrl, url);
	strcat(g_szUrl, "api/user/add_or_update");
	LogInfo("Debug: Ovirt_ModifyPass url : %s.\n", g_szUrl);
	char sz_data[512] = {0};
	char szTmp[20] = {0};
	char szflag[20] = {0};
	strcpy(szTmp, user);
	char *p = strtok(szTmp, "@");
	char *p1 = strtok(NULL, "@");
	if (strcmp(p, "Administrator") == 0)
	{
		strcpy(szflag, "true");
	}else
	{
		strcpy(szflag, "false");
	}
	sprintf(sz_data, "user.name=%s&user.domain=%s&user.password=%s&user.superuser=%s&user.oldPassword=%s&token=%s", p, p1, new_password, szflag, old_password, token);
	LogInfo("Debug: Ovirt_ModifyPass http_get data = %s.\n", sz_data);
	if (http_get(g_szUrl, sz_data, ret) < 0)
	{
		LogInfo("Debug: Ovirt_ModifyPass Http_Post2 failed.\n");
		g_working = 0;
		return SY_OVIRT_LOGIN_FAILED;
	}
	g_working = 0;
	return 0;
}

int Ovirt_Login_Pass(char *url, char *user, char* password, char* ret)
{
	if (url == NULL && strlen(url) <= 0)
	    return -1;
	if (user == NULL && strlen(user) <= 0)
	{
	    return SY_USER_ISNULL;
	}
	if (g_working == 1)
		return 0;
	g_working = 1;
	char sz_data[512] = {0};
	memset(g_szUrl, 0, MAX_DATA_SIZE);
	strcat(g_szUrl, url);
	strcat(g_szUrl, "api/user/login");
	sprintf(sz_data, "username=%s&userpass=%s", user, password);
	LogInfo("Debug: Ovirt_Login_Pass url : %s.\n", g_szUrl);
	if (http_get(g_szUrl, sz_data, ret) < 0)
	{
		LogInfo("Debug: Ovirt_Login_Pass failed.\n");
		g_working = 0;
		return SY_OVIRT_LOGIN_FAILED;
	}
	g_working = 0;
	return 0;
}

