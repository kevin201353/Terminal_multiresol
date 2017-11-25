#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "global.h"
#include <string.h>
#include <unistd.h>

GtkBuilder *g_builder = NULL;
#define IMAGE_CHECKBUTTON_NOR  "images2/checkbtnset_nor.png"
#define IMAGE_CHECKBUTTON_PRESS  "images2/checkbtnset_press.png"

GdkPixbuf *g_checkNorimage;
GdkPixbuf *g_checkPressimage;
static unsigned short g_checkrepass = 0;
int g_bnetstatic = 0;
struct NetStaticInfo{
     char szIP[MAX_BUFF_SIZE];
     char szNetMask[MAX_BUFF_SIZE];
     char szGatWay[MAX_BUFF_SIZE];
     char szDns[3][MAX_BUFF_SIZE];
};
//CallBackFun
extern void SYmsgCaller(CallBackFun fun, char *p);
extern int SYMsgFun(char *p);
//
static struct NetStaticInfo g_NetStaticInfo = {0,0,0,{0,0,0}};

static GtkWidget *Msgdialog;
void GetNetStaticInfo();
//字符串去空格
extern char *rtspace(char *str);
extern int check_ipv4_valid(char *s);
extern void setfontcolor(GtkWidget * widget, char *value);
static GtkWidget *btn_auto_dns;
static GtkWidget *btn_manual_dns;
//static GtkWidget *radio1;
//static GtkWidget *radio2;


void Msg2Dailog(char * sMsg)
{
    Msgdialog = gtk_message_dialog_new (NULL,
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_WARNING,
                                     GTK_BUTTONS_YES_NO,
                                     "%s",
                                     sMsg);
    int nRet = gtk_dialog_run (GTK_DIALOG (Msgdialog));
    switch (nRet)
    {
      case GTK_RESPONSE_YES:
         gtk_main_quit();
#ifdef ARM
		 system("sudo echo 1 > /sys/class/graphics/fb0/blank");
#endif
         system("sudo reboot");
         break;
      default:
         break;
    }
    gtk_widget_destroy(Msgdialog);
}

static pthread_t tid;
static void *SetNetthrd(void *arg)
{
    SaveSysNetwork();
}

static void net_static_data_clean()
{
	GObject *entry_netip;
	GObject *entry_netmask;
	GObject *entry_netgatway;
	entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
	entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
	entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");
	gtk_entry_set_text(GTK_ENTRY(entry_netip), "");
	gtk_entry_set_text(GTK_ENTRY(entry_netmask), "");
	gtk_entry_set_text(GTK_ENTRY(entry_netgatway), "");
}
static void dns_data_clean()
{
	GObject *entry_netdns;
	GObject *entry_netdns2;
	GObject *entry_netdns3;
	entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
	entry_netdns2 = gtk_builder_get_object (g_builder, "entry_netdns2");
	entry_netdns3 = gtk_builder_get_object (g_builder, "entry_netdns3");
	gtk_entry_set_text(GTK_ENTRY(entry_netdns), "");
	gtk_entry_set_text(GTK_ENTRY(entry_netdns2), "");
	gtk_entry_set_text(GTK_ENTRY(entry_netdns3), "");
}

static void SetDnsSensitive(unsigned short value)
{
	GObject *entry_netdns;
	GObject *entry_netdns2;
	GObject *entry_netdns3;
	GObject *label_netdns;
	GObject *label_netdns2;
	GObject *label_netdns3;
	entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
	entry_netdns2 = gtk_builder_get_object (g_builder, "entry_netdns2");
	entry_netdns3 = gtk_builder_get_object (g_builder, "entry_netdns3");
	//
	label_netdns = gtk_builder_get_object (g_builder, "label_netdns");
	label_netdns2 = gtk_builder_get_object (g_builder, "label_netdns2");
	label_netdns3 = gtk_builder_get_object (g_builder, "label_netdns3");
	///
	gtk_widget_set_sensitive(GTK_WIDGET(entry_netdns), value);
	gtk_widget_set_sensitive(GTK_WIDGET(entry_netdns2), value);
	gtk_widget_set_sensitive(GTK_WIDGET(entry_netdns3), value);
	gtk_widget_set_sensitive(GTK_WIDGET(label_netdns), value);
	gtk_widget_set_sensitive(GTK_WIDGET(label_netdns2), value);
	gtk_widget_set_sensitive(GTK_WIDGET(label_netdns3), value);
}

static void netstaitc_entry_color(char *color)
{
	 GtkCssProvider *provider;
	 GdkDisplay *display;
	 GdkScreen *screen;
	 provider = gtk_css_provider_new ();
	 display = gdk_display_get_default ();
	 screen = gdk_display_get_default_screen (display);
	 gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	 gsize bytes_written, bytes_read;
	 GError *error = 0;
	 char szTmp[MAX_BUFF_SIZE] = {0};
	 sprintf(szTmp, "#entry_netip, #entry_netmask, #entry_netgatway{\n"
                                         "background-color: %s;\n"
                                         "}\n", color);
	 gtk_css_provider_load_from_data (provider, szTmp, -1, NULL);
	 g_object_unref (provider);
}

static void dns_entry_color(char *color)
{
	 GtkCssProvider *provider;
	 GdkDisplay *display;
	 GdkScreen *screen;
	 provider = gtk_css_provider_new ();
	 display = gdk_display_get_default ();
	 screen = gdk_display_get_default_screen (display);
	 gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	 gsize bytes_written, bytes_read;
	 GError *error = 0;
	 char szTmp[MAX_BUFF_SIZE] = {0};
	 sprintf(szTmp, "#entry_netdns, #entry_netdns2, #entry_netdns3{\n"
                                         "background-color: %s;\n"
                                         "}\n", color);
	 gtk_css_provider_load_from_data (provider, szTmp, -1, NULL);
	 g_object_unref (provider);
}

GObject * GetNetWorkSetLayout()
{
     return gtk_builder_get_object(g_builder, "layout_network");
}

//还回值 ， 1： 找到   0: 没找到
int findstr(char * szfile, char * str)
{
    system("sudo cat /etc/network/interfaces > net_tmp.txt");
    FILE* fp=fopen("net_tmp.txt", "r");
    if (fp == NULL)
       return -1;
    char buf[MAX_BUFF_SIZE] = {0};
    while(fgets(buf, MAX_BUFF_SIZE, fp) != NULL)
    {
        char * p = strstr(buf, str);//p为dhcp的出现位置,NULL则为没找到
        if (p)
        {
            LogInfo("Debug: Network findstr find.\n");
            fclose(fp);
            return 1; //找到， 网络为dhcp
        }
    }
    fclose(fp);
    system("sudo rm -rf net_tmp.txt");
    return 0;
}

