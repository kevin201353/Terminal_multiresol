#include "global.h"
#include <linux/sockios.h>	
#include <sys/socket.h>  
#include <sys/ioctl.h>	
#include <linux/if.h>  
#include <string.h>  
#include <stdio.h>	
#include <stdlib.h>  
#include <unistd.h>  
  
#define ETHTOOL_GLINK		 0x0000000a /* Get link status (ethtool_value) */  
  
typedef enum { IFSTATUS_UP, IFSTATUS_DOWN, IFSTATUS_ERR } interface_status_t;  
static char hw_name[10] = {'\0'};
static pthread_t tid;
static pthread_t tid_autologin;

static GtkBuilder *g_builder;
extern GdkPixbuf *g_netstatus_Up;
extern GdkPixbuf *g_netstatus_Down;
static GThread *g_cthread;
static int g_auto_login = 0;
static struct LoginInfo infot = {"", "", "", "", 3389, 0, 0, 0};
static int ping_count = 0;



/* for passing single values */  
struct ethtool_value  
{  
	u32    cmd;  
	u32    data;  
};	
void check_net_status(GtkWidget *widget);

static void* thrd_auto_login()
{
	while(1)
	{
		if (g_auto_login == 1)
			break;
		sleep(2);
	}
	if (g_auto_login == 1)
	{
		struct ServerInfo  serverInfo;
		GetServerInfo2(&serverInfo);
		while(ping_count >= 20)
		{
			int nRet = ping_net(serverInfo.szIP);
			if (nRet == 1)
			{
				break;
			}
			ping_count++;
			sleep(2);
		}
	}
}

//return 0: net Unreachable
int ping_net(char *ip)
{
	char szDebug[512] = {0};
	if (ip != NULL)
	{
		char szbuf[100] = {0};
		char data[100] = {0};
		sprintf(szbuf, "sudo ping %s -c 4 > /usr/local/shencloud/log/ping_net.log", ip);
		system(szbuf);
		FILE *fp = fopen("/usr/local/shencloud/log/ping_net.log", "r");
		if (fp != NULL)
		{
			while(fgets(data, 100, fp) != NULL)
			{
				if (strstr(data, "ttl=")!= NULL && strstr(data, "time=") != NULL)
				{
					sprintf(szDebug, "ping_net : %s,  network = 1", ip);
					LogInfo(szDebug);
					return 1;
				}else if (strstr(data, "Destination Host Unreachable"))
				{
					sprintf(szDebug, "ping_net : %s,  network = 0", ip);
					LogInfo(szDebug);
					return 0;
				}
				sprintf(szDebug, "ping_net ip: %s, data: %s", ip, data);
			     LogInfo(szDebug);
			}
			fclose(fp);
		}
	}
	return 0;
}

static gboolean update_widget (GtkWidget *widget)
{
	check_net_status(widget);
	return FALSE;
}

void call_msg_win_back(MsgCallWin fun, gpointer data)
{
	fun(data);
}

static gboolean terminate (GThread *thread)
{
   g_thread_join(thread);
   call_msg_win_back(msg_respose_win, 1);
   return FALSE;
}

static void *thread_func (GtkWidget *widget)
{
  for (;;)
    {
       //if (g_auto_login == 1)
	   //	break;
      //usleep (100000); /* 0.1 s */
#ifdef ARM
	usleep(3 * 100000);
#else
      sleep(3);
#endif
      gdk_threads_add_idle ((GSourceFunc)update_widget, widget);
    }

  /* Make sure this thread is joined properly */
  gdk_threads_add_idle ((GSourceFunc)terminate, g_thread_self());
  return NULL;
}

static gboolean login_terminate(GThread *thread)
{
	g_thread_join(thread);
	call_msg_win_back(msg_respose_win, 2);
	return FALSE;
}

static void *autologin_func(GtkWidget *widget)
{
	thrd_auto_login();
	gdk_threads_add_idle ((GSourceFunc)login_terminate, g_thread_self());
	return NULL;
}

interface_status_t interface_detect_beat_ethtool(int fd, char *iface)  
{  
	struct ifreq ifr;  
	struct ethtool_value edata;  
	 
	memset(&ifr, 0, sizeof(ifr));  
	strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);  
  
	edata.cmd = ETHTOOL_GLINK;	
	ifr.ifr_data = (struct sockaddr_in*) &edata;  
  
	if (ioctl(fd, SIOCETHTOOL, &ifr) == -1)  
	{  
		perror("ETHTOOL_GLINK failed ");  
		return IFSTATUS_ERR;  
	}  
  
	return edata.data ? IFSTATUS_UP : IFSTATUS_DOWN;  
}  

