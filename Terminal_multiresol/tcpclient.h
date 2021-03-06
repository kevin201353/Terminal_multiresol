#ifndef __TCP_CLIENT_H_
#define __TCP_CLIENT_H_
#pragma pack(1)
struct Report {
	char uname[64];
	char vname[64];
	unsigned int action; //用户动作（登录=1，连接=2，断开连接=3，退出=4）
}report;

#pragma pack()

int start_tcpclient();
int send_data(struct Report data);
int close_tcpclient();
void del_xxfile();
void wirte_conflag_data(char* path_file, char* data);


#endif  //__TCP_CLIENT_H_