int findstr2(char * szfile, char * str, int nflag)
{
	if (nflag == 0)
    		system("sudo cat /etc/resolv.conf > net_tmp1.txt");
    FILE* fp=fopen("net_tmp1.txt", "r");
    if (fp == NULL)
       return -1;
    char buf[MAX_BUFF_SIZE] = {0};
    while(fgets(buf, MAX_BUFF_SIZE, fp) != NULL)
    {
        char * p = strstr(buf, str);//p为dhcp的出现位置,NULL则为没找到
        if (p)
        {
            LogInfo("Debug: Network findstr find.\n");
            fclose(fp);
            return 1; //找到， 网络为dhcp
        }
    }
    fclose(fp);
    system("sudo rm -rf net_tmp1.txt");
    return 0;
}

int GetStaticNet()
{
    int nRet = system("sudo ./netget.sh");
    if (nRet < 0)
        return -1;

    FILE* fp=fopen("nettmp.txt", "r");
    if (fp == NULL)
       return -1;

    memset(g_NetStaticInfo.szIP, 0, MAX_BUFF_SIZE);
    memset(g_NetStaticInfo.szNetMask, 0, MAX_BUFF_SIZE);
    memset(g_NetStaticInfo.szGatWay, 0, MAX_BUFF_SIZE);

    char szTmp[MAX_BUFF_SIZE] = {0};
    fgets(szTmp, MAX_BUFF_SIZE, fp);
    szTmp[strlen(szTmp) - 1] = '\0';
    strcpy(g_NetStaticInfo.szIP, rtspace(szTmp));
    memset(szTmp, 0, MAX_BUFF_SIZE);
    fgets(szTmp, MAX_BUFF_SIZE, fp);
    szTmp[strlen(szTmp) - 1] = '\0';
    strcpy(g_NetStaticInfo.szNetMask, rtspace(szTmp));
    memset(szTmp, 0, MAX_BUFF_SIZE);
    fgets(szTmp, MAX_BUFF_SIZE, fp);
    szTmp[strlen(szTmp) - 1] = '\0';
    strcpy(g_NetStaticInfo.szGatWay, rtspace(szTmp));
    memset(szTmp, 0, MAX_BUFF_SIZE);
    LogInfo("Debug: Network info Get static net ip :%s.\n", g_NetStaticInfo.szIP);
    LogInfo("Debug: Network info Get static net mask :%s.\n", g_NetStaticInfo.szNetMask);
    LogInfo("Debug: Network info Get static net gatway :%s.\n", g_NetStaticInfo.szGatWay);
    //LogInfo("Debug: Network info Get static net dns :%s.\n", g_NetStaticInfo.szDns);
    fclose(fp);
    system("rm -rf nettmp.txt");
    return 0;
}

int isDhcp()
{
    return findstr("net_tmp.txt", "dhcp");
}

int isStatic()
{
    return findstr("net_tmp.txt", "static");
}

int isDynamic_dns()
{
	return findstr2("net_tmp.txt", "#", 0) && findstr2("net_tmp.txt", "Dynamic", 0);
}
int SetDhcpNet()
{
	char szcmd[512] = {0};
	FILE *fp;
#ifdef  ARM
	sprintf(szcmd, "sudo ./ar_dhcp.sh");
	if((fp = popen(szcmd, "r")) == NULL) {
	     perror("Arm  SetDhcpNet file open failed.");
	}
	pclose(fp);
#else
    //system("sudo ./dhcp.sh");
    sprintf(szcmd, "sudo ./dhcp.sh");
	if((fp = popen(szcmd, "r")) == NULL) {
		     perror("SetDhcpNet file open failed.");
	}
	pclose(fp);
#endif
    return 0;
}

int SetDns()
{
    if (g_auto_get_ns == 1)
    {
		system("sudo ifdown eth0 \nifup eth0");
		if (access("/etc/resolvconf/resolv.conf.d/tail", F_OK) != -1)
		{
			system("sudo rm -rf /etc/resolvconf/resolv.conf.d/tail");
		}
	}else
	{
	    if (strcmp(g_NetStaticInfo.szDns[0], "") >= 0 ||
	        strcmp(g_NetStaticInfo.szDns[1], "") >= 0 ||
	        strcmp(g_NetStaticInfo.szDns[2], "") >= 0)
	    {
	        char szTmp[MAX_BUFF_SIZE] = {0};
	        sprintf(szTmp, "sudo ./dns.sh %s %s %s", g_NetStaticInfo.szDns[0], g_NetStaticInfo.szDns[1], g_NetStaticInfo.szDns[2]);
	        LogInfo("Debug: set net dns cmd ---- %s .\n", szTmp);
	        system(szTmp);
	    }
	}
    return 0;
}

int GetDns22()
{
    LogInfo("Debug: get net dns, open get dns value  000000.\n");
    FILE* fp = fopen("/etc/resolvconf/resolv.conf.d/tail", "r");
    LogInfo("Debug: get net dns, open get dns value 44444.\n");
    if (fp == NULL)
    {
       LogInfo("Debug: get net dns, open /etc/resolvconf/resolv.conf.d/tail failed.\n");
       return -1;
    }
    char *delim = " ";
    char szTmp[MAX_BUFF_SIZE] = {0};
    int nCount = 0;
    while (fgets(szTmp, MAX_BUFF_SIZE, fp) != NULL) {
        LogInfo("Debug: get net dns, open get dns value  111111,  fgets string = %s, len = %d.\n", szTmp, strlen(szTmp));
        if (strcmp(szTmp, "") != 0)
        {
          LogInfo("Debug: get net dns, open get dns value  222222.\n");
          if (strlen(szTmp) <= 0)
              break;
          szTmp[strlen(szTmp) - 1] = '\0';
          LogInfo("Debug: get net dns, open get dns value  222222dddd.\n");
          if (strstr(szTmp, "#") == NULL)//p为dhcp的出现位置,NULL则为没找到
          {
              if (strstr(szTmp, "nameserver") != NULL && strlen(szTmp) > 12 )
              {
                if (nCount < 3)
                {
                  char *pt = strtok(szTmp, delim);
                  pt  = strtok(NULL, delim);
                  strcpy(szTmp, pt);
                  LogInfo("Debug: get net dns, open get dns value = %s.\n", szTmp);
                  strcpy(g_NetStaticInfo.szDns[nCount], rtspace(szTmp));
                  LogInfo("Debug: get net dns, netstatic info szDns = %s.\n", g_NetStaticInfo.szDns[nCount]);
                  nCount++;
                }
              }
          }
        }
    }
    fclose(fp);
    return 0;
}


