#ifndef __TCP_CLIENT_H_
#define __TCP_CLIENT_H_

struct Report {
	char uname[64];
	char vname[64];
	unsigned int action; //�û���������¼=1������=2���Ͽ�����=3���˳�=4��
}report;

int start_tcpclient();
int send_data(struct Report data);
int close_tcpclient();

#endif  //__TCP_CLIENT_H_
