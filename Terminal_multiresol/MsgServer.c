#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntoa()������ͷ�ļ�
#include <pthread.h>
#include "msg.h"
#include <fcntl.h>

int sockfd, new_fd;
struct sockaddr_in server_addr; //������������ַ
struct sockaddr_in client_addr; //�����ͻ��˵�ַ
static int sin_size;
static char buffer[MAX_BUFF_SIZE] = {0};
static char sendbuff[MAX_BUFF_SIZE] = {0};
static pthread_t conntid;
static pthread_t deal_tid;
static pthread_t send_tid;
static int connecting = 0;

int exit_thrd = 0;
static pthread_mutex_t mutex;
static int g_read_len = 0;

void msg_respose(struct ReportMsg msg)
{
//	switch(msg.action)
//	{
//		case MSG_WINDOW_CLOSE:
//			printf("msg_respose : recved close window msg.\n");
//			break;
//		case MSG_SERVER_CLOSE:
//			exit_thrd = 1;
//			printf("msg_respose : exit server recv msg.\n");
//			break;
//		default:
//			break;
//	}
}

void call_msg_back(MsgCallBackFun fun, struct ReportMsg msg)
{
	fun(msg);
}

static void thrd_dealdata(void *arg)
{
	while(1)
	{
		if (exit_thrd == 1)
			break;
		pthread_mutex_lock(&mutex);
		if (g_read_len > 0)
		{
			//�յ�Msg
			struct ReportMsg * msg = (struct ReportMsg *)buffer;
			char szTmp[MAX_BUFF_SIZE] = {0};
			sprintf(szTmp, "Debug: server get_msg_value = %d .\n", msg->action);
			LogInfo(szTmp);
			call_msg_back(msg_respose, *msg);
			g_read_len = 0;
			memset(buffer, 0, MAX_BUFF_SIZE);
		}
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}

static void *thrd_connect(void *arg)
{
	int nbytes = 0;
	while(1)
	{
		if (exit_thrd == 1)
			break;

		/* ����������,ֱ���ͻ����������� */
		sin_size = sizeof(struct sockaddr_in);
		new_fd = accept(sockfd,(struct sockaddr *)(&client_addr), &sin_size);
		if(new_fd == -1)
		{
			LogInfo("Accept error 222 :%s\a\n", strerror(errno));
			return;
		}
		fprintf(stderr,"Server get connection from %s\n",inet_ntoa(client_addr.sin_addr)); // �������ַת����.�ַ���������ӡ������ն�
		connecting = 1;
		while(1)
		{
			if (exit_thrd == 1)
				break;
			pthread_mutex_lock(&mutex);  
			nbytes = read(new_fd, &buffer, sizeof(struct ReportMsg));
			g_read_len = nbytes;
			pthread_mutex_unlock(&mutex);
			if(nbytes == -1)
			{
				LogInfo("tcpserver read from client data failed.\n");
				break;
			}
			if (nbytes == 0)
			{
				LogInfo("tcpserver read from client data finish nbytes = 0.\n");
			}
			printf("read tcpclient msg data len = %d .\n", nbytes);
			sleep(1);
		}
		close(new_fd);
		connecting = 0;
		sleep(1);
	}
}

int server_init()
{
	exit_thrd = 0;
	pthread_mutex_init(&mutex,NULL);
	if (sockfd > 0)
	{
		close(sockfd);
	}
	connecting = 0;
	/* �������˿�ʼ����sockfd������ */
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd == -1) // AF_INET:IPV4;SOCK_STREAM:TCP
	{
		LogInfo("server_init Socket error 222 :%s\a\n", strerror(errno));
		return -1;
	}
	/* ����������� sockaddr�ṹ */
	bzero(&server_addr, sizeof(struct sockaddr_in)); //��ʼ��,��0
	server_addr.sin_family = AF_INET; //Internet
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //(���������ϵ�long����ת��Ϊ�����ϵ�long����)���κ�����ͨ�� //INADDR_ANY ��ʾ���Խ�������IP��ַ�����ݣ����󶨵����е�IP
	//server_addr.sin_addr.s_addr = inet_addr("192.168.1.1"); //���ڰ󶨵�һ���̶�IP,inet_addr���ڰ����ּӸ�ʽ��ipת��Ϊ����ip
	server_addr.sin_port = htons(msgport); //(���������ϵ�short����ת��Ϊ�����ϵ�short����)�˿ں�

	int flags = fcntl(socket, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);
	/* ����sockfd��������IP��ַ */
	int nRet = bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr));
	if(nRet == -1)
	{
		LogInfo("server_init Socket error 222 :%s\a\n", strerror(errno));
		return -1;
	}
	/* �����������ӵ����ͻ����� */
	nRet = listen(sockfd, 5);
	if( nRet == -1)
	{
		LogInfo("server_init Listen error 222 :%s\a\n", strerror(errno));
		return -1;
	}

	if ( pthread_create(&conntid, NULL, thrd_connect, NULL) != 0 ) 
	{
		LogInfo("pthread_create error 222 :%s\a\n", strerror(errno));
		return -1;
    }

	if ( pthread_create(&deal_tid, NULL, thrd_dealdata, NULL) != 0 ) 
	{
		LogInfo("pthread_create error 222 :%s\a\n", strerror(errno));
		return -1;
    }
	return 0;
}

void close_msg_server()
{
	exit_thrd = 1;
	connecting = 0;
	pthread_join(conntid, NULL);
	pthread_join(deal_tid, NULL);
	pthread_join(send_tid, NULL);
	pthread_mutex_destroy(&mutex);
	if (sockfd > 0)
		close(sockfd);
}

static void *thrd_send_data(void *arg)
{
	int total = sizeof(struct ReportMsg);
	int sender_len = 0;
	while((total - sender_len) > 0)
	{
		if (connecting > 0)
		{
			LogInfo("server start send data.\n");
			int Ret = send(new_fd, &sendbuff + sender_len, total - sender_len, 0);
			if ( Ret == -1)
			{
				LogInfo("send_data Connect Error 222 :%s\a\n", strerror(errno));
				return -1;
			}
			sender_len += Ret;
			LogInfo("send data bytes 222 sender_len = %d, Ret = %d.\n", sender_len, Ret);
		}
		sleep(1);
	}
	LogInfo("send data bytes finish sender_len = %d.\n", sender_len);
}

int server_send_msg(struct ReportMsg data)
{
    memset(sendbuff, 0, MAX_BUFF_SIZE);
    memcpy(sendbuff, &data, sizeof(struct ReportMsg));
	if ( pthread_create(&send_tid, NULL, thrd_send_data, NULL) != 0 ) 
	{
		LogInfo("pthread_create error 222 :%s\a\n", strerror(errno));
		return -1;
    }
	return 0;
}

//int main(int argc, char *argv[])
//{
//	server_init();
//	report_msg.action = MSG_CLIENT_CLOSE;
//	send_data(report_msg);
//	close_socket();
//	return 0;
//}


