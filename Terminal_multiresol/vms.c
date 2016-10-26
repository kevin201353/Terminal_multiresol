/***********
//filename: vms.c
//author: zhaosenhua
//version: 1.0
//funciton list: ...
//description: 获取服务器虚拟机列表处理模块
//Date: 2016/6/23
**************/
#include "vms.h"
#include<sys/stat.h>
#include<unistd.h>

mxml_node_t *g_tree = NULL;  //用来解析xml文件
mxml_node_t *g_treeTmp = NULL;  //用来解析xml文件

void InitVmsUpdate()
{
    for (int i=0; i<MAX_BUFF_SIZE; i++)
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
    printf("xml file: %s.\n", file);
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
    LogInfo("SY_LoadxmlTmp $$$$$$$$$$$, file size :%d .\n", len);
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
    printf("xml file: %s.\n", file);
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
    /*  list_for_each(plist, &head)
      {
          printf("SY_FreeList start.\n");
          struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
          if (node)
          {
              free(node);
          }
          printf("SY_FreeList end.\n");
      }*/
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
    node = mxmlFindElement(g_tree, g_tree, value,
                              NULL, NULL,
                              MXML_DESCEND);
   printf("1111  node %s\n", mxmlGetElement(node));
   if (node)
   {
     for (heading = mxmlGetFirstChild(node);
          heading;
          heading = mxmlGetNextSibling(heading))
          {
            //printf("[xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
          //  printf("[xml :xxx node 22 : %s].\n", mxmlGetElement (heading));
            //LogInfo("Debug: [xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
             if (mxmlGetElement(heading) != NULL)
             {
                 //printf("11111111111111.\n");
                 if (strcmp(mxmlGetElement(heading), "vm") == 0)
                 {
                     printf("[xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
                     printf("[xml :xxx node 22 : %s].\n", mxmlGetElement (heading));

                     struct Vms_Node *pNode = malloc(sizeof(struct Vms_Node));
                     if (pNode == NULL)
                     {
                        printf("findnode malloc Vms_node failed.\n");
                        LogInfo("Debug: findnode malloc Vms_node failed. \n");
                        return ;
                     }
                    strcpy(pNode->val.vmid, mxmlElementGetAttr(heading, "id"));
                     //get vms name
                     mxml_node_t * tmp_node = mxmlFindElement(heading, node, "name",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     printf("222222222, get vms attribute.\n");
                     if (tmp_node)
                     {
                        strcpy(pNode->val.name, tmp_node->child->value.text.string);
                        printf("pNode-val.name : %s.\n", pNode->val.name);
                        printf(" vms name : %s. \n", tmp_node->child->value.text.string);
                        LogInfo("Debug: pNode-val.name : %s.\n", pNode->val.name);
                     }//if name

                     //get vms os
                     tmp_node = mxmlFindElement(heading, node, "os",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     if (tmp_node)
                     {
                        strcpy(pNode->val.os, mxmlElementGetAttr(tmp_node, "type"));
                        printf("pNode-val.os : %s.\n", pNode->val.os);
                        LogInfo("Debug: pNode-val.os :%s.\n", pNode->val.os);
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
                        printf(" vms status : %s. \n", tmp_node->child->value.text.string);
                        if (strcmp(tmp_node->child->value.text.string, VMS_STATE_UP) == 0)
                          pNode->val.status = 1;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_DOWN) == 0)
                          pNode->val.status = 0;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_SUSPENDED) == 0)
                          pNode->val.status = 2;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_POWERINGUP) == 0)
                          pNode->val.status = 3;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_POWERINGDOWN) == 0)
                          pNode->val.status = 4;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_SAVINGSTATE) == 0)
                          pNode->val.status = 5;
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
                            const char *strcpu = mxmlElementGetAttr(cpuNode, "sockets");
                            if (strcmp(strcpu, "") >= 0)
                            {
                                printf(" vms cpu count : %s. \n", strcpu);
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
                        printf(" vms memory : %s. \n", tmp_node->child->value.text.string);
                        pNode->val.memory = strtod(tmp_node->child->value.text.string, NULL)/(1024*1024*1024);
                        printf(" vms memory long: %ld. \n", pNode->val.memory);
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
                            printf(" vms usb strategy : %s.\n", typeNode->child->value.text.string);
                            //strcpy(pNode->val.usb, typeNode->child->value.text.string);
                            if (strcmp(typeNode->child->value.text.string, "true") == 0)
                                pNode->val.usb = 1;
                            else
                                pNode->val.usb = 0;
                        }else{
                           //strcpy(pNode->val.usb, "");
                        }
                    }//usb strategy

                    //get address
                    tmp_node = mxmlFindElement(heading, node, "address",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        printf(" vms address : %s. \n", tmp_node->child->value.text.string);
                        strcpy(pNode->val.ip, tmp_node->child->value.text.string);
                    }

                    tmp_node = mxmlFindElement(heading, node, "port",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                       pNode->val.port = atoi(tmp_node->child->value.text.string);
                       printf("pNode-val.port : %d.\n", pNode->val.port);
                       LogInfo("Debug: pNode-val.port :%d.\n", pNode->val.port);
                    }//os

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
    if (g_treeTmp == NULL)
    {
        printf("findNode g_treeTmp == NULL , return.\n");
        return ;
    }
    mxml_node_t *node = NULL;
    mxml_node_t *heading = NULL;
    node = mxmlFindElement(g_treeTmp, g_treeTmp, value,
                              NULL, NULL,
                              MXML_DESCEND);
   printf("1111  node %s\n", mxmlGetElement(node));
   if (node)
   {
     for (heading = mxmlGetFirstChild(node);
          heading;
          heading = mxmlGetNextSibling(heading))
          {
            //printf("[xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
          //  printf("[xml :xxx node 22 : %s].\n", mxmlGetElement (heading));
            //LogInfo("Debug: [xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
             if (mxmlGetElement(heading) != NULL)
             {
                 //printf("11111111111111.\n");
                 if (strcmp(mxmlGetElement(heading), "vm") == 0)
                 {
                     //printf("[xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
                     //printf("[xml :xxx node 22 : %s].\n", mxmlGetElement (heading));

                    strcpy(g_vmsComUpdate[g_vmsComCount].vmid, mxmlElementGetAttr(heading, "id"));
                     //get vms name
                     mxml_node_t * tmp_node = mxmlFindElement(heading, node, "name",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     printf("222222222, get vms attribute.\n");
                     if (tmp_node)
                     {
                        strcpy(g_vmsComUpdate[g_vmsComCount].name, tmp_node->child->value.text.string);
                        //printf("g_vmsComUpdate.name : %s.\n", g_vmsComUpdate[g_vmsComCount].name);
                        //printf(" vms name : %s. \n", tmp_node->child->value.text.string);
                        LogInfo("Debug: g_vmsComUpdate.name : %s.\n", g_vmsComUpdate[g_vmsComCount].name);
                     }//if name

                     //get vms os
                     tmp_node = mxmlFindElement(heading, node, "os",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     if (tmp_node)
                     {
                        strcpy(g_vmsComUpdate[g_vmsComCount].os, mxmlElementGetAttr(tmp_node, "type"));
                        printf("g_vmsComUpdate.os : %s.\n", g_vmsComUpdate[g_vmsComCount].os);
                        LogInfo("Debug: g_vmsComUpdate.os :%s.\n", g_vmsComUpdate[g_vmsComCount].os);
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
                        printf(" vms status : %s. \n", tmp_node->child->value.text.string);
                        if (strcmp(tmp_node->child->value.text.string, VMS_STATE_UP) == 0)
                          g_vmsComUpdate[g_vmsComCount].status = 1;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_DOWN) == 0)
                          g_vmsComUpdate[g_vmsComCount].status = 0;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_SUSPENDED) == 0)
                          g_vmsComUpdate[g_vmsComCount].status = 2;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_POWERINGUP) == 0)
                          g_vmsComUpdate[g_vmsComCount].status = 3;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_POWERINGDOWN) == 0)
                          g_vmsComUpdate[g_vmsComCount].status = 4;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_SAVINGSTATE) == 0)
                          g_vmsComUpdate[g_vmsComCount].status = 5;
                     }//status

                    //get memory
                    tmp_node = mxmlFindElement(heading, node, "memory",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        printf(" vms memory : %s. \n", tmp_node->child->value.text.string);
                        g_vmsComUpdate[g_vmsComCount].memory = strtod(tmp_node->child->value.text.string, NULL)/(1024*1024*1024);
                        printf(" vms memory long: %ld. \n", g_vmsComUpdate[g_vmsComCount].memory);
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
                            printf(" vms usb strategy : %s.\n", typeNode->child->value.text.string);
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
                    tmp_node = mxmlFindElement(heading, node, "address",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        printf(" vms address : %s. \n", tmp_node->child->value.text.string);
                        strcpy(g_vmsComUpdate[g_vmsComCount].ip, tmp_node->child->value.text.string);
                    }

                    //port
                    tmp_node = mxmlFindElement(heading, node, "port",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                       g_vmsComUpdate[g_vmsComCount].port = atoi(tmp_node->child->value.text.string);
                       //printf("g_vmsComUpdate.port : %d.\n", g_vmsComUpdate[g_vmsComCount].port);
                       LogInfo("Debug: g_vmsComUpdate.port :%d.\n", g_vmsComUpdate[g_vmsComCount].port);
                    }//os
                    g_vmsComCount++;
                 }//if vm
             }//if null
          }
   }
    return ;
}