void gethw_name()
{
	FILE *fp;  
	char buf[512] = {'\0'};  
	char *token = NULL;  
  
	/* ��ȡ�������� */	
	if ((fp = fopen("/proc/net/dev", "r")) != NULL)  
	{  
		while (fgets(buf, sizeof(buf), fp) != NULL)  
		{  
			if(strstr(buf, "eth") != NULL)	
			{		  
				token = strtok(buf, ":");  
				while (*token == ' ') ++token;	
				strncpy(hw_name, token, strlen(token));  
			}  
		}  
	}  
	fclose(fp);  
}

void check_net_status(GtkWidget *widget)
{
	int fd;
	interface_status_t status;
	GObject *image_netstatus;
	image_netstatus = gtk_builder_get_object (g_builder, "image_netstatus");
	  
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
	{  
		perror("socket ");	
		exit(0);  
	}  
	status = interface_detect_beat_ethtool(fd, hw_name);  
	close(fd);		
	switch (status)  
	{  
		case IFSTATUS_UP:
			{
				//printf("%s : link up\n", hw_name);
				gtk_image_set_from_pixbuf(GTK_IMAGE(image_netstatus), g_netstatus_Up);
				GetLoginInfo(&infot);
				LogInfo("thrd_net_setup 22222222222222222222 000000 g_auto_login = %d.\n", g_auto_login);
				if (infot.autologin == 1 && g_auto_login == 0)
				{
					//LogInfo("thrd_net_setup 22222222222222222222 .\n");
					g_auto_login = 1;
				}
			}
			break;	
		  
		case IFSTATUS_DOWN:  
			//printf("%s : link down\n", hw_name);
			gtk_image_set_from_pixbuf(GTK_IMAGE(image_netstatus), g_netstatus_Down);
			LogInfo("thrd_net_setup, network check status is down.");
			break;	
		  
		default:  
			//printf("Detect Error\n");
			gtk_image_set_from_pixbuf(GTK_IMAGE(image_netstatus), g_netstatus_Down);
			LogInfo("thrd_net_setup, network check status detect error.");
			break;	
	}
}

void Net_status_checking(GtkBuilder *builder, GtkWidget *widget)
{
	g_builder = builder;
	gethw_name();
	g_auto_login = 0;
	g_thread_new ("netstatus", (GThreadFunc)thread_func, widget);
//	if ( pthread_create(&tid_autologin, NULL, thrd_auto_login, NULL) !=0 )
//	{
//		printf("Create thread error!\n");
//	}
	g_thread_new ("autologin", (GThreadFunc)autologin_func, widget);

}

void login_exit_thrd()
{
	pthread_exit(tid_autologin);
}

//static pthread_t tid;
//static void* thrd_net_setup()
//{
//	for (;;)
//	{
//		int fd;
//		interface_status_t status;
//		GObject *image_netstatus;
//		image_netstatus = gtk_builder_get_object (g_builder, "image_netstatus");
//		  
//		if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
//		{  
//			perror("socket ");	
//			break; 
//		}
//		LogInfo("thrd_net_setup 333333333333333333 .\n");
//		status = interface_detect_beat_ethtool(fd, hw_name);  
//		close(fd);
//		if (status == IFSTATUS_UP)
//		{
//			LogInfo("thrd_net_setup 22222222222222222222 .\n");
//			ShenCloud_autoLogin();
//			break;
//		}
//		sleep(1);
//	}
//}

//void wait_net_setup()
//{
//	gethw_name();
//	if ( pthread_create(&tid, NULL, thrd_net_setup, NULL) !=0 ) 
//	{
//		printf("Create thread error!\n");
//	};
//	pthread_join(tid, NULL);
//	for (;;)
//	{
//		int fd;
//		interface_status_t status;
//		GObject *image_netstatus;
//		image_netstatus = gtk_builder_get_object (g_builder, "image_netstatus");
//		  
//		if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
//		{  
//			perror("socket ");	
//			break; 
//		}
//		LogInfo("thrd_net_setup 333333333333333333 .\n");
//		status = interface_detect_beat_ethtool(fd, hw_name);  
//		close(fd);
//		if (status == IFSTATUS_UP)
//		{
//			LogInfo("thrd_net_setup 22222222222222222222 .\n");
//			ShenCloud_autoLogin();
//			break;
//		}
//		sleep(1);
//	}
//}

//int main (int argc, char *argv[])  
//{   
	//g_thread_new ("netstatus", (GThreadFunc)thread_func, label_time);
	//return 0;  
//}  

