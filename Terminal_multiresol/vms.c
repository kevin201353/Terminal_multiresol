/***********
//filename: vms.c
//author: zhaosenhua
//version: 1.0
//funciton list: ...
//description: 获取服务器虚拟机列表处理模块
//Date: 2016/6/23
**************/
#include "vms.h"
#include <sys/stat.h>
#include <unistd.h>

mxml_node_t *g_tree = NULL;  //用来解析xml文件
mxml_node_t *g_treeTmp = NULL;  //用来解析xml文件
static struct ServerInfo  serverInfo;

void InitVmsUpdate()
{
    for (int i=0; i<MAX_BUFF_VM; i++)
    {
         memset(g_vmsComUpdate[i].name, 0, sizeof(char) * MAX_BUFF_SIZE);
         memset(g_vmsComUpdate[i].os, 0, sizeof(char) * MAX_BUFF_SIZE);
         memset(g_vmsComUpdate[i].tab, 0, sizeof(char) * MAX_BUFF_SIZE);
         memset(g_vmsComUpdate[i].ip, 0, sizeof(char) * MAX_BUFF_SIZE);
         memset(g_vmsComUpdate[i].vmid, 0, sizeof(char) * MAX_BUFF_SIZE);
         g_vmsComUpdate[i].status = 0;
         g_vmsComUpdate[i].vcpu = 1;
         g_vmsComUpdate[i].memory = 1;
         g_vmsComUpdate[i].usb = 0;
         g_vmsComUpdate[i].port = 5900;
    }
    g_vmsComCount = 0;
}

