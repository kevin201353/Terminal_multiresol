#ifndef  _SY_VMS_H
#define  _SY_VMS_H

#include <mxml.h>  //minixml 解析库
#include <string.h>
#include "global.h"
#include <stdlib.h>

#define   VMS_STATE_UP    				"up"    // 1   运行
#define   VMS_STATE_DOWN   				"down"   //0  已关机
#define   VMS_STATE_SUSPENDED  			"suspended"  //2    已暂停
#define   VMS_STATE_POWERINGUP  		"powering_up"  //3  正在启动
#define   VMS_STATE_POWERINGDOWN  		"powering_down" //4  正在关机
#define   VMS_STATE_SAVINGSTATE   		"saving_state"  //2 已暂停
#define   VMS_STATE_PAUSED   			"paused" //2    已暂停
#define   VMS_STATE_REBOOTINPROCESS  	"reboot_in_progress" //7  正在重启
#define   VMS_STATE_UNASSIGNED       	"unassigned" //8 初始化中
#define   VMS_STATE_POWEREDDOWN         "powered_down"  //0 已关机
#define   VMS_STATE_WAITFORLAUNCH     	"wait_for_launch"  // 3 正在启动
#define   VMS_STATE_MIGRATING           "migrating" //10 正在迁移
#define   VMS_STATE_UNKNOW              "unknow"  // 11错误
#define   VMS_STATE_NOT_RESPONDING      "not_responding" // 11 错误
#define   VMS_STATE_IMAGE_LOCKED        "image_locked" // 12 镜像锁定
#define   VMS_STATE_IMAGE_ILLEGAL		"image_illegal" // 13 非法镜像
#define   VMS_STATE_RESTORING    		"restoring_state"  // 2 已暂停

int SY_Loadxml(char *file);
void SY_Unloadxml(mxml_node_t * node);
void FindNode(char* value);
int SY_GetVms();
void SY_FreeVmsList();
unsigned short SY_GetVmState(char* vmid);
void SY_GetPostResult(int* ret, char* msg);

#endif  //_SY_VMS_H
