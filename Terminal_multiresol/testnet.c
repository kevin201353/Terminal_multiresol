#include "global.h"
#include <linux/sockios.h>	
#include <sys/socket.h>  
#include <sys/ioctl.h>	
#include <linux/if.h>  
#include <string.h>  
#include <stdio.h>	
#include <stdlib.h>  
#include <unistd.h>  
#include "msg.h"
#define ETHTOOL_GLINK		 0x0000000a /* Get link status (ethtool_value) */  
  
typedef enum { IFSTATUS_UP, IFSTATUS_DOWN, IFSTATUS_ERR } interface_status_t;  
static char hw_name[10] = {'\0'};
static pthread_t tid;
static GtkBuilder *g_builder;
extern GdkPixbuf *g_netstatus_Up;
extern GdkPixbuf *g_netstatus_Down;
static GThread *g_cthread;
static int g_auto_login = 0;
static struct LoginInfo infot = {"", "", "", "", 3389, 0, 0, 0};


/* for passing single values */  
struct ethtool_value  
{  
	u32    cmd;  
	u32    data;  
};	
void check_net_status(GtkWidget *widget);
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
       if (g_auto_login == 1)
	   	break;
      //usleep (100000); /* 0.1 s */
      sleep(1);
      gdk_threads_add_idle ((GSourceFunc)update_widget, widget);
    }

  /* Make sure this thread is joined properly */
  gdk_threads_add_idle ((GSourceFunc)terminate, g_thread_self());
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
  
	/* »ñÈ¡Íø¿¨Ãû³Æ */	
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
					LogInfo("thrd_net_setup 22222222222222222222 .\n");
					if ( ShenCloud_autoLogin() < 0 )
					{
					    report_msg.action = MSG_WINDOW_CLOSE;
						server_send_msg(report_msg);
						LogInfo("thrd_net_setup 33333333333333 .\n");
					}	
					g_auto_login = 1;
				}
			}
			break;	
		  
		case IFSTATUS_DOWN:  
			//printf("%s : link down\n", hw_name);
			gtk_image_set_from_pixbuf(GTK_IMAGE(image_netstatus), g_netstatus_Down);
			break;	
		  
		default:  
			//printf("Detect Error\n");
			gtk_image_set_from_pixbuf(GTK_IMAGE(image_netstatus), g_netstatus_Down);
			break;	
	}
}

void Net_status_checking(GtkBuilder *builder, GtkWidget *widget)
{
	g_builder = builder;
	gethw_name();
	g_auto_login = 0;
	g_thread_new ("netstatus", (GThreadFunc)thread_func, widget);
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