int SY_Loadxml(char *file)
{
    FILE *fp;
    struct stat buf;
    stat (file, &buf);
    int len = (int)buf.st_size;
    if (len <= 0)
    {
        LogInfo("SY_Loadxml failed, file size <= 0.\n");
        return -1;
    }
    fp = fopen(file, "r");
    if (fp == NULL)
    {
        LogInfo("SY_Loadxml open file failed.\n");
        return -1;
    }
    g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
    if (g_tree == NULL)
    {
        printf("sy_loadxml failed.\n");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

int SY_LoadxmlTmp(char *file)
{
    FILE *fp;
    struct stat buf;
    stat (file, &buf);
    int len = (int)buf.st_size;
    if (len <= 0)
    {
        LogInfo("SY_LoadxmlTmp failed, file size <= 0.\n");
        return -1;
    }
    fp = fopen(file, "r");
    if (fp == NULL)
    {
        LogInfo("SY_LoadxmlTmp open file failed.\n");
        return -1;
    }
    g_treeTmp = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
    if (g_treeTmp == NULL)
    {
        printf("sy_loadxml failed.\n");
        fclose(fp);
        return -1;
    }
    fclose(fp);
    return 0;
}

void SY_Unloadxml(mxml_node_t * node)
{
   if (node != NULL)
      mxmlDelete(node);
}

void SY_FreeVmsList()
{
	if (!list_empty(&head))
	{
		LogInfo("list_empty ### enter.\n");
		list_for_each(plist, &head)
		{
			LogInfo("SY_FreeList start.\n");
			struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
			if (NULL != node)
			{
				LogInfo("SY_FreeList xxxxx.\n");
				free(node);
				node = NULL;
			}
			LogInfo("SY_FreeList end.\n");
		}
	}
}

void FindNode(char* value)
{
	if (g_tree == NULL)
	{
	    printf("findNode g_tree == NULL , return.\n");
	    return ;
	}
	mxml_node_t *node = NULL;
	mxml_node_t *heading = NULL;
	node = mxmlFindElement(g_tree, g_tree, value, NULL, NULL, MXML_DESCEND);
   if (node)
   {
     for (heading = mxmlGetFirstChild(node);
          heading;
          heading = mxmlGetNextSibling(heading))
          {
            //LogInfo("Debug: [xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
             if (mxmlGetElement(heading) != NULL)
             {
                 if (strcmp(mxmlGetElement(heading), "vm") == 0)
                 {
				     //LogInfo("Debug: [FindNode xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
                     struct Vms_Node *pNode = malloc(sizeof(struct Vms_Node));
                     if (pNode == NULL)
                     {
                        LogInfo("Debug: findnode malloc Vms_node failed. \n");
                        return ;
                     }
				    memset(pNode, 0, sizeof(struct Vms_Node));
                    strcpy(pNode->val.vmid, mxmlElementGetAttr(heading, "id"));
                     //get vms name
                     mxml_node_t * tmp_node = mxmlFindElement(heading, node, "name",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     //printf("222222222, get vms attribute.\n");
                     if (tmp_node)
                     {
                        strcpy(pNode->val.name, tmp_node->child->value.text.string);
                        //LogInfo("Debug: pNode-val.name : %s.\n", pNode->val.name);
                     }//if name

                     //get vms os
                     tmp_node = mxmlFindElement(heading, node, "os",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     if (tmp_node)
                     {
                        strcpy(pNode->val.os, mxmlElementGetAttr(tmp_node, "type"));
                        //LogInfo("Debug: pNode-val.os :%s.\n", pNode->val.os);
                     }//os

                     //get status
                     tmp_node = mxmlFindElement(heading, node, "state",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     if (tmp_node)
                     {
                        //LogInfo("Debug: findNode vms status : %s. \n", tmp_node->child->value.text.string);
                        if (strcmp(tmp_node->child->value.text.string, VMS_STATE_UP) == 0)
                        {
                        	pNode->val.status = 1;
                        }
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_DOWN) == 0 || 
							strcmp(tmp_node->child->value.text.string, VMS_STATE_POWEREDDOWN) == 0)
                        {
                        	pNode->val.status = 0;
						}
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_SUSPENDED) == 0 ||
							strcmp(tmp_node->child->value.text.string, VMS_STATE_PAUSED) == 0 || 
							strcmp(tmp_node->child->value.text.string, VMS_STATE_SAVINGSTATE) == 0 || 
							strcmp(tmp_node->child->value.text.string, VMS_STATE_RESTORING) == 0)
                        {
                        	pNode->val.status = 2;
						}
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_POWERINGUP) == 0 || 
							strcmp(tmp_node->child->value.text.string, VMS_STATE_WAITFORLAUNCH) == 0)
                        {
                        	pNode->val.status = 3;
						}
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_POWERINGDOWN) == 0)
                        {
                        	pNode->val.status = 4;
                        }
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_REBOOTINPROCESS) == 0)
                        {
                        	pNode->val.status = 7;
                        }
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_UNASSIGNED) == 0)
						{
							pNode->val.status = 8;
						}
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_UNKNOW) == 0 || 
							strcmp(tmp_node->child->value.text.string, VMS_STATE_NOT_RESPONDING) == 0)
						{
							pNode->val.status = 11;
						}
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_MIGRATING) == 0)
						{
							pNode->val.status = 10;
						}
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_IMAGE_LOCKED) == 0)
						{
							pNode->val.status = 12;
						}
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_IMAGE_ILLEGAL) == 0)
						{
							pNode->val.status = 13;
						}
                     }//status

                     //get cpu count
                     tmp_node = mxmlFindElement(heading, node, "cpu",
                                               NULL, NULL,
                                               MXML_DESCEND);
                    if (tmp_node)
                    {
                        mxml_node_t *cpuNode = mxmlFindElement(tmp_node, heading, "topology",
                                                NULL, NULL,
                                                MXML_DESCEND);
                        if (cpuNode)
                        {
                            const char *strcpu = mxmlElementGetAttr(cpuNode, "cores");
                            if (strcmp(strcpu, "") >= 0)
                            {
							 	//LogInfo("vms cpu count : %s. \n", strcpu);
                                pNode->val.vcpu = atoi(strcpu);
                            }
                        }
                    }//get cpu count

                    //get memory
                    tmp_node = mxmlFindElement(heading, node, "memory",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        pNode->val.memory = strtod(tmp_node->child->value.text.string, NULL)/(1024*1024*1024);
                    }//get memory

                    //get usb strategy
                    tmp_node = mxmlFindElement(heading, node, "usb",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        mxml_node_t * typeNode = mxmlFindElement(tmp_node, heading, "enabled",
                                                 NULL, NULL,
                                                 MXML_DESCEND);
                        if (typeNode)
                        {
                            if (strcmp(typeNode->child->value.text.string, "true") == 0)
                                pNode->val.usb = 1;
                            else
                                pNode->val.usb = 0;
                        }else{
                           //strcpy(pNode->val.usb, "");
                        }
                    }//usb strategy

                    //get address
					tmp_node = mxmlFindElement(heading, node, "display",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                    	mxml_node_t * typeNode = mxmlFindElement(tmp_node, heading, "address",
                                                 NULL, NULL,
                                                 MXML_DESCEND);
						if (typeNode)
						{
							strcpy(pNode->val.ip, typeNode->child->value.text.string);
						}

						typeNode = mxmlFindElement(tmp_node, heading, "port",
                                              NULL, NULL,
	                                              MXML_DESCEND);
	                    if (typeNode)
	                    {
	                       pNode->val.port = atoi(typeNode->child->value.text.string);
	                       //LogInfo("Debug: pNode-val.port :%d.\n", pNode->val.port);
	                    }
                    }
				  //sc
                    g_nVmCount++;
                    list_add(&pNode->list, &head);
                 }//if vm
             }//if null
          }
   }
    return ;
}


