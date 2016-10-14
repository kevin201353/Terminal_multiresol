#ifndef __TCP_CLIENT_H_
#define __TCP_CLIENT_H_

struct Report {
	char uname[64];
	char vname[64];
	unsigned int action; //用户动作（登录=1，连接=2，断开连接=3，退出=4）
}report;

int start_tcpclient();
int send_data(struct Report data);
int close_tcpclient();

#endif  //__TCP_CLIENT_H_