int GetDns()
{
    LogInfo("Debug: get net dns, open get dns value  000000.\n");
    FILE* fp = fopen("/etc/resolv.conf", "r");
    LogInfo("Debug: get net dns, open get dns value 44444.\n");
    if (fp == NULL)
    {
       LogInfo("Debug: get net dns, open /etc/resolv.conf failed.\n");
       return -1;
    }
    char *delim = " ";
    char szTmp[MAX_BUFF_SIZE] = {0};
    int nCount = 0;
    while (fgets(szTmp, MAX_BUFF_SIZE, fp) != NULL) {
        LogInfo("Debug: get net dns, open get dns value  111111,  fgets string = %s, len = %d.\n", szTmp, strlen(szTmp));
        if (strcmp(szTmp, "") != 0)
        {
          LogInfo("Debug: get net dns, open get dns value  222222.\n");
          if (strlen(szTmp) <= 0)
              break;
          szTmp[strlen(szTmp) - 1] = '\0';
          LogInfo("Debug: get net dns, open get dns value  222222dddd.\n");
          if (strstr(szTmp, "#") == NULL)//p为dhcp的出现位置,NULL则为没找到
          {
              if (strstr(szTmp, "nameserver") != NULL && strlen(szTmp) > 12 )
              {
                if (nCount < 3)
                {
                  char *pt = strtok(szTmp, delim);
                  pt  = strtok(NULL, delim);
                  strcpy(szTmp, pt);
                  LogInfo("Debug: get net dns, open get dns value = %s.\n", szTmp);
                  strcpy(g_NetStaticInfo.szDns[nCount], rtspace(szTmp));
                  LogInfo("Debug: get net dns, netstatic info szDns = %s.\n", g_NetStaticInfo.szDns[nCount]);
                  nCount++;
                }
              }
          }
        }
    }
    fclose(fp);
    return 0;
}

int SetStaticNet()
{
    char szCmd[MAX_BUFF_SIZE] = {0};
//	if (!check_ipv4_valid(g_NetStaticInfo.szIP))
//	{
//	   SYMsgDialog(4, "IP 格式不正确,请重新输入！");
//	   return 0;
//	}
//	if (!check_ipv4_valid(g_NetStaticInfo.szNetMask))
//	{
//	   SYMsgDialog(4, "子网掩码格式不正确,请重新输入！");
//	   return 0;
//	}
//	if (!check_ipv4_valid(g_NetStaticInfo.szGatWay))
//	{
//	   SYMsgDialog(4, "网关格式不正确,请重新输入！");
//	   return 0;
//	}
    sprintf(szCmd, "sudo ./netstatic.sh %s %s %s", g_NetStaticInfo.szIP, g_NetStaticInfo.szNetMask, g_NetStaticInfo.szGatWay);
    LogInfo("Debug: set static net ---- %s .\n", szCmd);
    system(szCmd);
    return 0;
}

void initStaticNetctrl()
{
    GObject *entry_netip;
    GObject *entry_netmask;
    GObject *entry_netgatway;
    GObject *entry_netdns;
    GObject *entry_netdns2;
    GObject *entry_netdns3;

    GObject *rabtn_static;  //radio button 静态IP
    GObject *rabtn_dhcp;    //dhcp ip
    rabtn_static = gtk_builder_get_object (g_builder, "rabtn_static");
    rabtn_dhcp = gtk_builder_get_object (g_builder, "rabtn_dhcp");
    if (isDhcp() == 1)
    {
		gtk_toggle_button_set_active(rabtn_static, FALSE);
		gtk_toggle_button_set_active(btn_auto_dns, TRUE);
		SetDnsSensitive(0);
		netstaitc_entry_color("#EDF0F2");
		LogInfo("Debug: initStaticNetctrl isDhcp .\n");
		g_bnetstatic = 0;
		
    }

    if (isStatic() == 1)
    {
        gtk_toggle_button_set_active(rabtn_static, TRUE);
	    gtk_toggle_button_set_active(btn_auto_dns, FALSE);
        entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
        entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
        entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");

        gtk_entry_set_text(GTK_ENTRY(entry_netip), g_NetStaticInfo.szIP);
        gtk_entry_set_text(GTK_ENTRY(entry_netmask), g_NetStaticInfo.szNetMask);
        gtk_entry_set_text(GTK_ENTRY(entry_netgatway), g_NetStaticInfo.szGatWay);
		netstaitc_entry_color("#ffffff");
        LogInfo("Debug: initStaticNetctrl isStatic .\n");
		g_bnetstatic = 1;
    }
	if (access("/etc/resolvconf/resolv.conf.d/tail", F_OK) != -1)
	{
		gtk_toggle_button_set_active(btn_manual_dns, TRUE);
		GetDns22();
	    LogInfo("initStaticNetctrl  get dns, init control , dns1=%s, dns2=%s, dns3=%s.\n", g_NetStaticInfo.szDns[0], g_NetStaticInfo.szDns[1],g_NetStaticInfo.szDns[2]);
	    entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
	    gtk_entry_set_text(GTK_ENTRY(entry_netdns), g_NetStaticInfo.szDns[0]);
	    entry_netdns2 = gtk_builder_get_object (g_builder, "entry_netdns2");
	    gtk_entry_set_text(GTK_ENTRY(entry_netdns2), g_NetStaticInfo.szDns[1]);
	    entry_netdns3 = gtk_builder_get_object (g_builder, "entry_netdns3");
	    gtk_entry_set_text(GTK_ENTRY(entry_netdns3), g_NetStaticInfo.szDns[2]);
	}else
	{
		gtk_toggle_button_set_active(btn_auto_dns, TRUE);
	}
}

void SaveSysNetwork()
{
    if (g_bnetstatic == 1)
    {
        SetStaticNet();
    }
    else
    {
        LogInfo("Debug: save set dhcp net 11 .\n");
        SetDhcpNet();
    }
    SetDns();
    LogInfo("网络配置完成！.\n");
}

