/***********
//filename: configxml.c
//author: zhaosenhua
//version: 1.0
//funciton list: ...etc.
//description: 保存，获取配置信息库，配置信息存储在XML文件中
//使用minixml 库进行处理，这里采用简短的XML结构
//如果XML结构复杂，将分开采用多个XML文件。
//Date: 2016/6/21
**************/
#include <mxml.h>  //minixml 解析库
#include <string.h>
#include <stdlib.h>
#include "global.h"
//login.xml
/*<?xml version="1.0"?>
<login>
    <user>val1</user>
    <password>val2</password>
    <ip>val3</ip>
    <port>val4</port>
</login>*/
#define  FILE_CONFIG_LOGIN      "login.xml"
#define  FILE_CONFIG_SETTING    "setting.xml"
#define  FILE_CONFIG_MIRLOGIN   "login_mir.xml"
#define  FILE_CONFIG_VMARECONF  "login_vm.xml"

void Parsexml(char * element,  char * value,  int ntype)
{
  	FILE *fp = NULL;
    char szFile[MAX_BUFF_SIZE] = {0};
    if (ntype == 0)
         strcpy(szFile, FILE_CONFIG_LOGIN);
    else if (ntype == 1)
         strcpy(szFile, FILE_CONFIG_MIRLOGIN);
    else if (ntype == 3)
         strcpy(szFile, FILE_CONFIG_VMARECONF);
  	fp = fopen(szFile, "r");
    if (fp)
    {
      	mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
      	if (g_tree != NULL)
      	{
        		mxml_node_t *node;
        		node = mxmlFindElement(g_tree, g_tree, element,
        												 NULL, NULL,
        												 MXML_DESCEND);
        		if (node != NULL)
        		{
               if (node->child != NULL)
                  strcpy(value, node->child->value.text.string);
        			 LogInfo("login xml get element :%s  value: %s.\n", element, node->child->value.text.string);
               //memcpy(value, node->child->value.text.string, strlen(node->child->value.text.string));
        		}
      	}
      	fclose(fp);
    }
}

void SaveLogin(struct LoginInfo info)
{
    mxml_node_t *xml;    /* <?xml ... ?> */
    mxml_node_t *data;   /* <data> */
    mxml_node_t *node;   /* <node> */

    xml = mxmlNewXML("1.0");
    data = mxmlNewElement(xml, "login");
    node = mxmlNewElement(data, "user");
    LogInfo("configxml savelogin user: %s.\n", info.user);
    mxmlNewText(node, 0, info.user);
    node = mxmlNewElement(data, "password");
    LogInfo("configxml savelogin password: %s.\n", info.pass);
    mxmlNewText(node, 0, info.pass);
    node = mxmlNewElement(data, "repass");
    char szTmp[MAX_BUFF_SIZE] = {0};
    sprintf(szTmp, "%d", info.repass);
    LogInfo("configxml savelogin repass: %d.\n", info.repass);
    LogInfo("configxml savelogin repass sztmp: %s.\n", szTmp);
    mxmlNewText(node, 0, szTmp);
    FILE *fp;
    fp = fopen(FILE_CONFIG_LOGIN, "w");
    if (fp)
    {
        mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
        fclose(fp);
    }
}

void SaveServerInfo(struct ServerInfo info)
{
    mxml_node_t *xml;    /* <?xml ... ?> */
    mxml_node_t *data;   /* <data> */
    mxml_node_t *node;   /* <node> */
    mxml_node_t *node_server;

    xml = mxmlNewXML("1.0");
    data = mxmlNewElement(xml, "setting");
    node_server = mxmlNewElement(data, "server");
    node = mxmlNewElement(node_server, "address");
    mxmlNewText(node, 0, info.szIP);
    node = mxmlNewElement(node_server, "port");
    char szTmp[MAX_BUFF_SIZE] = {0};
    sprintf(szTmp, "%d", info.nport);
    mxmlNewText(node, 0, szTmp);
    node = mxmlNewElement(node_server, "user");
    mxmlNewText(node, 0, info.szUser);
    node = mxmlNewElement(node_server, "password");
    mxmlNewText(node, 0, info.szPass);
    node = mxmlNewElement(node_server, "resolution_client");
    memset(szTmp, 0, sizeof(MAX_BUFF_SIZE));
    sprintf(szTmp, "%d", info.resol);
    mxmlNewText(node, 0, szTmp);
    node = mxmlNewElement(node_server, "resolution_manual");
    memset(szTmp, 0, sizeof(MAX_BUFF_SIZE));
    sprintf(szTmp, "%d", info.manresol);
    mxmlNewText(node, 0, szTmp);
    node = mxmlNewElement(node_server, "resolution_value");
    mxmlNewText(node, 0, info.szResol);
    FILE *fp;
    fp = fopen(FILE_CONFIG_SETTING, "w");
    if (fp)
    {
        mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
        fclose(fp);
    }
}

