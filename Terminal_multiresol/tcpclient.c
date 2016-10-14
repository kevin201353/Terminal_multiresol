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
#include <unistd.h>
#include <fcntl.h>
#include "tcpclient.h"
#include <sys/time.h>


#define portnumber 12321 //����˿ںţ���0-1024Ϊ�����˿ںţ���ò�Ҫ�ã�
static void *thrd_connect(void *arg);
static pthread_t tid;
static int sockfd;
static struct sockaddr_in server_addr; //�����������ĵ�ַ
static int nbytes = 0;
static char buffer[1024];
static int g_thrdExit = 0;
static void *thrd_connect(void *arg)
{
	//����Ϊ����ģʽ
	/* �ͻ��������������� */
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
	/* ���ӳɹ��� */
	printf("exit thrd func .\n");
}

int start_tcpclient()
{
	g_thrdExit = 0;
	/* �ͻ�����ʼ���� sockfd������ */
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) // AF_INET:Internet;SOCK_STREAM:TCP
	{
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
		return -1;
	}
	/* �ͻ�����������˵����� */
	bzero(&server_addr,sizeof(server_addr)); // ��ʼ��,��0
	server_addr.sin_family = AF_INET; // IPV4
	server_addr.sin_port = htons(portnumber); // (���������ϵ�short����ת��Ϊ�����ϵ�short����)�˿ں�
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP��ַ
	int flags = fcntl(socket, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);
	 // �����߳�tid�����̺߳�����thrd_connectָ����thrd_connect����ڵ㣬������ִ�д��̺߳���
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
//	/* ʹ��hostname��ѯhost ���� */
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