void savenetwork_button_clicked(GtkButton *button,  gpointer user_data)
{
    if (g_bnetstatic == 1)
    {
        GetNetStaticInfo();
//        if (strcmp(g_NetStaticInfo.szIP, "") == 0 && strcmp(g_NetStaticInfo.szNetMask, "") == 0 &&
//            strcmp(g_NetStaticInfo.szGatWay, "") == 0)
//            return ;
		if (strcmp(g_NetStaticInfo.szIP, "") == 0)
		{
		    SYMsgDialog2(4, "IP地址不能为空！");
		    return;
		}
		if (!check_ipv4_valid(g_NetStaticInfo.szIP))
        {
           SYMsgDialog2(4, "IP 格式不正确,请重新输入！");
           return;
        }
		if (strcmp(g_NetStaticInfo.szNetMask, "") == 0)
		{
		    SYMsgDialog2(4, "子网掩码不能为空！");
		    return;
		}

        if (!check_ipv4_valid(g_NetStaticInfo.szNetMask))
        {
           SYMsgDialog2(4, "子网掩码格式不正确,请重新输入！");
           return;
        }

        if (strcmp(g_NetStaticInfo.szGatWay, "") == 0)
    		{
    		    SYMsgDialog2(4, "网关地址不能为空！");
    		    return;
    		}
        if (!check_ipv4_valid(g_NetStaticInfo.szGatWay))
        {
           SYMsgDialog2(4, "网关格式不正确,请重新输入！");
           return;
        }
    }
    GObject *entry_netdns;
    GObject *entry_netdns2;
    GObject *entry_netdns3;
    entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
    strcpy(g_NetStaticInfo.szDns[0], gtk_entry_get_text(GTK_ENTRY(entry_netdns)));
    entry_netdns2 = gtk_builder_get_object (g_builder, "entry_netdns2");
    strcpy(g_NetStaticInfo.szDns[1], gtk_entry_get_text(GTK_ENTRY(entry_netdns2)));
    entry_netdns3 = gtk_builder_get_object (g_builder, "entry_netdns3");
    strcpy(g_NetStaticInfo.szDns[2], gtk_entry_get_text(GTK_ENTRY(entry_netdns3)));
    if ( pthread_create(&tid, NULL, SetNetthrd, NULL) !=0 )
     {
         printf("Create SetNetthrd error!\n");
     }
     ShowMsgWindow();
     pthread_join(tid, NULL);
	 close_setting_window();
     LogInfo("Debug: savenetwork_button_clicked end.\n");
}

void GetNetStaticInfo()
{
    GObject *entry_netip;
    GObject *entry_netmask;
    GObject *entry_netgatway;
    //GObject *entry_netdns;

    entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
    entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
    entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");
    //entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
    strcpy(g_NetStaticInfo.szIP, gtk_entry_get_text(GTK_ENTRY(entry_netip)));
    strcpy(g_NetStaticInfo.szNetMask, gtk_entry_get_text(GTK_ENTRY(entry_netmask)));
    strcpy(g_NetStaticInfo.szGatWay, gtk_entry_get_text(GTK_ENTRY(entry_netgatway)));
    //strcpy(g_NetStaticInfo.szDns, gtk_entry_get_text(GTK_ENTRY(entry_netdns)));
}

void SetNetCtrlSensitive(int value)
{
    GObject *label_netip;
    GObject *label_netmask;
    GObject *label_netgatway;
    //GObject *label_netdns;

    GObject *entry_netip;
    GObject *entry_netmask;
    GObject *entry_netgatway;
    //GObject *entry_netdns;
	if (NULL == g_builder)
		return;
    label_netip = gtk_builder_get_object (g_builder, "label_netip");
    label_netmask = gtk_builder_get_object (g_builder, "label_netmask");
    label_netgatway = gtk_builder_get_object (g_builder, "label_netgatway");
    //label_netdns = gtk_builder_get_object (g_builder, "label_netdns");
    gtk_widget_set_sensitive(GTK_WIDGET(label_netip), value);
    gtk_widget_set_sensitive(GTK_WIDGET(label_netmask), value);
    gtk_widget_set_sensitive(GTK_WIDGET(label_netgatway), value);
    //gtk_widget_set_sensitive(GTK_WIDGET(label_netdns), value);
    entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
    entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
    entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");
    //entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
    gtk_widget_set_sensitive(GTK_WIDGET(entry_netip), value);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_netmask), value);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_netgatway), value);
    //gtk_widget_set_sensitive(GTK_WIDGET(entry_netdns), value);
}

void rabtn_static_button_callback(GtkWidget *check_button, gpointer data)
{
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button)))
    {
        printf("check button selected\n");
        //到这里，是选这了Radio 静态IP地址
		SetNetCtrlSensitive(1);
		netstaitc_entry_color("#ffffff");
		dns_entry_color("#ffffff");
		gtk_toggle_button_set_active(btn_manual_dns, TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(btn_auto_dns), FALSE);
		g_bnetstatic = 1;
    }
    else
    {
        printf("check button unselected\n");
		SetNetCtrlSensitive(0);
		netstaitc_entry_color("#EDF0F2");
		gtk_widget_set_sensitive(GTK_WIDGET(btn_auto_dns), TRUE);
		net_static_data_clean();
		g_bnetstatic = 0;
    }
}

void selnet_static_button_callback(GtkWidget *check_button, gpointer data)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button)))
    {
        printf("check button selnetwork selected\n");
	   SetDnsSensitive(0);
	   dns_entry_color("#EDF0F2");
	   g_auto_get_ns = 1;
	   dns_data_clean();
    }
    else
    {
        printf("check button selnetwork unselected\n");
	   SetDnsSensitive(1);
	   dns_entry_color("#ffffff");
	   g_auto_get_ns = 0;
    }
}


static gboolean checkbutton_wlan_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        g_checkrepass = !g_checkrepass;
        if (g_checkrepass == 1)
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_checkPressimage);
        }
        else
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_checkNorimage);
        }
    }
    return TRUE;
}

extern void SY_NetworkDisableCtrl();
void SY_NetworkDisableCtrl()
{
   GObject *btn_advanced;
   btn_advanced = gtk_builder_get_object (g_builder, "btn_advanced");
   gtk_widget_set_sensitive(GTK_WIDGET(btn_advanced), 0);
   gtk_widget_set_visible(GTK_WIDGET(btn_advanced), 0);
}