void FindNode2(char* value)
{
	FILE *fp = fopen(FILE_OVIRT_INFOTMP_PATH, "r");
	if (fp == NULL)
	{
		LogInfo("SY_LoadxmlTmp open file failed.\n");
		return -1;
	}
	g_treeTmp = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
	if (g_treeTmp == NULL)
	{
		printf("sy_loadxml failed.\n");
		fclose(fp);
		return -1;
	}
    mxml_node_t *node = NULL;
    mxml_node_t *heading = NULL;
    node = mxmlFindElement(g_treeTmp, g_treeTmp, value, NULL, NULL, MXML_DESCEND);
   if (node)
   {
     for (heading = mxmlGetFirstChild(node);
          heading;
          heading = mxmlGetNextSibling(heading))
          {
             if (mxmlGetElement(heading) != NULL)
             {
                 if (strcmp(mxmlGetElement(heading), "vm") == 0)
                 {
                 	if (g_vmsComCount >= 500)
                 	{
						LogInfo("Debug: FindNode2 , get server vm count >= 500, to Max value, return .\n");
						return;
					}
                     //LogInfo("Debug: [FindNode2 xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
                    strcpy(g_vmsComUpdate[g_vmsComCount].vmid, mxmlElementGetAttr(heading, "id"));
                     //get vms name
                     mxml_node_t * tmp_node = mxmlFindElement(heading, node, "name",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     if (tmp_node)
                     {
                        strcpy(g_vmsComUpdate[g_vmsComCount].name, tmp_node->child->value.text.string);
                        //LogInfo("Debug: g_vmsComUpdate.name : %s.\n", g_vmsComUpdate[g_vmsComCount].name);
                     }//if name

                     //get vms os
                     tmp_node = mxmlFindElement(heading, node, "os",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     if (tmp_node)
                     {
                        strcpy(g_vmsComUpdate[g_vmsComCount].os, mxmlElementGetAttr(tmp_node, "type"));
                        //LogInfo("Debug: g_vmsComUpdate.os :%s.\n", g_vmsComUpdate[g_vmsComCount].os);
                     }//os

                     //get status
                     tmp_node = mxmlFindElement(heading, node, "state",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     if (tmp_node)
                     {
                      //  strcpy(pNode->val.os, mxmlElementGetAttr(tmp_node, "type"));
                      //  printf("pNode-val.os : %s.\n", tmp_node->child->value.text.string);
                      //  LogInfo("Debug: pNode-val.os :%s.\n", tmp_node->child->value.text.string);
                      //  printf(" vms status : %s. \n", tmp_node->child->value.text.string);
                        if (strcmp(tmp_node->child->value.text.string, VMS_STATE_UP) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 1;
                        }
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_DOWN) == 0 ||
								strcmp(tmp_node->child->value.text.string, VMS_STATE_POWEREDDOWN) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 0;
                        }
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_SUSPENDED) == 0 ||
							strcmp(tmp_node->child->value.text.string, VMS_STATE_PAUSED) == 0 ||
							strcmp(tmp_node->child->value.text.string, VMS_STATE_SAVINGSTATE) == 0 || 
							strcmp(tmp_node->child->value.text.string, VMS_STATE_RESTORING) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 2;
						}
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_POWERINGUP) == 0 ||
							strcmp(tmp_node->child->value.text.string, VMS_STATE_WAITFORLAUNCH) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 3;
                        }
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_POWERINGDOWN) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 4;
                        }
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_REBOOTINPROCESS) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 7;
                        }
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_UNASSIGNED) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 8;
                        }
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_UNKNOW) == 0 || 
							strcmp(tmp_node->child->value.text.string, VMS_STATE_NOT_RESPONDING) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 11;
                        }
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_MIGRATING) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 10;
                        }
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_IMAGE_LOCKED) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 12;
                        }
						else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_IMAGE_ILLEGAL) == 0)
                        {
                        	g_vmsComUpdate[g_vmsComCount].status = 13;
                        }
                     }//status

                    //get memory
                    tmp_node = mxmlFindElement(heading, node, "memory",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        //printf(" vms memory : %s. \n", tmp_node->child->value.text.string);
                        g_vmsComUpdate[g_vmsComCount].memory = strtod(tmp_node->child->value.text.string, NULL)/(1024*1024*1024);
                        //printf(" vms memory long: %ld. \n", g_vmsComUpdate[g_vmsComCount].memory);
                    }//get memory

                    //get usb strategy
                    tmp_node = mxmlFindElement(heading, node, "usb",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        mxml_node_t * typeNode = mxmlFindElement(tmp_node, heading, "enabled",
                                                 NULL, NULL,
                                                 MXML_DESCEND);
                        if (typeNode)
                        {
                            //printf(" vms usb strategy : %s.\n", typeNode->child->value.text.string);
                            //strcpy(pNode->val.usb, typeNode->child->value.text.string);
                            if (strcmp(typeNode->child->value.text.string, "true") == 0)
                                g_vmsComUpdate[g_vmsComCount].usb = 1;
                            else
                                g_vmsComUpdate[g_vmsComCount].usb = 0;
                        }else{
                           //strcpy(pNode->val.usb, "");
                        }
                    }//usb strategy

                    //get address
					tmp_node = mxmlFindElement(heading, node, "display",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                    	mxml_node_t * typeNode = mxmlFindElement(tmp_node, heading, "address",
                                                 NULL, NULL,
                                                 MXML_DESCEND);
                        //printf(" vms address : %s. \n", tmp_node->child->value.text.string);
                        if (typeNode)
                        {
							strcpy(g_vmsComUpdate[g_vmsComCount].ip, typeNode->child->value.text.string);
                        }
						//port
	                    typeNode = mxmlFindElement(tmp_node, heading, "port",
	                                              NULL, NULL,
	                                              MXML_DESCEND);
	                    if (typeNode)
	                    {
	                       g_vmsComUpdate[g_vmsComCount].port = atoi(typeNode->child->value.text.string);
	                       //printf("g_vmsComUpdate.port : %d.\n", g_vmsComUpdate[g_vmsComCount].port);
	                       //LogInfo("Debug: g_vmsComUpdate.port :%d.\n", g_vmsComUpdate[g_vmsComCount].port);
	                    }
                    }
                    g_vmsComCount++;
                 }//if vm
             }//if null
          }
   }
	if (NULL != g_treeTmp)
	{
		mxmlDelete(g_treeTmp);
		g_treeTmp = NULL;
	}
   fclose(fp);
    return ;
	
}

