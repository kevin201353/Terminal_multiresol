#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_ntoa()函数的头文件
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include "msg.h"

static void *thrd_connect(void *arg);
static pthread_t conn_tid;
static pthread_t recv_tid;
static pthread_t deal_tid;


static int sockfd;
static struct sockaddr_in server_addr; //描述服务器的地址
static char buffer[MAX_BUFF_SIZE] = {0};

static pthread_mutex_t mutex;
static int  g_read_len = 0;

extern void LogInfo(const char* ms, ... );

//void msg_respose(struct ReportMsg msg)
//{
//	switch(msg.action)
//	{
//		case MSG_WINDOW_CLOSE:
//			g_thrdMsgExit = 1;
//			LogInfo("msg_respose : recved close window msg.\n");
//			break;
//		default:
//			break;
//	}
//}  

void call_msg_back(MsgCallBackFun fun, struct ReportMsg msg)
{
	fun(msg);
}

static void *thrd_connect(void *arg)
{
	//设置为阻塞模式
	/* 客户程序发起连接请求 */
	struct  timeval  start;
	struct  timeval  end;
	unsigned long timer;

	gettimeofday(&start,NULL);
	for (;;)
	{
	    if (g_thrdMsgExit == 1)
		{
			LogInfo("Debug : thrd_connect exit, g_thrdMsgExit = %d .\n",  g_thrdMsgExit);
			break;
		}
		if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr)) == -1)
		{
			LogInfo("Connect Error:%s\a\n", strerror(errno));
			gettimeofday(&end,NULL);
			timer = end.tv_sec - start.tv_sec;
			LogInfo("tcpclient connect tcpserver timeout value 222 = %d.\n", timer);
			if (timer >= 5)
			{
				LogInfo("tcpclient connect tcpserver timeout exit 222.\n");
				break;
			}
			sleep(1);
			continue;
		}
		break;
	}
	LogInfo("tcpclient connect tcpserver successed 222.\n");
}


static void *thrd_recvdata(void *arg)
{
	do
	{
	 	if (g_thrdMsgExit == 1)
		{
			LogInfo("Debug : thrd_recvdata exit, g_thrdMsgExit = %d .\n",  g_thrdMsgExit);
			break;
		}
		LogInfo("thrd_recvdata start recv .\n");
		pthread_mutex_lock(&mutex);
	    int nRet = read(sockfd, &buffer, MAX_BUFF_SIZE*sizeof(char));
		g_read_len = nRet;
		pthread_mutex_unlock(&mutex);
		if( nRet == -1)
		{
			LogInfo("thrd_dealdata failed :%s\a\n", strerror(errno));
			return -1;
		}
		if (nRet == 0)
		{
		    LogInfo("Debug: thrd_dealdata reach the end of file nRet = 0.\n");
		}
		LogInfo("thrd_dealdata success, data len  = %d .\n", nRet);
		sleep(1);
	}while(1);
}

static void thrd_dealdata(void *arg)
{
    while(1)
    {
		if (g_thrdMsgExit == 1)
		{
			LogInfo("Debug : thrd_dealdata exit, g_thrdMsgExit = %d .\n",  g_thrdMsgExit);
			break;
		}
		LogInfo("thrd_dealdata start deal .\n");
		pthread_mutex_lock(&mutex);
		if (g_read_len > 0)
		{
			//收到Msg
			struct ReportMsg * msg = (struct ReportMsg *)buffer;
			char szTmp[MAX_BUFF_SIZE] = {0};
			sprintf(szTmp, "Debug: client get_msg_value = %d .\n", msg->action);
			LogInfo(szTmp);
			call_msg_back(msg_respose, *msg);
			g_read_len = 0;
			memset(buffer, 0, MAX_BUFF_SIZE);
		}
		pthread_mutex_unlock(&mutex);
		sleep(1);
    }
}

int start_msg_sender()
{
	/* 客户程序开始建立 sockfd描述符 */
	if (sockfd > 0)
		close(sockfd);
	pthread_mutex_init(&mutex,NULL);
	g_read_len = 0;
	g_thrdMsgExit = 0;
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd  == -1) //AF_INET:Internet;SOCK_STREAM:TCP
	{
		LogInfo("Socket Error 222 :%s\a\n", strerror(errno));
		return -1;
	}
	
	/* 客户程序填充服务端的资料 */
	bzero(&server_addr,sizeof(server_addr)); // 初始化,置0
	server_addr.sin_family = AF_INET; // IPV4
	server_addr.sin_port = htons(msgport); // (将本机器上的short数据转化为网络上的short数据)端口号
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP地址
	int flags = fcntl(socket, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);
	 // 创建线程tid，且线程函数由thrd_connect指向，是thrd_connect的入口点，即马上执行此线程函数
	if ( pthread_create(&conn_tid, NULL, thrd_connect, NULL) != 0 ) {
		LogInfo("pthread_create Error 222 :%s\a\n", strerror(errno));
		return -1;
	}
	pthread_join(conn_tid, NULL);

	if ( pthread_create(&recv_tid, NULL, thrd_recvdata, NULL) != 0 ) {
		LogInfo("thrd_recvdata Error 222 :%s\a\n", strerror(errno));
		return -1;
	}
	
	if ( pthread_create(&deal_tid, NULL, thrd_dealdata, NULL) != 0 ) {
		LogInfo("thrd_dealdata Error 222 :%s\a\n", strerror(errno));
		return -1;
	}
	LogInfo("Debug : thrd_recvdata , thrd_dealdata end. \n");
	return 0;
}

int send_msg(struct ReportMsg data)
{
	int Ret = send(sockfd, &data, sizeof(struct ReportMsg), 0);
	if ( Ret == -1)
	{
		LogInfo("send_data Connect Error 222 :%s\a\n", strerror(errno));
		return -1;
	}
    LogInfo("send data bytes 222  = %d.\n", Ret);
	return 0;
}

int close_msg_sender()
{
	pthread_join(conn_tid, NULL);
	pthread_join(deal_tid, NULL);
	pthread_join(recv_tid, NULL);
	pthread_mutex_destroy(&mutex);
	if (sockfd > 0)
		close(sockfd);
	g_read_len = 0;
	g_thrdMsgExit = 0;
}

/* int main(int argc, char *argv[])
{
	g_thrdExit = 0;
	start_msg_sender();
 	report_msg.action = MSG_CONNECTING_VM;
	send_msg(report_msg);
	/*report_msg.action = MSG_SERVER_CLOSE;
	send_data(report_msg); */
//	sleep(4);
//	close_msg_sender();
//	return 0;
//} */


