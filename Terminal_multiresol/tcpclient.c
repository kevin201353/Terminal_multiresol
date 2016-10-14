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
static void *thrd_connect(void *arg);
static pthread_t tid;
static int sockfd;
static struct sockaddr_in server_addr; //描述服务器的地址
static int nbytes = 0;
static char buffer[1024];
static int g_thrdExit = 0;
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
			fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
			gettimeofday(&end,NULL);
			timer = end.tv_sec - start.tv_sec;
			printf("tcpclient connect tcpserver timeout value = %d.\n", timer);
			if (timer >= 5)
			{
			    printf("tcpclient connect tcpserver timeout exit.\n");
				break;
			}
			sleep(1);
			continue;
		}
		printf("tcpclient connect tcpserver successed.\n");
		break;
	}
	/* 连接成功了 */
	printf("exit thrd func .\n");
}

int start_tcpclient()
{
	g_thrdExit = 0;
	/* 客户程序开始建立 sockfd描述符 */
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) // AF_INET:Internet;SOCK_STREAM:TCP
	{
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
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
        fprintf(stderr,"pthread_create Error:%s\a\n",strerror(errno));
		return -1;
    }
	pthread_join(tid, NULL);
	return 0;
}

int send_data(struct Report data)
{
	if (nbytes = send(sockfd, &data, sizeof(struct Report), 0) == -1)
	{
		fprintf(stderr,"Connect Error:%s\a\n",strerror(errno));
		return -1;
	}
	return 0;
}

int close_tcpclient()
{
    g_thrdExit = 1;
	close(sockfd);
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