int SY_GetVms()
{
    INIT_LIST_HEAD(&head);
    SY_FreeVmsList();
    g_nVmCount = 0;
    if (SY_Loadxml(FILE_OVIRT_INFO_PATH) < 0)
    {
        return -1;
    }
    FindNode("vms");
    SY_Unloadxml(g_tree);
    return 0;
}

int SY_GetVms2()
{
    InitVmsUpdate();
    FindNode2("vms");
    return 0;
}

int SY_GetVmsTicket(char * szTicket)
{
  	FILE *fp;
  	fp = fopen(FILE_OVIRT_TICKET_PATH, "r");
    if (fp)
    {
      	mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
      	if (g_tree != NULL)
      	{
      		mxml_node_t *node;
      		node = mxmlFindElement(g_tree, g_tree, "value",
      												 NULL, NULL,
      												 MXML_DESCEND);
      		if (node != NULL)
      		{
      			 //printf("SY Get Vms Ticket: %s.\n", node->child->value.text.string);
             strcpy(szTicket, node->child->value.text.string);
      		}
			node = mxmlFindElement(g_tree, g_tree, "state",
      												 NULL, NULL,
      												 MXML_DESCEND);
      		if (node != NULL)
      		{
      			 //printf("SY Get Vms Ticket: %s.\n", node->child->value.text.string);
				 if (strcmp(node->child->value.text.string, "failed") == 0)
				 {
				 	if (NULL != fp)
						fclose(fp);
             			return -1;
				 }
      		}
      	}
		if (NULL != g_tree)
		{
			mxmlDelete(g_tree);
			g_tree = NULL;
		}
      	fclose(fp);
    }//if
    return 0;
}

