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
#define  FILE_CONFIG_SOUND      "sound.xml"
#define  FILE_MANUFACTURE_TYPE   "manufact.xml"
#define  FILE_CONFIG_TEASERVER   "teaserver.xml"
#define  FILE_CONFIG_PASSWORD    "/usr/bin/sypassword.xml"
#define  FILE_CONFIG_DLNET      "dlnet.xml"

extern int g_interfacetype;
extern int g_thinviewlog;
int g_workflag;  //0: office 1: teacher


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
	else if (ntype == 7)
		 strcpy(szFile, FILE_CONFIG_DLNET);
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
			mxmlDelete(g_tree);
      	}
      	fclose(fp);
    }
}

void Parsexml2(char * file, char *element, char * value)
{
  	FILE *fp = NULL;
    char szFile[MAX_BUFF_SIZE] = {0};
  	fp = fopen(file, "r");
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
               {
                  strcpy(value, node->child->value.text.string);
        			 LogInfo("login xml get element :%s  value: %s.\n", element, node->child->value.text.string);
               }
        		}//if node != NULL
        	    mxmlDelete(g_tree);
      	}
      	fclose(fp);
    }
}

void GetDlnet(char * sznet)
{
	if (NULL == sznet)
		return;
	Parsexml("curnet", sznet, 7);
    LogInfo("GetDlnet   curnet: %s", sznet);
}

void SaveDlnet(char* sznet)
{
	mxml_node_t *xml;    /* <?xml ... ?> */
    mxml_node_t *data;   /* <data> */
    mxml_node_t *node;   /* <node> */

    xml = mxmlNewXML("1.0");
    data = mxmlNewElement(xml, "dlnet");
    node = mxmlNewElement(data, "curnet");
    LogInfo("configxml SaveDlnet curnet: %s.\n", sznet);
    mxmlNewText(node, 0, sznet);
	FILE *fp;
    fp = fopen(FILE_CONFIG_DLNET, "w");
    if (fp)
    {
        mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
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
	//autologin
    node = mxmlNewElement(data, "autologin");
	memset(szTmp, 0, MAX_BUFF_SIZE);
	sprintf(szTmp, "%d", info.autologin);
	LogInfo("configxml savelogin autologin: %d.\n", info.autologin);
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
    memset(szTmp, 0, MAX_BUFF_SIZE);
    sprintf(szTmp, "%d", info.resol);
    mxmlNewText(node, 0, szTmp);
    node = mxmlNewElement(node_server, "resolution_manual");
    memset(szTmp, 0, MAX_BUFF_SIZE);
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
	//Update_xmlNode(FILE_CONFIG_LOGIN, "user", info.szUser);
	//Update_xmlNode(FILE_CONFIG_LOGIN, "password", info.szPass);
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
         mxmlDelete(g_tree);
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
         mxmlDelete(g_tree);
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
	//add kevin 2016/11/4
	memset(szTmp, 0, MAX_BUFF_SIZE);
	Parsexml("autologin",  szTmp, 0);
	LogInfo("GetLoginInfo   autologin: %s", szTmp);
	pInfo->autologin = atoi(szTmp);
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

void Save_Sound(struct SoundInfo info)
{
	mxml_node_t *xml;
	mxml_node_t *data;
	mxml_node_t *node; 
	mxml_node_t *node_server; 

    char szTmp[MAX_BUFF_SIZE] = {0};
	xml = mxmlNewXML("1.0");
	data = mxmlNewElement(xml, "setting");
	node_server = mxmlNewElement(data, "server");
	node = mxmlNewElement(node_server, "sound_value");
	memset(szTmp, 0, MAX_BUFF_SIZE);
	sprintf(szTmp, "%d", info.volume);
	mxmlNewText(node, 0, szTmp);
	FILE *fp;
	fp = fopen(FILE_CONFIG_SOUND, "w");
	if (fp)
	{
		mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
		fclose(fp);
	}
}

void Get_Sound(struct SoundInfo *info)
{
	 FILE *fp;
     fp = fopen(FILE_CONFIG_SOUND, "r");
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
                      mxml_node_t *tmp_node = mxmlFindElement(heading, node, "sound_value",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          info->volume = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, sound_value : %d.\n", info->volume);
                    }
              }//for
         }//if
         mxmlDelete(g_tree);
     }
     fclose(fp);
     return 0;
}

//add by
void GetManufactureType()
{
	char szTmp[MAX_BUFF_SIZE] = {0};
	Parsexml2(FILE_MANUFACTURE_TYPE, "manufacture", szTmp);
	LogInfo("GetManufactureType manufacture type: %s", szTmp);
	g_interfacetype = atoi(szTmp);
	memset(szTmp, 0, MAX_BUFF_SIZE);
	Parsexml2(FILE_MANUFACTURE_TYPE, "log", szTmp);
	LogInfo("GetManufactureType log type: %s", szTmp);
	g_thinviewlog = atoi(szTmp);
	memset(szTmp, 0, MAX_BUFF_SIZE);
	Parsexml2(FILE_MANUFACTURE_TYPE, "workflag", szTmp);  
	LogInfo("GetManufactureType workflag type: %s", szTmp);
	g_workflag = atoi(szTmp);
	memset(szTmp, 0, MAX_BUFF_SIZE);
	Parsexml2(FILE_MANUFACTURE_TYPE, "modifyuserpas", szTmp);  
	LogInfo("GetManufactureType modifyuserpas type: %s", szTmp);
	g_openModifyUserPas = atoi(szTmp);
}
//add end