void init_network_pos(GtkBuilder *g_builder)
{
    int win_width = 0;
	int win_height = 0;
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	GtkWidget * widget = GetNetWorkSetLayout();
	if ((scr_width == 1024 && scr_height == 768)  || (scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   ||
		(scr_width == 1600 && scr_height == 1024) )
	{
		win_width = 500;
		win_height = 470 + 60;
		gtk_widget_set_size_request(GTK_WIDGET(widget), win_width, win_height);
	}else if ( (scr_width == 1280 && scr_height == 720)  ||
	    (scr_width == 1280 && scr_height == 768) ||
	     (scr_width == 1280 && scr_height == 1024) ||
	    (scr_width == 1366 && scr_height == 768) || ( scr_width == 1368 && scr_height == 768 ) ||
	    ( scr_width == 1360 && scr_height == 768 ) )
	{
		win_width = 500;
		win_height = 470 + 60;
		gtk_widget_set_size_request(GTK_WIDGET(widget), win_width, win_height);
	}
	GObject *label_wan;
    GObject *label_hotspot;  //无线热点
    GObject *label_netinter;
    GObject *label_netip;
    GObject *label_netmask;
    GObject *label_netgatway;
    GObject *label_netdns;
    GObject *label_netdns2;
    GObject *label_netdns3;

    GObject *btn_connect;
    GObject *rabtn_static;  //radio button 静态IP
    GObject *rabtn_dhcp;    //dhcp ip
    GObject *btn_advanced;
    GObject *btn_savenetwork;
	GObject *comboboxtext_wan;

	GObject *entry_netip;
	GObject *entry_netmask;
	GObject *entry_netgatway;
	GObject *entry_netdns;
	GObject *entry_netdns2;
	GObject *entry_netdns3;

	GObject *separator1;
	GObject *eventbox_wlan;


    label_wan = gtk_builder_get_object (g_builder, "label_wan");
    label_hotspot = gtk_builder_get_object (g_builder, "label_hotspot");
    label_netinter = gtk_builder_get_object (g_builder, "label_netinter");
    label_netip = gtk_builder_get_object (g_builder, "label_netip");
    label_netmask = gtk_builder_get_object (g_builder, "label_netmask");
    label_netgatway = gtk_builder_get_object (g_builder, "label_netgatway");
    label_netdns = gtk_builder_get_object (g_builder, "label_netdns");
    label_netdns2 = gtk_builder_get_object (g_builder, "label_netdns2");
    label_netdns3 = gtk_builder_get_object (g_builder, "label_netdns3");

    btn_connect = gtk_builder_get_object (g_builder, "btn_connect");
    rabtn_static = gtk_builder_get_object (g_builder, "rabtn_static");
    rabtn_dhcp = gtk_builder_get_object (g_builder, "rabtn_dhcp");
    btn_advanced = gtk_builder_get_object (g_builder, "btn_advanced");
    btn_savenetwork = gtk_builder_get_object (g_builder, "btn_savenetwork");
	comboboxtext_wan = gtk_builder_get_object (g_builder, "comboboxtext_wan");

	entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
	entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
	entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");
	entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
	entry_netdns2 = gtk_builder_get_object (g_builder, "entry_netdns2");
	entry_netdns3 = gtk_builder_get_object (g_builder, "entry_netdns3");
	separator1 = gtk_builder_get_object (g_builder, "separator1");

    gtk_label_set_text(GTK_LABEL(label_wan), "WLAN");
    gtk_label_set_text(GTK_LABEL(label_hotspot), "无线热点");
    gtk_label_set_text(GTK_LABEL(label_netinter), "网络接口");
    gtk_label_set_text(GTK_LABEL(label_netip), " IP地址");
    gtk_label_set_text(GTK_LABEL(label_netmask), "子网掩码");
    gtk_label_set_text(GTK_LABEL(label_netgatway), "默认网关");
    gtk_label_set_text(GTK_LABEL(label_netdns), "DNS1服务器");
    gtk_label_set_text(GTK_LABEL(label_netdns2), "DNS2服务器");
    gtk_label_set_text(GTK_LABEL(label_netdns3), "DNS3服务器");
    gtk_label_set_justify(GTK_LABEL(label_netip), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netmask), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netgatway), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netdns), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netdns2), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netdns3), GTK_JUSTIFY_RIGHT);

    gtk_button_set_label(GTK_BUTTON(btn_connect), "连接");
    gtk_button_set_label(GTK_BUTTON(rabtn_static), "使用静态IP地址");
    gtk_button_set_label(GTK_BUTTON(rabtn_dhcp), "使用DHCP自动获取");
    gtk_button_set_label(GTK_BUTTON(btn_advanced), "高级");
    gtk_button_set_label(GTK_BUTTON(btn_savenetwork), "保存");

    GObject * comboboxtext_netinter = gtk_builder_get_object (g_builder, "comboboxtext_netinter");
    gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxtext_netinter, 0, "eth0");
    gtk_combo_box_set_active((GtkComboBox *)comboboxtext_netinter, 0);
	g_signal_connect(G_OBJECT(rabtn_static), "toggled", G_CALLBACK(rabtn_static_button_callback), NULL); //toggled
    g_signal_connect(G_OBJECT(btn_savenetwork), "clicked", G_CALLBACK(savenetwork_button_clicked), NULL);
	int font_size = 0;
	if ((scr_width == 1024 && scr_height == 768)  || (scr_width == 1440 && scr_height == 900) ||
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050))
	{
		font_size = 9;
		setctrlFont(GTK_WIDGET(label_wan), font_size);
		setctrlFont(GTK_WIDGET(label_hotspot), font_size);
		setctrlFont(GTK_WIDGET(label_netinter ), font_size);
		setctrlFont(GTK_WIDGET(label_netip), font_size);
		setctrlFont(GTK_WIDGET(label_netmask), font_size);
		setctrlFont(GTK_WIDGET(label_netgatway), font_size);
		setctrlFont(GTK_WIDGET(label_netdns), font_size);
		setctrlFont(GTK_WIDGET(label_netdns2), font_size);
		setctrlFont(GTK_WIDGET(label_netdns3), font_size);

		setctrlFont(GTK_WIDGET(btn_connect), font_size);
		setctrlFont(GTK_WIDGET(rabtn_static), font_size);
		setctrlFont(GTK_WIDGET(rabtn_dhcp), font_size);
		setctrlFont(GTK_WIDGET(btn_advanced), font_size);
		setctrlFont(GTK_WIDGET(btn_savenetwork), font_size);
		setctrlFont(GTK_WIDGET(comboboxtext_wan), font_size);
		setctrlFont(GTK_WIDGET(comboboxtext_netinter), font_size);

		setctrlFont(GTK_WIDGET(entry_netip), font_size);
		setctrlFont(GTK_WIDGET(entry_netmask), font_size);
		setctrlFont(GTK_WIDGET(entry_netgatway), font_size);
		setctrlFont(GTK_WIDGET(entry_netdns), font_size);
		setctrlFont(GTK_WIDGET(entry_netdns2), font_size);
		setctrlFont(GTK_WIDGET(entry_netdns3), font_size);

		//set pos
		int left = 20;
		int top = 10;
		int delay_width = 3;
		int delay_height = 10;
		int x, y = 0;
		eventbox_wlan = gtk_builder_get_object (g_builder, "eventbox_wlan");
		//set size
		int label_width = 0;
		int label_height = 0;
		label_width = 60;
		label_height = 20;
		gtk_widget_set_size_request(GTK_WIDGET(label_wan), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_hotspot), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netinter), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netip), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netmask), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netgatway), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netdns), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netdns2), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netdns3), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_wan), 80, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_netinter), 80, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(btn_connect), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(rabtn_dhcp), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(rabtn_static), label_width, label_height);
		//entry
		gtk_widget_set_size_request(GTK_WIDGET(entry_netip), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netmask), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netgatway), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netdns), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netdns2), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netdns3), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(btn_savenetwork), label_width, label_height);
		x = left;
		y = top;
		int image_width = 8;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_wlan), x, y);
		x = left + image_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_wan), x, y);
		x = left + image_width + delay_width + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_hotspot), x, y);
		x = left + image_width + delay_width + label_width + delay_width + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_wan), x, y);
		x = left + image_width + delay_width + label_width + delay_width + label_width + delay_width + 80 + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_connect), x, y);
		//netinter
		x = left;
		//y = top + label_height + delay_height;
		y = top;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netinter), x, y);
		x = left + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_netinter), x, y);
		//dhcp
		x = left;
		//y = top + label_height + delay_height + label_height + delay_height;
		y = top;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(rabtn_dhcp), x, y);
		x = left;
		//y = top + label_height + delay_height + label_height + delay_height + label_height + delay_height/2;
		y = top + label_height + delay_height/2;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(rabtn_static), x, y);
		//address
		//int tmp_top = top + label_height + delay_height + label_height + delay_height + label_height + delay_height;
		int tmp_top = top + label_height + label_height + delay_height;
		x = left + 20;
		delay_height = 15;
		y = tmp_top + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netip), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netip), x, y);
		x = left + 20;
		y = tmp_top + delay_height + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netmask), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netmask), x, y);
		x = left + 20;
		y = tmp_top + delay_height + label_height + delay_height + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netgatway), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netgatway), x, y);
		x = left + 20;
		y = tmp_top + delay_height + label_height + delay_height + label_height + delay_height + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(separator1), x, y);
		//dns
		x = left + 20;
		label_width = 70;
		tmp_top = y;
		y = tmp_top + delay_height;
		init_dnssel_pos(widget, left, y);
		y = y + 60;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns), x, y);
		x = left + 20;
		y = y + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns2), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns2), x, y);
		x = left + 20;
		y = y + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns3), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns3), x, y);
		//btn_savenetwork
		delay_width = 10;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_savenetwork), x + delay_width + label_width*3, y + 10);
	}
	else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