int SY_GetVms()
{
    //printf("SY_Get Vms @@@@@ 11.\n");
    SY_FreeVmsList();
    INIT_LIST_HEAD(&head);
    g_nVmCount = 0;
    //printf("SY_Get Vms @@@@@ 22.\n");
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
    if (SY_LoadxmlTmp(FILE_OVIRT_INFOTMP_PATH) < 0)
    {
        return -1;
    }
    FindNode2("vms");
    SY_Unloadxml(g_treeTmp);
    return 0;
}

void SY_GetVmsTicket(char * szTicket)
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
      			 printf("SY Get Vms Ticket: %s.\n", node->child->value.text.string);
             strcpy(szTicket, node->child->value.text.string);
      		}
      	}
      	fclose(fp);
    }//if
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
                 printf("SY_GetVmsState: %s.\n", node->child->value.text.string);
                 if (strcmp(node->child->value.text.string, VMS_STATE_UP) == 0)
                   state = 1;
                 else if (strcmp(node->child->value.text.string, VMS_STATE_DOWN) == 0)
                   state = 0;
                 else if (strcmp(node->child->value.text.string, VMS_STATE_SUSPENDED) == 0)
                   state = 2;
                 else if (strcmp(node->child->value.text.string, VMS_STATE_POWERINGUP) == 0)
                   state = 3;
                 else if (strcmp(node->child->value.text.string, VMS_STATE_POWERINGDOWN) == 0)
                   state = 4;
                 else if (strcmp(node->child->value.text.string, VMS_STATE_SAVINGSTATE) == 0)
                   state = 5;
            }//if
        }
        fclose(fp);
    }
    return state;
}