int GetServerInfo(struct ServerInfo info)
{
     FILE *fp;
     fp = fopen(FILE_CONFIG_SETTING, "r");
     if (fp == NULL)
        return -1;
     mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
     if (g_tree != NULL)
     {
         mxml_node_t *node = NULL;
         mxml_node_t *heading = NULL;
         node = mxmlFindElement(g_tree, g_tree, "setting",
                              NULL, NULL,
                              MXML_DESCEND);
         if (node != NULL)
         {
             //printf("login xml get element :%s  value: %s.\n", element, node->child->value.text.string);
             //memcpy(value, node->child->value.text.string, strlen(node->child->value.text.string));
             for (heading = mxmlGetFirstChild(node);
                  heading;
                  heading = mxmlGetNextSibling(heading))
              {
                    mxml_node_t * tmp_node = mxmlFindElement(heading, node, "address",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          strcpy(info.szIP, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, address : %s\n", info.szIP);
                    }

                    tmp_node = mxmlFindElement(heading, node, "port",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          info.nport = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, port : %d.\n", info.nport);
                    }

                    tmp_node = mxmlFindElement(heading, node, "user",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          strcpy(info.szUser, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, user : %s.\n", info.szUser);
                    }

                    tmp_node = mxmlFindElement(heading, node, "password",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          strcpy(info.szPass, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, password : %s.\n", info.szPass);
                    }

                    tmp_node = mxmlFindElement(heading, node, "resolution_client",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          info.resol = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, resolution_client : %d.\n", info.resol);
                    }
                    tmp_node = mxmlFindElement(heading, node, "resolution_manual",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          info.manresol = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, resolution_manual : %d.\n", info.manresol);
                    }
                    tmp_node = mxmlFindElement(heading, node, "resolution_value",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          strcpy(info.szResol, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, resolution_value : %s.\n", info.szResol);
                    }
              }//for
         }//if
     }
     fclose(fp);
     return 0;
}

int GetServerInfo2(struct ServerInfo *pInfo)
{
     FILE *fp;
     fp = fopen(FILE_CONFIG_SETTING, "r");
     if (fp == NULL)
        return -1;
     mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
     if (g_tree != NULL)
     {
         mxml_node_t *node = NULL;
         mxml_node_t *heading = NULL;
         node = mxmlFindElement(g_tree, g_tree, "setting",
                              NULL, NULL,
                              MXML_DESCEND);
         if (node != NULL)
         {
             //printf("login xml get element :%s  value: %s.\n", element, node->child->value.text.string);
             //memcpy(value, node->child->value.text.string, strlen(node->child->value.text.string));
             for (heading = mxmlGetFirstChild(node);
                  heading;
                  heading = mxmlGetNextSibling(heading))
              {
                    mxml_node_t * tmp_node = mxmlFindElement(heading, node, "address",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                       if (tmp_node->child != NULL)
                         strcpy(pInfo->szIP, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, address : %s\n", pInfo->szIP);
                    }

                    tmp_node = mxmlFindElement(heading, node, "port",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                         pInfo->nport = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, port : %d.\n", pInfo->nport);
                    }

                    tmp_node = mxmlFindElement(heading, node, "user",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                            strcpy(pInfo->szUser, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, user : %s.\n", pInfo->szUser);
                    }

                    tmp_node = mxmlFindElement(heading, node, "password",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          strcpy(pInfo->szPass, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, password : %s.\n", pInfo->szPass);
                    }

                    tmp_node = mxmlFindElement(heading, node, "resolution_client",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          pInfo->resol = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, resolution_client : %d.\n", pInfo->resol);
                    }
                    tmp_node = mxmlFindElement(heading, node, "resolution_manual",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          pInfo->manresol = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, resolution_manual : %d.\n", pInfo->manresol);
                    }
                    tmp_node = mxmlFindElement(heading, node, "resolution_value",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                           strcpy(pInfo->szResol, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, resolution_value : %s.\n", pInfo->szResol);
                    }
              }//for
         }//if
     }
     fclose(fp);
     return 0;
}

int GetLoginInfo(struct LoginInfo *pInfo)
{
    Parsexml("user",  pInfo->user, 0);
    LogInfo("GetLoginInfo   user: %s", pInfo->user);
    Parsexml("password",  pInfo->pass, 0);
    LogInfo("GetLoginInfo   password: %s", pInfo->pass);
    char szTmp[MAX_BUFF_SIZE] = {0};
    Parsexml("repass",  szTmp, 0);
    LogInfo("GetLoginInfo   repass: %s", szTmp);
    pInfo->repass = atoi(szTmp);
}

void SaveMirLogin(struct LoginInfo info)
{
    mxml_node_t *xml;    /* <?xml ... ?> */
    mxml_node_t *data;   /* <data> */
    mxml_node_t *node;   /* <node> */

   char szTmp[MAX_BUFF_SIZE] = {0};
    xml = mxmlNewXML("1.0");
    data = mxmlNewElement(xml, "login");
    node = mxmlNewElement(data, "address");
    LogInfo("configxml SaveMirLogin password: %s.\n", info.ip);
    mxmlNewText(node, 0, info.ip);
    node = mxmlNewElement(data, "port");
    memset(szTmp, 0, MAX_BUFF_SIZE);
    sprintf(szTmp, "%d", info.port);
    LogInfo("configxml SaveMirLogin port: %d.\n", info.port);
    mxmlNewText(node, 0, szTmp);
    node = mxmlNewElement(data, "user");
    LogInfo("configxml SaveMirLogin user: %s.\n", info.user);
    mxmlNewText(node, 0, info.user);
    node = mxmlNewElement(data, "password");
    LogInfo("configxml SaveMirLogin password: %s.\n", info.pass);
    mxmlNewText(node, 0, info.pass);
    node = mxmlNewElement(data, "repass");
    memset(szTmp, 0, MAX_BUFF_SIZE);
    sprintf(szTmp, "%d", info.repass);
    LogInfo("configxml SaveMirLogin repass: %d.\n", info.repass);
    LogInfo("configxml SaveMirLogin repass sztmp: %s.\n", szTmp);
    mxmlNewText(node, 0, szTmp);
    FILE *fp;
    fp = fopen(FILE_CONFIG_MIRLOGIN, "w");
    if (fp)
    {
        mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
        fclose(fp);
    }
}

void GetMirLoginInfo(struct LoginInfo *pInfo)
{
    char szTmp[MAX_BUFF_SIZE] = {0};
    Parsexml("address",  pInfo->ip,  1);
    LogInfo("GetMirLoginInfo   address: %s", pInfo->ip);
    memset(szTmp, 0, MAX_BUFF_SIZE);
    Parsexml("port",  szTmp, 1);
    LogInfo("GetMirLoginInfo   port: %s", szTmp);
    pInfo->port = atoi(szTmp);
    Parsexml("user",  pInfo->user, 1);
    LogInfo("GetMirLoginInfo   user: %s", pInfo->user);
    Parsexml("password",  pInfo->pass, 1);
    LogInfo("GetMirLoginInfo   password: %s", pInfo->pass);
    memset(szTmp, 0, MAX_BUFF_SIZE);
    Parsexml("repass",  szTmp, 1);
    LogInfo("GetMirLoginInfo   repass: %s", szTmp);
    pInfo->repass = atoi(szTmp);
}

void SaveVMareLogin(struct LoginInfo info)
{
    mxml_node_t *xml;    /* <?xml ... ?> */
    mxml_node_t *data;   /* <data> */
    mxml_node_t *node;   /* <node> */

    char szTmp[MAX_BUFF_SIZE] = {0};
    xml = mxmlNewXML("1.0");
    data = mxmlNewElement(xml, "login");
    node = mxmlNewElement(data, "address");
    LogInfo("configxml SaveVmLogin password: %s.\n", info.ip);
    mxmlNewText(node, 0, info.ip);
    node = mxmlNewElement(data, "port");
    memset(szTmp, 0, MAX_BUFF_SIZE);
    sprintf(szTmp, "%d", info.port);
    LogInfo("configxml SaveVmLogin port: %d.\n", info.port);
    mxmlNewText(node, 0, szTmp);
    node = mxmlNewElement(data, "user");
    LogInfo("configxml SaveVmLogin user: %s.\n", info.user);
    mxmlNewText(node, 0, info.user);
    node = mxmlNewElement(data, "password");
    LogInfo("configxml SaveVmLogin password: %s.\n", info.pass);
    mxmlNewText(node, 0, info.pass);
    node = mxmlNewElement(data, "repass");
    memset(szTmp, 0, MAX_BUFF_SIZE);
    sprintf(szTmp, "%d", info.repass);
    LogInfo("configxml SaveVmLogin repass: %d.\n", info.repass);
    LogInfo("configxml SaveVmLogin repass sztmp: %s.\n", szTmp);
    mxmlNewText(node, 0, szTmp);
    FILE *fp;
    fp = fopen(FILE_CONFIG_VMARECONF, "w");
    if (fp)
    {
        mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
        fclose(fp);
    }
}

void GetVmareLoginInfo(struct LoginInfo *pInfo)
{
    char szTmp[MAX_BUFF_SIZE] = {0};
    Parsexml("address",  pInfo->ip,  3);
    LogInfo("GetVmLoginInfo   address: %s", pInfo->ip);
    memset(szTmp, 0, MAX_BUFF_SIZE);
    Parsexml("port",  szTmp, 3);
    LogInfo("GetVmLoginInfo   port: %s", szTmp);
    pInfo->port = atoi(szTmp);
    Parsexml("user",  pInfo->user, 3);
    LogInfo("GetVmLoginInfo   user: %s", pInfo->user);
    Parsexml("password",  pInfo->pass, 3);
    LogInfo("GetVmLoginInfo   password: %s", pInfo->pass);
    memset(szTmp, 0, MAX_BUFF_SIZE);
    Parsexml("repass",  szTmp, 3);
    LogInfo("GetVmLoginInfo   repass: %s", szTmp);
    pInfo->repass = atoi(szTmp);
}