//		font_size = 11;
//		setctrlFont(GTK_WIDGET(label_wan), font_size);
//		setctrlFont(GTK_WIDGET(label_hotspot), font_size);
//		setctrlFont(GTK_WIDGET(label_netinter ), font_size);
//		setctrlFont(GTK_WIDGET(label_netip), font_size);
//		setctrlFont(GTK_WIDGET(label_netmask), font_size);
//		setctrlFont(GTK_WIDGET(label_netgatway), font_size);
//		setctrlFont(GTK_WIDGET(label_netdns), font_size);
//		setctrlFont(GTK_WIDGET(label_netdns2), font_size);
//		setctrlFont(GTK_WIDGET(label_netdns3), font_size);

//		setctrlFont(GTK_WIDGET(btn_connect), font_size);
//		setctrlFont(GTK_WIDGET(rabtn_static), font_size);
//		setctrlFont(GTK_WIDGET(rabtn_dhcp), font_size);
//		setctrlFont(GTK_WIDGET(btn_advanced), font_size);
//		setctrlFont(GTK_WIDGET(btn_savenetwork), font_size);
//		setctrlFont(GTK_WIDGET(comboboxtext_wan), font_size);
//		setctrlFont(GTK_WIDGET(comboboxtext_netinter), font_size);

//		setctrlFont(GTK_WIDGET(entry_netip), font_size);
//		setctrlFont(GTK_WIDGET(entry_netmask), font_size);
//		setctrlFont(GTK_WIDGET(entry_netgatway), font_size);
//		setctrlFont(GTK_WIDGET(entry_netdns), font_size);
//		setctrlFont(GTK_WIDGET(entry_netdns2), font_size);
//		setctrlFont(GTK_WIDGET(entry_netdns3), font_size);

