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
static GtkBuilder *g_builder;
extern GdkPixbuf *g_netstatus_Up;
extern GdkPixbuf *g_netstatus_Down;

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

static gboolean terminate (GThread *thread)
{
   g_thread_join(thread);
   return FALSE;
}

static void *thread_func (GtkWidget *widget)
{
  for (;;)
    {
      //usleep (100000); /* 0.1 s */
      sleep(5);
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
			//printf("%s : link up\n", hw_name);
			gtk_image_set_from_pixbuf(GTK_IMAGE(image_netstatus), g_netstatus_Up);
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
	g_thread_new ("netstatus", (GThreadFunc)thread_func, widget);
}

//int main (int argc, char *argv[])  
//{   
	//g_thread_new ("netstatus", (GThreadFunc)thread_func, label_time);
	//return 0;  
//}  

