#ifndef __MSG_H_
#define __MSG_H_
#include "global.h"

#pragma pack(1)
struct ReportMsg {
	unsigned int action; //消息类型
}report_msg;

#pragma pack()

#define  MAX_BUFF_SIZE  	100
#define  msgport 		5801 //define msg port

#define MSG_WINDOW_CLOSE     100
#define MSG_CONNECTING_VM    101

int g_thrdMsgExit;


typedef void (*MsgCallBackFun)(struct ReportMsg msg);
void msg_respose(struct ReportMsg msg);
void call_msg_back(MsgCallBackFun fun, struct ReportMsg msg);

//sender
int start_msg_sender();
int send_msg(struct ReportMsg data);
int close_msg_sender();

//server
void close_msg_server();
int server_send_msg(struct ReportMsg data);
int server_init();


#endif  //__MSG_H