//		//set pos
//		int left = 20;
//		int top = 10;
//		int delay_width = 3;
//		int delay_height = 10;
//		int x, y = 0;
//		eventbox_wlan = gtk_builder_get_object (g_builder, "eventbox_wlan");
//		//set size
//		int label_width = 0;
//		int label_height = 0;
//		label_width = 60;
//		label_height = 20;
//		gtk_widget_set_size_request(GTK_WIDGET(label_wan), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(label_hotspot), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(label_netinter), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(label_netip), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(label_netmask), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(label_netgatway), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(label_netdns), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(label_netdns2), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(label_netdns3), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_wan), 80, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_netinter), 80, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(btn_connect), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(rabtn_dhcp), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(rabtn_static), label_width, label_height);
//		//entry
//		gtk_widget_set_size_request(GTK_WIDGET(entry_netip), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(entry_netmask), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(entry_netgatway), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(entry_netdns), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(entry_netdns2), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(entry_netdns3), label_width, label_height);
//		gtk_widget_set_size_request(GTK_WIDGET(btn_savenetwork), label_width, label_height);
//		x = left;
//		y = top;
//		int image_width = 8;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_wlan), x, y);
//		x = left + image_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_wan), x, y);
//		x = left + image_width + delay_width + label_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_hotspot), x, y);
//		x = left + image_width + delay_width + label_width + delay_width + label_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_wan), x, y);
//		x = left + image_width + delay_width + label_width + delay_width + label_width + delay_width + 80 + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_connect), x, y);
//		//netinter
//		x = left;
//		//y = top + label_height + delay_height;
//		y = top;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netinter), x, y);
//		x = left + label_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_netinter), x, y);
//		//dhcp
//		x = left;
//		//y = top + label_height + delay_height + label_height + delay_height;
//		y = top;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(rabtn_dhcp), x, y);
//		x = left;
//		//y = top + label_height + delay_height + label_height + delay_height + label_height + delay_height/2;
//		y = top + label_height + delay_height/2;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(rabtn_static), x, y);
//		//address
//		//int tmp_top = top + label_height + delay_height + label_height + delay_height + label_height + delay_height;
//		int tmp_top = top + label_height + label_height + delay_height;
//		x = left + 20;
//		delay_height = 15;
//		y = tmp_top + delay_height;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netip), x, y);
//		x = left + 20 + label_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netip), x, y);
//		x = left + 20;
//		y = tmp_top + delay_height + label_height + delay_height;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netmask), x, y);
//		x = left + 20 + label_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netmask), x, y);
//		x = left + 20;
//		y = tmp_top + delay_height + label_height + delay_height + label_height + delay_height;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netgatway), x, y);
//		x = left + 20 + label_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netgatway), x, y);
//		x = left + 20;
//		y = tmp_top + delay_height + label_height + delay_height + label_height + delay_height + label_height + delay_height;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(separator1), x, y);
//		//dns
//		x = left + 20;
//		label_width = 70;
//		tmp_top = y;
//		y = tmp_top + delay_height;
//		init_dnssel_pos(widget, left, y);
//		y = y + 60;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns), x, y);
//		x = left + 20 + label_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns), x, y);
//		x = left + 20;
//		y = y + label_height + delay_height;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns2), x, y);
//		x = left + 20 + label_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns2), x, y);
//		x = left + 20;
//		y = y + label_height + delay_height;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns3), x, y);
//		x = left + 20 + label_width + delay_width;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns3), x, y);
//		//btn_savenetwork
//		delay_width = 10;
//		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_savenetwork), x + delay_width + label_width*3, y + 10);


		//170609 add
		////dhcp
		int label_width = 0;
		int label_height = 0;
		label_width = 100;
		label_height = 20;
		int delay_height = 15;
		int delay_width = 20;
		int left = 83;
		int x, y = 0;
		x = left;
		int top = 20;
		y = top;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(rabtn_dhcp), x, y);
		x = left;
		y = top + label_height + delay_height/2;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(rabtn_static), x, y);

		int tmp_top = y;
		int net_x = x + 25;
		y = tmp_top + delay_height + label_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netip), net_x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netip), x, y);
		x = left + 20;
		y = tmp_top + delay_height + label_height + delay_height + label_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netmask), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netmask), x, y);
		x = left + 20;
		y = tmp_top + delay_height + label_height + delay_height + label_height + delay_height + label_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netgatway), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netgatway), x, y);
		x = left + 20;
		y = tmp_top + delay_height + label_height + delay_height + label_height + delay_height + label_height + delay_height + label_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(separator1), x, y);
		//end
		//set radio button pos
		top = tmp_top + delay_height + label_height + delay_height + label_height + delay_height + label_height + delay_height + label_height;
		x = 0;
		y = 0;
		delay_width = 10;
		delay_height = 10;
		x = left;
		y = top + delay_height;
		init_dnssel_pos(widget, x, y);
		tmp_top = y;
		///set dns pos
		left = 129;
		top = tmp_top;
		x = 0;
		y = 0;
		delay_width = 10;
		delay_height = 10;
		y = top + 60;
		x = left;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns), x, y);
		x += 100;
		x += delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns), x, y);
		y += 30;
		y += delay_height;
		x = left;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns2), x, y);
		x += 100;
		x += delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns2), x, y);
		y += 30;
		y += delay_height;
		x = left;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns3), x, y);
		x += 100;
		x += delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns3), x, y);

	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768)||
	    (scr_width == 1280 && scr_height == 1024) ||
	    (scr_width == 1366 && scr_height == 768) ||
	    (scr_width == 1368 && scr_height == 768) ||
	    (scr_width == 1360 && scr_height == 768))
	{
		font_size = 9;
		setctrlFont(GTK_WIDGET(label_wan), font_size);
		setctrlFont(GTK_WIDGET(label_hotspot), font_size);
		setctrlFont(GTK_WIDGET(label_netinter ), font_size);
		setctrlFont(GTK_WIDGET(label_netip), font_size);
		setctrlFont(GTK_WIDGET(label_netmask), font_size);
		setctrlFont(GTK_WIDGET(label_netgatway), font_size);
		setctrlFont(GTK_WIDGET(label_netdns), font_size);
		setctrlFont(GTK_WIDGET(label_netdns2), font_size);
		setctrlFont(GTK_WIDGET(label_netdns3), font_size);

		setctrlFont(GTK_WIDGET(btn_connect), font_size);
		setctrlFont(GTK_WIDGET(rabtn_static), font_size);
		setctrlFont(GTK_WIDGET(rabtn_dhcp), font_size);
		setctrlFont(GTK_WIDGET(btn_advanced), font_size);
		setctrlFont(GTK_WIDGET(btn_savenetwork), font_size);
		setctrlFont(GTK_WIDGET(comboboxtext_wan), font_size);
		setctrlFont(GTK_WIDGET(comboboxtext_netinter), font_size);

		setctrlFont(GTK_WIDGET(entry_netip), font_size);
		setctrlFont(GTK_WIDGET(entry_netmask), font_size);
		setctrlFont(GTK_WIDGET(entry_netgatway), font_size);
		setctrlFont(GTK_WIDGET(entry_netdns), font_size);
		setctrlFont(GTK_WIDGET(entry_netdns2), font_size);
		setctrlFont(GTK_WIDGET(entry_netdns3), font_size);

		//set pos
		int left = 20;
		int top = 10;
		int delay_width = 3;
		int delay_height = 10;
		int x, y = 0;
		eventbox_wlan = gtk_builder_get_object (g_builder, "eventbox_wlan");
		//set size
		int label_width = 0;
		int label_height = 0;
		label_width = 60;
		label_height = 20;
		gtk_widget_set_size_request(GTK_WIDGET(label_wan), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_hotspot), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netinter), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netip), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netmask), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netgatway), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netdns), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netdns2), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(label_netdns3), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_wan), 80, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_netinter), 80, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(btn_connect), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(rabtn_dhcp), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(rabtn_static), label_width, label_height);
		//entry
		gtk_widget_set_size_request(GTK_WIDGET(entry_netip), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netmask), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netgatway), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netdns), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netdns2), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(entry_netdns3), label_width, label_height);
		gtk_widget_set_size_request(GTK_WIDGET(btn_savenetwork), label_width, label_height);
		x = left;
		y = top;
		int image_width = 8;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_wlan), x, y);
		x = left + image_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_wan), x, y);
		x = left + image_width + delay_width + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_hotspot), x, y);
		x = left + image_width + delay_width + label_width + delay_width + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_wan), x, y);
		x = left + image_width + delay_width + label_width + delay_width + label_width + delay_width + 80 + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_connect), x, y);
		//netinter
		x = left;
		//y = top + label_height + delay_height;
		y = top;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netinter), x, y);
		x = left + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_netinter), x, y);
		//dhcp
		x = left;
		//y = top + label_height + delay_height + label_height + delay_height;
		y = top;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(rabtn_dhcp), x, y);
		x = left;
		//y = top + label_height + delay_height + label_height + delay_height + label_height + delay_height/2;
		y = top + label_height + delay_height/2;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(rabtn_static), x, y);
		//address
		//int tmp_top = top + label_height + delay_height + label_height + delay_height + label_height + delay_height;
		int tmp_top = top + label_height + label_height + delay_height;
		x = left + 20;
		delay_height = 15;
		y = tmp_top + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netip), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netip), x, y);
		x = left + 20;
		y = tmp_top + delay_height + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netmask), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netmask), x, y);
		x = left + 20;
		y = tmp_top + delay_height + label_height + delay_height + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netgatway), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netgatway), x, y);
		x = left + 20;
		y = tmp_top + delay_height + label_height + delay_height + label_height + delay_height + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(separator1), x, y);
		//dns
		x = left + 20;
		label_width = 70;
		tmp_top = y;
		y = tmp_top + delay_height;
		init_dnssel_pos(widget, left, y);
		y = y + 60;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns), x, y);
		x = left + 20;
		y = y + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns2), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns2), x, y);
		x = left + 20;
		y = y + label_height + delay_height;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_netdns3), x, y);
		x = left + 20 + label_width + delay_width;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_netdns3), x, y);
		//btn_savenetwork
		delay_width = 10;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_savenetwork), x + delay_width + label_width*3, y + 10);
	}
	eventbox_wlan = gtk_builder_get_object (g_builder, "eventbox_wlan");
	gtk_widget_set_sensitive(GTK_WIDGET(comboboxtext_wan), 0);
	gtk_widget_set_sensitive(GTK_WIDGET(comboboxtext_netinter), 0);
	gtk_widget_set_sensitive(GTK_WIDGET(btn_connect), 0);
	gtk_widget_set_sensitive(GTK_WIDGET(eventbox_wlan), 0);
}

