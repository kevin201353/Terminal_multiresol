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
#include "tcpclient.h"
#include <sys/time.h>


#define portnumber 12321 //定义端口号：（0-1024为保留端口号，最好不要用）
#define MAX_DATA_SIZE   100
static void *thrd_connect(void *arg);
static pthread_t tid;
static int sockfd;
static struct sockaddr_in server_addr; //描述服务器的地址
static int nbytes = 0;
static char buffer[1024];
static int g_thrdExit = 0;
static int connect_refused = 0;
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
	    if (g_thrdExit == 1)
			break;
		if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr)) == -1)
		{
			//fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
			LogInfo("Connect Error:%s\a\n", strerror(errno));
			gettimeofday(&end,NULL);
			timer = end.tv_sec - start.tv_sec;
			//printf("tcpclient connect tcpserver timeout value = %d.\n", timer);
			LogInfo("tcpclient connect tcpserver timeout value = %d.\n", timer);
			if (timer >= 5)
			{
			    //printf("tcpclient connect tcpserver timeout exit.\n");
				LogInfo("tcpclient connect tcpserver timeout exit.\n");
				break;
			}
			sleep(1);
			continue;
		}
		//printf("tcpclient connect tcpserver successed.\n");
		LogInfo("tcpclient connect tcpserver successed.\n");
		break;
	}
	/* 连接成功了 */
	//printf("exit thrd func .\n");
	LogInfo("exit thrd func .\n");
}

int start_tcpclient()
{
	g_thrdExit = 0;
	connect_refused = 0;
	if (sockfd >= 0)
		close(sockfd);
	/* 客户程序开始建立 sockfd描述符 */
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) // AF_INET:Internet;SOCK_STREAM:TCP
	{
		//fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
		LogInfo("start_tcpclient Socket Error:%s\a\n", strerror(errno));
		return -1;
	}
	/* 客户程序填充服务端的资料 */
	bzero(&server_addr,sizeof(server_addr)); // 初始化,置0
	server_addr.sin_family = AF_INET; // IPV4
	server_addr.sin_port = htons(portnumber); // (将本机器上的short数据转化为网络上的short数据)端口号
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP地址
	int flags = fcntl(socket, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);
	 // 创建线程tid，且线程函数由thrd_connect指向，是thrd_connect的入口点，即马上执行此线程函数
    if ( pthread_create(&tid, NULL, thrd_connect, NULL) != 0 ) {
        //fprintf(stderr,"pthread_create Error:%s\a\n",strerror(errno));
		LogInfo("pthread_create Error:%s\a\n", strerror(errno));
		return -1;
    }
	pthread_join(tid, NULL);
	int nRet = ajust_auth();
	if ( nRet < 0  || nRet == 2)
	{
		connect_refused = 1;
		close_tcpclient();
	}
	return 0;
}

void del_xxfile()
{
	if (access("/tmp/data_xor", F_OK) != 0)
	{
		remove("/tmp/data_xor");
	}
	if (access("/tmp/data_port", F_OK) != 0)
	{
		remove("/tmp/data_port");
	}
}

void wirte_conflag_data(char* path_file, char* data)
{
	FILE* file = fopen(path_file, "a+");
	if (file != NULL)
	{
		fwrite(data,1,strlen(data), file);
		fclose(file);
	}
}

int send_data(struct Report data)
{
    if (connect_refused == 1)
		return 0;

	del_xxfile();
	int nRet = 0;
	nRet = send(sockfd, &data, sizeof(struct Report), 0);
	if ( nRet == -1)
	{
		//fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
		LogInfo("send_data Connect Error:%s\a\n", strerror(errno));
		return -1;
	}
	LogInfo("Debug: send data bytes = %d .\n", nRet);
	return 0;
}


//reuturn : error -1, failed 0, success 1  refused 2
int ajust_auth()
{
    char data[MAX_DATA_SIZE] = {0};
	strcpy(data, "thin_view");
	nbytes = send(sockfd, &data, sizeof(data), 0);
	if ( nbytes == -1)
	{
		//fprintf(stderr,"ajust_auth send data error :%s\n",strerror(errno));
		LogInfo("ajust_auth send data error:%s\a\n", strerror(errno));
		return -1;
	}
	memset(data, 0, MAX_DATA_SIZE);
	int ncount = 3;
	do
	{
	    nbytes = read(sockfd, &data, MAX_DATA_SIZE*sizeof(char));
		if( nbytes == -1)
		{
			//fprintf(stderr,"tcpclient read from server data failed :%s\n", strerror(errno));
			LogInfo("tcpclient read from server data failed :%s\a\n", strerror(errno));
			return -1;
		}
		if (nbytes == 0)
		{
		    LogInfo("Debug: ajust_auth reach the end of file .\n");
			return 0;
		}
		
		//printf("tcpclient ajust auth success, data = %s .\n", data);
		LogInfo("tcpclient ajust auth success, data = %s .\n", data);
		if (strcmp(data, "thin_ok") == 0)
		{
			return 1;
		}else if (strcmp(data, "thin_refused") == 0)
		{
			return 2;
		}
		usleep(600);
	}while(ncount-- > 0);
	return 0;
}

int close_tcpclient()
{
    g_thrdExit = 1;
	int i = 0;
	char szbuf[MAX_DATA_SIZE] = {0};
	fd_set fred;
	struct timeval tv;
	tv.tv_sec = 1;//超时时间
	tv.tv_usec = 0;
	int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, flags | O_NONBLOCK);
	for (; i<1; i++)
	{
		FD_ZERO(&fred);
		FD_SET(sockfd, &fred);
		int nRet = select(sockfd + 1, &fred, NULL, NULL, &tv);
		if (nRet == 0)
		{
			//time out
			LogInfo("tcpclient close_tcpclient select timeout .\n");
			close(sockfd);
			return 0;
		}
		if (FD_ISSET(sockfd, &fred))
		{
			int nret = read(sockfd, &szbuf, MAX_DATA_SIZE);
			if (nret == -1)
			{
			   LogInfo("tcpclient close_tcpclient read data error .\n");
			   close(sockfd);
			   return 0;
			}
			if (strcmp(szbuf, "bye") == 0)
			{
			   LogInfo("tcpclient close_tcpclient read data bye , len =%d.\n", nret);
			   close(sockfd);
			}
		}
	}
	return 0;
}

//int main(int argc, char *argv[])
//{
//	struct hostent *host;
//	strcpy(report.uname, "zhaosenhua");
//	strcpy(report.vname, "vm_stu1");
//	report.action = 1; 
//	/* 使用hostname查询host 名字 */
//	if(argc!=2)
//	{
//		fprintf(stderr,"Usage:%s hostname \a\n",argv[0]);
//		exit(1);
//	}
//	if((host = gethostbyname(argv[1]))==NULL)
//	{
//		fprintf(stderr,"Gethostname error\n");
//		exit(1);
//	}
//	start_tcpclient();
//	send_data(report);
//	strcpy(report.uname, "zhaosenhua22");
//	strcpy(report.vname, "vm_stu122");
//	report.action = 2;
//	send_data(report);
//	printf("main process exit.\n");
//	close_tcpclient();
//	return 0;
//}


