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

int Ovirt_Login(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_LOGIN);
    //printf("Ovirt_Login url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_Login url : %s.\n", g_szUrl);

    if (Http_Request(g_szUrl, user, password) < 0)
    {
       printf("Ovirt login failed.\n");
       LogInfo("Debug: Ovirt login failed.\n");
       return SY_OVIRT_LOGIN_FAILED;
    }
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
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_VMS);
  //  printf("Ovirt_getvms url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_getvms url : %s.\n", g_szUrl);

    if (Http_Request(g_szUrl, user, password) < 0)
    {
       printf("Ovirt get vms failed.\n");
       LogInfo("Debug: Ovirt get vms failed.\n");
       return SY_OVIRT_GETVMS_FAILED;
    }
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
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_VMS);
  //  printf("Ovirt_getvms url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_getvmsTmp url : %s.\n", g_szUrl);

    if (Http_Request2(g_szUrl, user, password, FILE_OVIRT_INFOTMP_PATH) < 0)
    {
       printf("Ovirt get vms tmp failed.\n");
       LogInfo("Debug: Ovirt get vms tmp failed.\n");
       return SY_OVIRT_GETVMS_FAILED;
    }
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
       return SY_OVIRT_GETVMS_FAILED;
    }
    return 0;
}

int Ovirt_GetVm3(char *url, char *vm)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
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
       return SY_OVIRT_GETVMS_FAILED;
    }
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
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_CLUSTER);
    //printf("Ovirt_getclusters url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_getclusters url : %s.\n", g_szUrl);

    if (Http_Request(g_szUrl, user, password) < 0)
    {
       printf("Ovirt get clusters failed.\n");
       LogInfo("Debug: Ovirt get clusters failed.\n");
       return SY_OVIRT_GETCLUSTERS_FAILED;
    }
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
     return SY_OVIRT_STARTVMS_FAILED;
  }
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
     return SY_OVIRT_SHUTDOWNVMS_FAILED;
  }
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
     return SY_OVIRT_SUSPENDVMS_FAILED;
  }
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
        return SY_OVIRT_GETVMSTICKET_FAILED;
    }
    printf("ovirt get vms ticker success.\n");
    return 0;
}