void sethidewan()
{
	GObject *comboboxtext_wan = gtk_builder_get_object (g_builder, "comboboxtext_wan");
	GObject *btn_connect = gtk_builder_get_object (g_builder, "btn_connect");
	GObject *eventbox_wlan = gtk_builder_get_object (g_builder, "eventbox_wlan");
	GObject *label_wan = gtk_builder_get_object (g_builder, "label_wan");
	GObject *label_hotspot = gtk_builder_get_object (g_builder, "label_hotspot");
	GObject *comboboxtext_netinter = gtk_builder_get_object (g_builder, "comboboxtext_netinter");
	GObject *label_netinter = gtk_builder_get_object (g_builder, "label_netinter");
	gtk_widget_set_visible(GTK_WIDGET(comboboxtext_wan), 0);
	gtk_widget_set_visible(GTK_WIDGET(btn_connect), 0);
	gtk_widget_set_visible(GTK_WIDGET(eventbox_wlan), 0);
	gtk_widget_set_visible(GTK_WIDGET(label_wan), 0); 
	gtk_widget_set_visible(GTK_WIDGET(label_hotspot), 0);
	gtk_widget_set_visible(GTK_WIDGET(comboboxtext_netinter), 0);
	gtk_widget_set_visible(GTK_WIDGET(label_netinter), 0);
}

static void  on_entry_netip_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
{
    if (new_text[0]  > '9'  || new_text[0] < '0' )
    {
       if (new_text[0] == '.')
	   	   ;
	   else
       		new_text[0] = '\0';
   	}
    return;
}

static void  on_entry_netmask_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
{
    if (new_text[0]  > '9'  || new_text [0] < '0')
    {
        if (new_text[0] == '.')
	   	   ;
	   else
       		new_text[0] = '\0';
   	}
    return;
}

static void  on_entry_netgatway_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
{
    if (new_text[0]  > '9'  || new_text [0] < '0')
    {
        if (new_text[0] == '.')
	   	   ;
	   else
       		new_text[0] = '\0';
   	}
    return;
}


static void init_entry_event()
{
	GObject *entry_netip;
	GObject *entry_netmask;
	GObject *entry_netgatway;
	GObject *entry_netdns;
	entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
    entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
    entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");
	g_signal_connect(G_OBJECT(entry_netip), "insert-text", G_CALLBACK(on_entry_netip_insert_text), NULL);
	g_signal_connect(G_OBJECT(entry_netmask), "insert-text", G_CALLBACK(on_entry_netmask_insert_text), NULL);
	g_signal_connect(G_OBJECT(entry_netgatway), "insert-text", G_CALLBACK(on_entry_netgatway_insert_text), NULL);
}

void init_dnssel_pos(GtkWidget * widget, int x, int y)
{
	GtkWidget *box;
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
	gtk_box_set_homogeneous (GTK_BOX(box), TRUE);
	gtk_box_pack_start (GTK_BOX (box), btn_auto_dns, TRUE, TRUE, 2);
	gtk_box_pack_start (GTK_BOX (box), btn_manual_dns, TRUE, TRUE, 2);
	gtk_container_add (GTK_CONTAINER (widget), box);
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(box), x, y);
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	int font_size = 0;
	if ((scr_width == 1024 && scr_height == 768  ) ||
		(scr_width == 1440 && scr_height == 900) ||
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   ||
		(scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050))
	{
		font_size = 9;
	}else if ( (scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		font_size = 11;
	}
	else if ( (scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		font_size = 9;
	}else if ((scr_width == 1366 && scr_height == 768) || (scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		font_size = 9;
	}
	setctrlFont(GTK_WIDGET(btn_auto_dns), font_size);
	setctrlFont(GTK_WIDGET(btn_manual_dns), font_size);
	setfontcolor(GTK_WIDGET(btn_auto_dns),"#282C34");
	setfontcolor(GTK_WIDGET(btn_manual_dns),"#282C34");
	gtk_widget_set_size_request(GTK_WIDGET(box), 100, 40);
	g_signal_connect(G_OBJECT(btn_auto_dns), "toggled", G_CALLBACK(selnet_static_button_callback), NULL);
	gtk_widget_show (box);
}

int SY_NetworkSet_main()
{
    g_builder = gtk_builder_new();
    GError *error = NULL;
    gtk_builder_add_from_file (g_builder, "network.glade", &error);
	btn_auto_dns = gtk_radio_button_new_with_label (NULL, "自动获得DNS服务器地址");
	GSList *radio_group;
	radio_group = gtk_radio_button_get_group(btn_auto_dns);  
	btn_manual_dns = gtk_radio_button_new_with_label(radio_group, "手动设置DNS服务器地址");
	init_network_pos(g_builder);

    g_checkNorimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_NOR, NULL);
    g_checkPressimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_PRESS, NULL);

    GObject *eventbox_wlan;
    eventbox_wlan = gtk_builder_get_object (g_builder, "eventbox_wlan");
    gtk_widget_set_events((GtkWidget *)eventbox_wlan, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件

    GObject *image_wlan;
    image_wlan = gtk_builder_get_object (g_builder, "image_wlan");
	gtk_widget_set_sensitive(GTK_WIDGET(image_wlan), 0);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image_wlan), g_checkNorimage);
    g_signal_connect (G_OBJECT (eventbox_wlan), "button_press_event",
                    G_CALLBACK (checkbutton_wlan_press_callback), (GtkWidget *)image_wlan);
    SetNetCtrlSensitive(0);
    //gtk_widget_set_sensitive(GTK_WIDGET(btn_advanced), 0);
    //gtk_widget_set_visible(GTK_WIDGET(btn_advanced), 0);
    GetStaticNet();
    initStaticNetctrl();
	init_entry_event();
    return 0;
}