void Save_StuServerInfo(struct StuServerInfo info)
{
	mxml_node_t *xml;
	mxml_node_t *data;
	mxml_node_t *node; 
	mxml_node_t *node_server; 

	char szTmp[MAX_BUFF_SIZE] = {0};
	xml = mxmlNewXML("1.0");
	data = mxmlNewElement(xml, "setting");
	node_server = mxmlNewElement(data, "server");
	//stu_addr
	node = mxmlNewElement(node_server, "stu_addr");
	mxmlNewText(node, 0, info.stu_addr);
	//upgrade_addr
	node = mxmlNewElement(node_server, "upgrade_addr");
	mxmlNewText(node, 0, info.upgrade_addr);
	//stu_port
	node = mxmlNewElement(node_server, "stu_port");
	memset(szTmp, 0, MAX_BUFF_SIZE);
	sprintf(szTmp, "%d", info.stu_port);
	mxmlNewText(node, 0, szTmp);
	//upgrade_port
	node = mxmlNewElement(node_server, "upgrade_port");
	memset(szTmp, 0, MAX_BUFF_SIZE);
	sprintf(szTmp, "%d", info.upgrade_port);
	mxmlNewText(node, 0, szTmp);
	FILE *fp;
	fp = fopen(FILE_CONFIG_TEASERVER, "w");
	if (fp)
	{
		mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
		fclose(fp);
	}
}

void Get_StuServerInfo(struct StuServerInfo *info)
{
	 FILE *fp;
     fp = fopen(FILE_CONFIG_TEASERVER, "r");
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
             for (heading = mxmlGetFirstChild(node);
                  heading;
                  heading = mxmlGetNextSibling(heading))
              {
              	  //stu_port
                    mxml_node_t *tmp_node = mxmlFindElement(heading, node, "stu_port",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          info->stu_port = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get stu_server info, stu_port : %d.\n", info->stu_port);
                    }
				  //upgrade_port
				  tmp_node = mxmlFindElement(heading, node, "upgrade_port",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                          info->upgrade_port = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get stu_server info, stu_port : %d.\n", info->upgrade_port);
                    }
				  //stu_addr
				  tmp_node = mxmlFindElement(heading, node, "stu_addr",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                           strcpy(info->stu_addr, tmp_node->child->value.text.string);
                        LogInfo("configxml Get stu_server info, stu_addr : %s.\n", info->stu_addr);
                    }
				  //upgrade_addr
				  tmp_node = mxmlFindElement(heading, node, "upgrade_addr",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        if (tmp_node->child != NULL)
                           strcpy(info->upgrade_addr, tmp_node->child->value.text.string);
                        LogInfo("configxml Get stu_server info, upgrade_addr : %s.\n", info->upgrade_addr);
                    }
              }//for
         }//if
         mxmlDelete(g_tree);
     }
     fclose(fp);
     return 0;
}

void Save_ConfigPass(char *szPass)
{
	mxml_node_t *xml;
	mxml_node_t *data;
	mxml_node_t *node; 
	mxml_node_t *node_server; 
	//if (access(FILE_CONFIG_PASSWORD, F_OK) != 0)
	{
		//no exist
		if (szPass != NULL)
		{
			xml = mxmlNewXML("1.0");
			data = mxmlNewElement(xml, "setting");
			node_server = mxmlNewElement(data, "server");
			node = mxmlNewElement(node_server, "configpass");
			mxmlNewText(node, 0, szPass);
			FILE *fp;
			fp = fopen(FILE_CONFIG_PASSWORD, "w");
			if (fp)
			{
				mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
				fclose(fp);
			}
		}	
	}
}

void Get_ConfigPass(char *szPass)
{
	if (szPass != NULL)
	{
		Parsexml2(FILE_CONFIG_PASSWORD, "configpass", szPass);
		LogInfo("Get_ConfigPass config md5 content: %s", szPass);
	}
}

void Update_xmlNode(char *path, char *node_name, char *value)
{
	FILE *fp;
	fp = fopen(path, "r+");
	if (fp == NULL)
	{
		return -1;
	}
	//
	mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
  	if (g_tree != NULL)
  	{
    		mxml_node_t *node;
    		node = mxmlFindElement(g_tree, g_tree, node_name,
    												 NULL, NULL,
    												 MXML_DESCEND);
    		if (node != NULL)
    		{
			mxmlSetText(node, 0, value);
    		}
			/*
		char szTmp[100] = {0};
		sprintf(szTmp, "sudo cat /dev/null > %s", path);
	    system(szTmp);*/
		fseek ( fp , 0 , SEEK_SET );
		mxmlSaveFile(g_tree, fp, MXML_NO_CALLBACK);
		mxmlDelete(g_tree);
  	}
  	fclose(fp);
}