unsigned short SY_GetVmState(char* vmid)
{
    unsigned short state = 0;
    FILE *fp;
    char szTmp[MAX_BUFF_SIZE] = {0};
    strcat(szTmp, "tmp/");
    strcat(szTmp, vmid);
    strcat(szTmp, ".xml");
    fp = fopen(szTmp, "r");
    if (fp)
    {
        mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
        if (g_tree != NULL)
        {
            mxml_node_t *node;
            node = mxmlFindElement(g_tree, g_tree, "state",
                                 NULL, NULL,
                                 MXML_DESCEND);
            if (node != NULL)
            {
                 //printf("SY_GetVmsState: %s.\n", node->child->value.text.string);
                 if (node->child)
                 {
					    if (strcmp(node->child->value.text.string, VMS_STATE_UP) == 0)
		                {
		                	state = 1;
		                }
		                else if (strcmp(node->child->value.text.string, VMS_STATE_DOWN) == 0 || 
							strcmp(node->child->value.text.string, VMS_STATE_POWEREDDOWN) == 0)
		                {
		                	state = 0;
						}
		                else if (strcmp(node->child->value.text.string, VMS_STATE_SUSPENDED) == 0 ||
							strcmp(node->child->value.text.string, VMS_STATE_PAUSED) == 0 || 
							strcmp(node->child->value.text.string, VMS_STATE_SAVINGSTATE) == 0 || 
							strcmp(node->child->value.text.string, VMS_STATE_RESTORING) == 0)
		                {
		                	state = 2;
						}
		                else if (strcmp(node->child->value.text.string, VMS_STATE_POWERINGUP) == 0 || 
							strcmp(node->child->value.text.string, VMS_STATE_WAITFORLAUNCH) == 0)
		                {
		                	state = 3;
						}
		                else if (strcmp(node->child->value.text.string, VMS_STATE_POWERINGDOWN) == 0)
		                {
		                	state = 4;
		                }
		                else if (strcmp(node->child->value.text.string, VMS_STATE_REBOOTINPROCESS) == 0)
		                {
		                	state = 7;
		                }
						else if (strcmp(node->child->value.text.string, VMS_STATE_UNASSIGNED) == 0)
						{
							state = 8;
						}
						else if (strcmp(node->child->value.text.string, VMS_STATE_UNKNOW) == 0 || 
							strcmp(node->child->value.text.string, VMS_STATE_NOT_RESPONDING) == 0)
						{
							state = 11;
						}
						else if (strcmp(node->child->value.text.string, VMS_STATE_MIGRATING) == 0)
						{
							state = 10;
						}
						else if (strcmp(node->child->value.text.string, VMS_STATE_IMAGE_LOCKED) == 0)
						{
							state = 12;
						}
						else if (strcmp(node->child->value.text.string, VMS_STATE_IMAGE_ILLEGAL) == 0)
						{
							state = 13;
						}
                 }
            }//if
        }
		if (NULL != g_tree)
		{
			mxmlDelete(g_tree);
			g_tree = NULL;
		}
		fclose(fp);
    }
    return state;
}

void SY_GetPostResult(int* ret, char* msg)
{
	if (NULL == msg || NULL == ret)
	{
		return;
	}
	FILE *fp = NULL;
	fp = fopen(FILE_OVIRT_TICKET_PATH, "r");
	if (NULL == fp)
	{
		return;
	}
	char szTmp[1024]= {0};
	mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK); 
	if (NULL != g_tree)
	{
		mxml_node_t *node = mxmlFindElement(g_tree, g_tree, "state", NULL, NULL, MXML_DESCEND);
		if (NULL != node && NULL != node->child)
		{
		    LogInfo("SY_GetPostResult 1111, state=%s", node->child->value.opaque);
			if (strcmp(node->child->value.opaque, "failed") == 0)
			{
				*ret = -1;
				node = mxmlFindElement(g_tree, g_tree, "detail", NULL, NULL, MXML_DESCEND);
				if (NULL != node && NULL != node->child)
				{
				    LogInfo("SY_GetPostResult 1111, detail=%s", node->child->value.opaque);
					snprintf(msg, MAX_BUFF_SIZE, "%s", node->child->value.opaque);
				}
			}else
			{
				*ret = 0;
			}
		}
	}
	if (NULL != g_tree)
	{
		mxmlDelete(g_tree);
		g_tree = NULL;
	}
	fclose(fp);
}

