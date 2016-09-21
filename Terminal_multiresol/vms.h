#ifndef  _SY_VMS_H
#define  _SY_VMS_H

#include <mxml.h>  //minixml 解析库
#include <string.h>
#include "global.h"
#include <stdlib.h>

#define   VMS_STATE_UP    "up"    // 1
#define   VMS_STATE_DOWN   "down"   //0
#define   VMS_STATE_SUSPENDED  "suspended"  //2
#define   VMS_STATE_POWERINGUP  "powering_up"  //3
#define   VMS_STATE_POWERINGDOWN  "powering_down" //4
#define   VMS_STATE_SAVINGSTATE   "saving_state"  //5

int SY_Loadxml(char *file);
void SY_Unloadxml(mxml_node_t * node);
void FindNode(char* value);
int SY_GetVms();
void SY_FreeVmsList();
unsigned short SY_GetVmState(char* vmid);

#endif  //_SY_VMS_H
