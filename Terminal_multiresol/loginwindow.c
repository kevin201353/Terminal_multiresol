#include <math.h>
#include "global.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <pango/pango.h>
#include "tcpclient.h"
#include <unistd.h>
#include "msg.h"


#define IMAGE_MAIN_BACKGROUD          "images2/login.png"
#define IMAGE_LOGO_VM                 "images2/wm_logo.png"
#define IMAGE_LOGO_MIR                "images2/mir_logo.png"
#define IMAGE_LOGO_SH                 "images2/sh_logo.png"
#define IMAGE_LOGO_CIT                "images2/cit_logo.png"
#define IMAGE_BTN_LOGIN_NOR           "images2/btnlogin_nor.png"
#define IMAGE_BTN_LOGIN_PRES          "images2/btnlogin_press.png"
#define IMAGE_CHECKBUTTON_NOR         "images2/checkbtn_nor.png"
#define IMAGE_CHECKBUTTON_PRESS       "images2/checkbtn_press.png"
#define IMAGE_BTN_PREV_NOR    		  "images2/loginout_nor.png"
#define IMAGE_BTN_PREV_PRES           "images2/loginout_press.png"
#define IMAGE_BTN_SHUTDWON_NOR        "images2/exit_nor.png"
#define IMAGE_BTN_SHUTDWON_PRES       "images2/exit_press.png"


static const char *g_logincss = "login.css";
static int scr_top = 0;
static int scr_layout2_height = 100;
static int scr_layout4_height = 150;
static int btn_inter_right = 80;
static int btn_inter_bottom = 80;
static int btn_space_width = 30;
static int btn_width = 80;
static int btn_height = 30;

static cairo_surface_t *surface1;
static GdkPixbuf *g_loginPress;
static GdkPixbuf *g_loginNor;
static GdkPixbuf *g_checkNorimage;
static GdkPixbuf *g_checkPressimage;

static GdkPixbuf *g_setPress;
static GdkPixbuf *g_setNor;
static GdkPixbuf *g_prevPress;
static GdkPixbuf *g_prevNor;

static GdkPixbuf *g_logoVm;
static GdkPixbuf *g_logoCit;
static GdkPixbuf *g_logoMir;
static GdkPixbuf *g_logoSh;
static GdkPixbuf *g_shutdownPress;
static GdkPixbuf *g_shutdownNor;

//data define
struct LoginInfo  g_loginfo = {SY_VM_COMMON_SPICE, "admin@internal", "shencloud", "127.0.0.1", 3389, 0, 0, 0};
static unsigned short g_checkrepass; //0: on selected 1: selected
static unsigned short g_checkautologin;
static struct ServerInfo  serverInfo;
static int showloginwindow = 0;
extern int check_ipv4_valid(char *s);
static pthread_t tid;
static GObject *g_window = NULL;
static GtkBuilder *g_builder = NULL;
short g_loginExit;
extern GdkPixbuf *create_pixbuf(const gchar * filename);
extern void thrd_monitor(char *cmd);
extern void closeAutoDialog();

//data end
static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    switch(event->keyval)
    {
		case GDK_KEY_Return:
			{
				if (showSyMsgDlg11 == 0)
				{
					printf("Debug : key press enter , gdk_key_return.\n");
					SYMsgDialog(0, "�������ӣ����Ժ� ... ");
				}
			}
			break;
//		case GDK_KEY_F11:
//			{
//				printf("Debug : key press enter , GDK_KEY_F11.\n");
//				gtk_window_fullscreen(GTK_WINDOW(g_window));
//			}
//			break;
    }
    return FALSE;
}


static void  on_btn_logout_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevPress);
   g_loginExit = 1;
   showloginwindow = 0;
   gtk_widget_destroy((GtkWidget *)g_window);
   gtk_main_quit();
}

static void  on_btn_logout_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevNor);
}

static void  on_btn_logout_enter(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevPress);
}

static void  on_btn_shutdown_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shutdownNor);
   printf("on_btn_shutdown_pressed .\n");
   SYMsgDialog2(11, "您确定要关闭系统吗？");
}

static void  on_btn_shutdown_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shutdownNor);
}

static void  on_btn_shutdown_enter(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shutdownPress);
}

static gboolean gtk_main_quit_login(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
      showloginwindow = 0;
      gtk_main_quit();
      g_loginExit = 1;
      return TRUE;
}

static gboolean checkbutton_repass_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        g_checkrepass = !g_checkrepass;
        LogInfo("checkbutton repass gtkimage check: %d.\n", g_checkrepass);
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

static gboolean checkbutton_autologin_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        g_checkautologin = !g_checkautologin;
        LogInfo("checkbutton autologin gtkimage check: %d.\n", g_checkautologin);
        if (g_checkautologin == 1)
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


static void  on_btn_login_enter(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginPress);
}

static void  on_btn_login_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginPress);
   SYMsgDialog(0, "�������ӣ����Ժ� ... ");
}

static void  on_btn_login_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginNor);
}


int initOvirtUrl()
{
	memset(ovirt_url, 0, MAX_BUFF_SIZE);
	if ( GetServerInfo2(&serverInfo) < 0 )
	{
	 //first statup, don't setting
	 return -1;
	}
	char sztmp[20] = {0};
	sprintf(sztmp, "%d", serverInfo.nport);
	strcpy(ovirt_url, "https://");
	strcat(ovirt_url, serverInfo.szIP);
	strcat(ovirt_url, ":");
	strcat(ovirt_url, sztmp);
	strcat(ovirt_url, "/");
	return 0;
}

int get_ctrldata()
{
    GObject *object;
    int len = 0;
    if (g_builder == NULL)
    {
        LogInfo("debug: Get_ctrldata  g_builder == NULL.\n");
        return -1;
    }
    if (g_selectProto == 1 || g_selectProto == 3)// freeRdp
    {
        object = gtk_builder_get_object (g_builder, "entry_ip");
        len = gtk_entry_get_text_length(GTK_ENTRY(object));
        if (len == 0)
        {
            LogInfo("debug: IP don't null.\n");
            SetSymsgContext(LOGIN_STATUS_IP_NONULL);
            return -1;
        }
        char * ip = gtk_entry_get_text(GTK_ENTRY(object));
        if (!check_ipv4_valid(ip))
        {
            SetSymsgContext(LOGIN_STATUS_TYPE_ERROR);
            return -1;
        }
        //printf("username: %s.\n", szUsername);
        LogInfo("debug: ip: %s.\n" , ip);
        memset(g_loginfo.ip, 0, MAX_BUFF_SIZE);
        memcpy(g_loginfo.ip, ip, strlen(ip));

        object = gtk_builder_get_object (g_builder, "entry_port");
        len = gtk_entry_get_text_length(GTK_ENTRY(object));
        if (len == 0)
        {
            LogInfo("debug: port don't null.\n");
            SetSymsgContext(LOGIN_STATUS_PORT_NONULL);
            return -1;
        }
        char * port = gtk_entry_get_text(GTK_ENTRY(object));
        LogInfo("debug: ip: %s.\n" , port);
        g_loginfo.port = atoi(port);
    }
    object = gtk_builder_get_object (g_builder, "comboboxtext_entry");
    if (object == NULL)
    {
        LogInfo("debug: 444444444 object  null.\n");
        return -1;
    }
    len = gtk_entry_get_text_length(GTK_ENTRY(object));
    if (len == 0)
    {
        LogInfo("debug: username don't null.\n");
        SetSymsgContext(LOGIN_STATUS_USERNAME_NONULL);
        return -1;
    }
    char * szUsername = gtk_entry_get_text(GTK_ENTRY(object));
    //printf("username: %s.\n", szUsername);
    LogInfo("debug: username: %s.\n" ,szUsername);
    memset(g_loginfo.user, 0, MAX_BUFF_SIZE);
    memcpy(g_loginfo.user, szUsername, strlen(szUsername));
    //printf("username 222: %s.\n", g_loginfo.user);

    object = gtk_builder_get_object (g_builder, "entry_pass");
    len = gtk_entry_get_text_length(GTK_ENTRY(object));
    if (len == 0)
    {
        LogInfo("debug: password don't null.\n");
        SetSymsgContext(LOGIN_STATUS_PASS_NONULL);
        return -1;
    }
    char * szPassword = gtk_entry_get_text(GTK_ENTRY(object));
    //printf("password: %s.\n", szPassword);
    LogInfo("debug: password: %s.\n" ,szPassword);
    memset(g_loginfo.pass, 0, MAX_BUFF_SIZE);
    memcpy(g_loginfo.pass, szPassword, strlen(szPassword));
    //printf("password 222: %s.\n", g_loginfo.pass);
    g_loginfo.repass = g_checkrepass;
	g_loginfo.autologin = g_checkautologin;
    LogInfo("debug: reapss: %d.\n" , g_loginfo.repass);
	LogInfo("debug: auto: %d.\n" , g_loginfo.autologin);
    return 0;
}

static char* GetVmsName(char *vmid)
{
    static char szName[MAX_BUFF_SIZE] = {0};
    list_for_each(plist, &head)
    {
        struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
        if (strcmp(node->val.vmid, vmid) == 0)
        {
            strcpy(szName, node->val.name);
        }
    }
	return szName;
}

static pthread_t g_xtid = NULL;
static char szcmd[MAX_BUFF_SIZE] = {0};
static void thrd_exec()
{
	printf("thrd_exec @@@@@  cmd = %s.\n", szcmd);
	FILE *fp;
	if((fp = popen(szcmd, "r")) == NULL) {
	     perror("thrd_monitor file open failed.");
	}
	pclose(fp);
}

void shell_exec(char *cmd)
{
   printf("shell_exec @@@@@ 000.\n");
   strcpy(szcmd, cmd);
   printf("shell_exec @@@@@ 111 szcmd = %s.\n", szcmd);
   if ( pthread_create(&g_xtid, NULL, thrd_exec, NULL) !=0 ) {
	    printf("shell_exec , Create thread error!\n");
	};
   pthread_join(g_xtid, NULL);
}

static void connectDisVm(char *ip, int port, char *vmid, int nstate)
{
    if ( vmid == NULL || strlen(vmid) <= 0 )
		return;
    printf("connectDisVm 111 .\n");
	char szIP[MAX_BUFF_SIZE] = {0};
	int nport = port;
	strcpy(szIP, ip);
	//one vm
	//add by kevin
	SetSymsgContext(LOGIN_STATUS_CONNECTING);
//	if (Ovirt_GetVm2(ovirt_url, g_loginfo.user, g_loginfo.pass, vmid) < 0)
//	{
//	    LogInfo("Debug: connectDisVm Ovirt_GetVm2() < 0.\n");
//	    return;
//	}
//	printf("connectDisVm 222 .\n");
//	int nstate = SY_GetVmState(vmid);
	if (nstate == 0) //vm is close
	{
		if (Ovirt_StartVms(ovirt_url, g_loginfo.user, g_loginfo.pass, vmid) < 0)
		{
		    LogInfo("Debug: connectDisVm Ovirt_StartVms() < 0, start vm failed.\n");
			return;
		}
		SetSymsgContext(LOGIN_STATUS_GETVMS);
		if (Ovirt_Login(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
	    {
	        LogInfo("main Ovirt login failed.\n");
	        SetSymsgContext(LOGIN_STATUS_FAILED);
	        return;
	    }
		int nflag = 0;
		int i = 0;
		for(;i<10;i++)
		{
			if (Ovirt_GetVms(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
		    {
		        LogInfo("main Ovirt get vms failed.\n");
		        SetSymsgContext(LOGIN_STATUS_GETVMS_FAILD);
		        return;
		    }
			if (SY_GetVms() < 0)
	        {
	            SetSymsgContext(LOGIN_STATUS_USER_PASS_ERROR);
	            return;
	        }
			 list_for_each(plist, &head)
	          {
				struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
				if (strcmp(node->val.vmid, vmid) == 0 && node->val.status != 0)
				{
					strcpy(szIP, node->val.ip);
					nport = node->val.port;
					nflag = 1;
					break;
				}
			 }
			 if (nflag == 1)
			 	break;
			 sleep(2);
		}
		printf("connectDisVm 222 .\n");
	}
	printf("connectDisVm 444 .\n");
	if (Ovirt_GetVmTicket(ovirt_url, g_loginfo.user, g_loginfo.pass, vmid) < 0)
	{
		SetSymsgContext(SY_OVIRT_GETVMSTICKET_FAILED);
		return;
	}
    char szTicket[MAX_BUFF_SIZE] = {0};
    char shellcmd[MAX_BUFF_SIZE] = {0};
    if ( SY_GetVmsTicket(szTicket) < 0)
    	{
    	    SetSymsgContext(SY_OVIRT_GETVMSTICKET_FAILED);
		return;
	}
    //find vm
    if (SY_GetVms() < 0)
    	{
		LogInfo("Debug: ShenCloud_login, SY_GetVms() failed. \n");
		return;
	}
	SetSymsgContext(LOGIN_SUCCESSED);
	printf("connectDisVm 555 .\n");
	strcpy(report.uname, g_loginfo.user);
	strcpy(report.vname, GetVmsName(vmid));
	report.action = 1;
	send_data(report);
	report.action = 2;
	send_data(report);
	if (strcmp(szTicket, "") == 0)
		sprintf(shellcmd, "spicy -h %s -p %d -f >> %s", szIP, nport, "/var/log/shencloud/spicy.log");
    else
    		sprintf(shellcmd, "spicy -h %s -p %d -w %s -f >> %s", szIP, nport, szTicket, "/var/log/shencloud/spicy.log");
    LogInfo("Debug: login window directly connect vms  : %s. \n", shellcmd);
    wirte_conflag_data("/tmp/syp_reconnect", shellcmd);
	wirte_conflag_data("/tmp/host", szIP);
    shell_exec(shellcmd);
    //system(shellcmd);
    //thrd_monitor(shellcmd);
	report.action = 3;
    send_data(report);
	report.action = 4;
	send_data(report);
	close_tcpclient();
	close_loginwindow();
}

static void connectDisVm2(char *ip, int port, char *vmid, int nstate)
{
    if ( vmid == NULL || strlen(vmid) <= 0 )
		return;
    printf("connectDisVm 111 .\n");
	char szIP[MAX_BUFF_SIZE] = {0};
	int nport = port;
	strcpy(szIP, ip);
	//one vm
	//add by kevin
	//SetSymsgContext(LOGIN_STATUS_CONNECTING);
//	if (Ovirt_GetVm2(ovirt_url, g_loginfo.user, g_loginfo.pass, vmid) < 0)
//	{
//	    LogInfo("Debug: connectDisVm Ovirt_GetVm2() < 0.\n");
//	    return;
//	}
//	printf("connectDisVm 222 .\n");
//	int nstate = SY_GetVmState(vmid);
	if (nstate == 0) //vm is close
	{
		if (Ovirt_StartVms(ovirt_url, g_loginfo.user, g_loginfo.pass, vmid) < 0)
		{
		    LogInfo("Debug: connectDisVm Ovirt_StartVms() < 0, start vm failed.\n");
			return;
		}
		//SetSymsgContext(LOGIN_STATUS_GETVMS);
		if (Ovirt_Login(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
	    {
	        LogInfo("main Ovirt login failed.\n");
	        //SetSymsgContext(LOGIN_STATUS_FAILED);
	        return;
	    }
		int nflag = 0;
		int i=0;
		for(;i<10;i++)
		{
			if (Ovirt_GetVms(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
		    {
		        LogInfo("main Ovirt get vms failed.\n");
		        //SetSymsgContext(LOGIN_STATUS_GETVMS_FAILD);
		        return;
		    }
			if (SY_GetVms() < 0)
	        {
	            //SetSymsgContext(LOGIN_STATUS_USER_PASS_ERROR);
	            return;
	        }
			 list_for_each(plist, &head)
	          {
				struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
				if (strcmp(node->val.vmid, vmid) == 0 && node->val.status != 0)
				{
					strcpy(szIP, node->val.ip);
					nport = node->val.port;
					nflag = 1;
					break;
				}
			 }
			 if (nflag == 1)
			 	break;
			 sleep(2);
		}
		printf("connectDisVm 222 .\n");
	}
	printf("connectDisVm 444 .\n");
	if (Ovirt_GetVmTicket(ovirt_url, g_loginfo.user, g_loginfo.pass, vmid) < 0)
	{
		//SetSymsgContext(SY_OVIRT_GETVMSTICKET_FAILED);
		return;
	}
    char szTicket[MAX_BUFF_SIZE] = {0};
    char shellcmd[MAX_BUFF_SIZE] = {0};
    if ( SY_GetVmsTicket(szTicket) < 0)
    	{
    	    //SetSymsgContext(SY_OVIRT_GETVMSTICKET_FAILED);
		return;
	}
    //find vm
    if (SY_GetVms() < 0)
    	{
		LogInfo("Debug: ShenCloud_login, SY_GetVms() failed. \n");
		return;
	}
	//SetSymsgContext(LOGIN_SUCCESSED);
	printf("connectDisVm 555 .\n");
	report_msg.action = MSG_WINDOW_CLOSE;
	server_send_msg(report_msg);
	strcpy(report.uname, g_loginfo.user);
	strcpy(report.vname, GetVmsName(vmid));
	report.action = 1;
	send_data(report);
	report.action = 2;
	send_data(report);
	if (strcmp(szTicket, "") == 0)
		sprintf(shellcmd, "spicy -h %s -p %d -f >> %s", szIP, nport, "/var/log/shencloud/spicy.log");
    else
    		sprintf(shellcmd, "spicy -h %s -p %d -w %s -f >> %s", szIP, nport, szTicket, "/var/log/shencloud/spicy.log");
    LogInfo("Debug: login window directly connect vms  : %s. \n", shellcmd);
    wirte_conflag_data("/tmp/syp_reconnect", shellcmd);
	wirte_conflag_data("/tmp/host", szIP);
    shell_exec(shellcmd);
    //system(shellcmd);
    //thrd_monitor(shellcmd);
	report.action = 3;
    send_data(report);
	report.action = 4;
	send_data(report);
	close_tcpclient();
	close_loginwindow();
}

void close_loginwindow()
{
	 if (g_window != NULL && showloginwindow == 1)
    {
	    gtk_widget_destroy((GtkWidget *)g_window);
	    gtk_main_quit();
    	}
    showloginwindow = 0;
    g_loginExit = 0;
}

static pthread_t wait_win_tid;

static void exec_wait_window()
{
	system("sudo ./wait3");
}

void start_wait_window()
{
	if ( pthread_create(&wait_win_tid, NULL, exec_wait_window, NULL) !=0 ) 
	{
		printf("Create start_wait_window thrd error!\n");
	}
}

int ShenCloud_autoLogin()
{
   start_wait_window();
   g_selectProto = 0;
   GetLoginInfo(&g_loginfo);
   if (g_loginfo.autologin == 0)
   		return -1;
	if (strcmp(g_loginfo.user, "") == 0 && strlen(g_loginfo.user) <= 0)
	{
		LogInfo("ShenCloud_autoLogin,  user == "" or user len <= 0 .\n");
		return -1;
	}
	if (strcmp(g_loginfo.pass, "") == 0 && strlen(g_loginfo.pass) <= 0)
	{
		LogInfo("ShenCloud_autoLogin,  pass == "" or pass len <= 0 .\n");
		return -1;
	}
	LogInfo("start auto login, ShenCloud_autoLogin() use.\n");
	initOvirtUrl();
	if (g_selectProto == 0)
	{
	    int i=0;
	    for (; i< 6; i++)
	    	{
		    if (Ovirt_Login(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
		    {
		        LogInfo("main Ovirt login failed.\n");
		        //SetSymsgContext(LOGIN_STATUS_FAILED);
		    }else
		    {
				break;
			}
		    sleep(1);
	    	}
	    if (Ovirt_GetVms(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
	    {
	        LogInfo("main Ovirt get vms failed.\n");
	        //SetSymsgContext(LOGIN_STATUS_GETVMS_FAILD);
	        return -1;
	    }
	}
    //获取服务器虚拟机列表数据
    if (g_selectProto == 0)  //shencloud
    {
        if (SY_GetVms() < 0)
        {
            //SetSymsgContext(LOGIN_STATUS_USER_PASS_ERROR);
            return -1;
        }
        //SetSymsgContext(LOGIN_STATUS_GETVMS);
        LogInfo("login server shencloud, get vm  count : %d.\n", g_nVmCount);
        if (g_nVmCount == 1)
        {
        	   //SetSymsgContext(LOGIN_SUCCESSED);
        	   start_tcpclient();
            //直接连接虚拟机
            list_for_each(plist, &head)
            {
                struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
				if (node != NULL)
				{
					LogInfo("Debug: login window autologin ---- directly connectVms g_loginfo.pass: = %s,  g_loginfo.pass = %s, vmid = %s, ip = %s.\n", g_loginfo.user, g_loginfo.pass, node->val.vmid, node->val.ip);
					connectDisVm2(node->val.ip, node->val.port, node->val.vmid, node->val.status);
					return 0;
				}
            }
        }
	    if (g_nVmCount == 0)
	    {
		   //SetSymsgContext(LOGIN_STATUS_GETVMS_FAILD);
	        return -1;
		}
    }
      //SetSymsgContext(LOGIN_SUCCESSED);
//    if (g_selectProto == 0 && g_nVmCount > 1)
//    {
//         printf("terminate  44444444  2222222 8888.\n");
//    		SY_vmlistwindow_main();
//    }
    return 0;
}

int ShenCloud_classLogin()
{
   g_selectProto = 0;
   struct LoginInfo info = {"", "", "", "", 3389, 0, 0, 0};
   GetLoginInfo(&info);
	if (strcmp(info.user, "") == 0 && strlen(info.user) <= 0)
	{
		LogInfo("ShenCloud_autoLogin,  user == "" or user len <= 0 .\n");
		return -1;
	}
	if (strcmp(info.pass, "") == 0 && strlen(info.pass) <= 0)
	{
		LogInfo("ShenCloud_autoLogin,  pass == "" or pass len <= 0 .\n");
		return -1;
	}
	LogInfo("start auto login, ShenCloud_autoLogin() use.\n");
	initOvirtUrl();
    //SetSymsgContext(LOGIN_STATUS_CONNECTING);
	if (g_selectProto == 0)
	{
	    if (Ovirt_Login(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
	    {
	        LogInfo("main Ovirt login failed.\n");
	        //SetSymsgContext(LOGIN_STATUS_FAILED);
	        return -1;
	    }
	    if (Ovirt_GetVms(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
	    {
	        LogInfo("main Ovirt get vms failed.\n");
	        //SetSymsgContext(LOGIN_STATUS_GETVMS_FAILD);
	        return -1;
	    }
	}
    //获取服务器虚拟机列表数据
    if (g_selectProto == 0)  //shencloud
    {
        if (SY_GetVms() < 0)
        {
            //SetSymsgContext(LOGIN_STATUS_USER_PASS_ERROR);
            return -1;
        }
        //SetSymsgContext(LOGIN_STATUS_GETVMS);
        LogInfo("login server shencloud, get vm  count : %d.\n", g_nVmCount);
        if (g_nVmCount == 1)
        {
        	   start_tcpclient();
            //直接连接虚拟机
            list_for_each(plist, &head)
            {
                struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
				if (node != NULL)
				{
					LogInfo("Debug: login window autologin ---- directly connectVms g_loginfo.pass: = %s,  g_loginfo.pass = %s, vmid = %s, ip = %s.\n", g_loginfo.user, g_loginfo.pass, node->val.vmid, node->val.ip);
					connectDisVm2(node->val.ip, node->val.port, node->val.vmid, node->val.status);
					return 0;
				}
            }
        }
	    if (g_nVmCount == 0)
	    {
		   //SetSymsgContext(LOGIN_STATUS_GETVMS_FAILD);
	        return -1;
		}
    }
    //SetSymsgContext(LOGIN_SUCCESSED);
    return 0;
}


int ShenCloud_login()
{
    LogInfo("it is login_window exit. \n");
    if (get_ctrldata() < 0)
      return -1;
    SetSymsgContext(LOGIN_STATUS_CONNECTING);
	if (g_selectProto == 0)
	{
	    if (Ovirt_Login(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
	    {
	        LogInfo("main Ovirt login failed.\n");
	        SetSymsgContext(LOGIN_STATUS_FAILED);
	        g_loginExit = 1;
	        return -1;
	    }
	    SaveLogin(g_loginfo);
	    if (Ovirt_GetVms(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
	    {
	        LogInfo("main Ovirt get vms failed.\n");
	        SetSymsgContext(LOGIN_STATUS_GETVMS_FAILD);
	        return -1;
	    }
	}
    //获取服务器虚拟机列表数据
    if (g_selectProto == 0)  //shencloud
    {
        if (SY_GetVms() < 0)
        {
            SetSymsgContext(LOGIN_STATUS_USER_PASS_ERROR);
            return -1;
        }
        SetSymsgContext(LOGIN_STATUS_GETVMS);
        LogInfo("login server shencloud, get vm  count : %d.\n", g_nVmCount);
        if (g_nVmCount == 1)
        {
        	   start_tcpclient();
            //直接连接虚拟机
            list_for_each(plist, &head)
            {
                struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
				if (node != NULL)
				{
					LogInfo("Debug: login window directly connectVms g_loginfo.pass: = %s,  g_loginfo.pass = %s, vmid = %s, ip = %s.\n", g_loginfo.user, g_loginfo.pass, node->val.vmid, node->val.ip);
					connectDisVm(node->val.ip, node->val.port, node->val.vmid, node->val.status); //add by kevin 2016/9/30
					return 0;
				}
            }
        }
	    if (g_nVmCount == 0)
	    {
		   SetSymsgContext(LOGIN_STATUS_GETVMS_FAILD);
	        return -1;
		}
    }
    int nRet = 0;
    if (g_selectProto == 1)
    {
        SaveMirLogin(g_loginfo);
        LogInfo("login server mirfeerdp , directly connect vm, user = %s, pass = %s , ip =%s.\n", g_loginfo.user, g_loginfo.pass, g_loginfo.ip);
        nRet = Run_FreeRDP(g_loginfo.user, g_loginfo.pass, g_loginfo.ip);
        if (nRet < 0)
        {
            switch (abs(nRet)) {
              case 7:
				 SetSymsgContext(LOGIN_STATUS_CONNECT_FAILED_AIR);
                 break;
              case 2:
				 SetSymsgContext(LOGIN_STATUS_IP_ERROR);
                 break;
            }
            LogInfo("Debug: login window mirsoft, freeredp directly connect vms failed, nRet: %d. \n", nRet);
            return -1;
        }
    }
    SetSymsgContext(LOGIN_SUCCESSED);
    return 0;
}

static void loadImage()
{
    GdkScreen* screen;
	screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	if (scr_width == 1024 && scr_height == 768)
	{
	    g_loginPress = gdk_pixbuf_new_from_file("images2/1024x768/btnlogin_press.png", NULL);
	    g_loginNor = gdk_pixbuf_new_from_file("images2/1024x768/btnlogin_nor.png", NULL);

	    g_checkNorimage = gdk_pixbuf_new_from_file("images2/1024x768/checkbtn_nor.png", NULL);
	    g_checkPressimage = gdk_pixbuf_new_from_file("images2/1024x768/checkbtn_press.png", NULL);

	    g_prevPress = gdk_pixbuf_new_from_file("images2/1024x768/loginout_nor.png", NULL);
	    g_prevNor = gdk_pixbuf_new_from_file("images2/1024x768/loginout_press.png", NULL);

		g_shutdownPress = gdk_pixbuf_new_from_file("images2/1024x768/exit_nor.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1024x768/exit_press.png", NULL);

		g_logoVm = gdk_pixbuf_new_from_file("images2/1024x768/vm_logo.png", NULL);
	    	g_logoCit = gdk_pixbuf_new_from_file("images2/1024x768/cit_logo.png", NULL);
	    	g_logoMir = gdk_pixbuf_new_from_file("images2/1024x768/mir_logo.png", NULL);
	    	g_logoSh = gdk_pixbuf_new_from_file("images2/1024x768/sh_logo.png", NULL);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		g_loginPress = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_PRES, NULL);
	    g_loginNor = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_NOR, NULL);

	    g_checkNorimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_NOR, NULL);
	    g_checkPressimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_PRESS, NULL);

	    g_prevPress = gdk_pixbuf_new_from_file(IMAGE_BTN_PREV_PRES, NULL);
	    g_prevNor = gdk_pixbuf_new_from_file(IMAGE_BTN_PREV_NOR, NULL);

		g_shutdownPress = gdk_pixbuf_new_from_file(IMAGE_BTN_SHUTDWON_PRES, NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file(IMAGE_BTN_SHUTDWON_NOR, NULL);

		g_logoVm = gdk_pixbuf_new_from_file(IMAGE_LOGO_VM, NULL);
	    	g_logoCit = gdk_pixbuf_new_from_file(IMAGE_LOGO_CIT, NULL);
	    	g_logoMir = gdk_pixbuf_new_from_file(IMAGE_LOGO_MIR, NULL);
	    	g_logoSh = gdk_pixbuf_new_from_file(IMAGE_LOGO_SH, NULL);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 ) 
	    || (scr_width == 1600 && scr_height == 1080))
	{
		g_loginPress = gdk_pixbuf_new_from_file("images2/1440x900/btnlogin_press.png", NULL);
	    g_loginNor = gdk_pixbuf_new_from_file("images2/1440x900/btnlogin_nor.png", NULL);

	    g_checkNorimage = gdk_pixbuf_new_from_file("images2/1440x900/checkbtn_nor.png", NULL);
	    g_checkPressimage = gdk_pixbuf_new_from_file("images2/1440x900/checkbtn_press.png", NULL);

	    g_prevPress = gdk_pixbuf_new_from_file("images2/1440x900/loginout_nor.png", NULL);
	    g_prevNor = gdk_pixbuf_new_from_file("images2/1440x900/loginout_press.png", NULL);

		g_shutdownPress = gdk_pixbuf_new_from_file("images2/1440x900/exit_nor.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1440x900/exit_press.png", NULL);

		g_logoVm = gdk_pixbuf_new_from_file("images2/1440x900/vm_logo.png", NULL);
	    	g_logoCit = gdk_pixbuf_new_from_file("images2/1440x900/cit_logo.png", NULL);
	    	g_logoMir = gdk_pixbuf_new_from_file("images2/1440x900/mir_logo.png", NULL);
	    	g_logoSh = gdk_pixbuf_new_from_file("images2/1440x900/sh_logo.png", NULL);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		g_loginPress = gdk_pixbuf_new_from_file("images2/1280x720/btnlogin_press.png", NULL);
	    g_loginNor = gdk_pixbuf_new_from_file("images2/1280x720/btnlogin_nor.png", NULL);

	    g_checkNorimage = gdk_pixbuf_new_from_file("images2/1280x720/checkbtn_nor.png", NULL);
	    g_checkPressimage = gdk_pixbuf_new_from_file("images2/1280x720/checkbtn_press.png", NULL);

	    g_prevPress = gdk_pixbuf_new_from_file("images2/1280x720/loginout_nor.png", NULL);
	    g_prevNor = gdk_pixbuf_new_from_file("images2/1280x720/loginout_press.png", NULL);

		g_shutdownPress = gdk_pixbuf_new_from_file("images2/1280x720/exit_nor.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1280x720/exit_press.png", NULL);

		g_logoVm = gdk_pixbuf_new_from_file("images2/1280x720/vm_logo.png", NULL);
	    	g_logoCit = gdk_pixbuf_new_from_file("images2/1280x720/cit_logo.png", NULL);
	    	g_logoMir = gdk_pixbuf_new_from_file("images2/1280x720/mir_logo.png", NULL);
	    	g_logoSh = gdk_pixbuf_new_from_file("images2/1280x720/sh_logo.png", NULL);
		//do code
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
			(scr_width == 1360 && scr_height == 768))
	{
		g_loginPress = gdk_pixbuf_new_from_file("images2/1366x768/btnlogin_press.png", NULL);
	    g_loginNor = gdk_pixbuf_new_from_file("images2/1366x768/btnlogin_nor.png", NULL);

	    g_checkNorimage = gdk_pixbuf_new_from_file("images2/1366x768/checkbtn_nor.png", NULL);
	    g_checkPressimage = gdk_pixbuf_new_from_file("images2/1366x768/checkbtn_press.png", NULL);

	    g_prevPress = gdk_pixbuf_new_from_file("images2/1366x768/loginout_nor.png", NULL);
	    g_prevNor = gdk_pixbuf_new_from_file("images2/1366x768/loginout_press.png", NULL);

		g_shutdownPress = gdk_pixbuf_new_from_file("images2/1366x768/exit_nor.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1366x768/exit_press.png", NULL);

		g_logoVm = gdk_pixbuf_new_from_file("images2/1366x768/vm_logo.png", NULL);
	    	g_logoCit = gdk_pixbuf_new_from_file("images2/1366x768/cit_logo.png", NULL);
	    	g_logoMir = gdk_pixbuf_new_from_file("images2/1366x768/mir_logo.png", NULL);
	    	g_logoSh = gdk_pixbuf_new_from_file("images2/1366x768/sh_logo.png", NULL);
		//do code
	}
}

static void init_ctrl_image(GtkBuilder *builder)
{
	GObject *image_btnlogin;
	GObject *image_shutdown;
	GObject *image_loginout;
	GObject *image_logo;
	image_btnlogin = gtk_builder_get_object (builder, "image_btnlogin");
	image_shutdown = gtk_builder_get_object (builder, "image_shutdown");
	image_loginout = gtk_builder_get_object (builder, "image_loginout");
	image_logo = gtk_builder_get_object (builder, "image_logo");
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_btnlogin), g_loginNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_shutdown), g_shutdownNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_loginout), g_prevNor);
	if (g_selectProto == 0)
		gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_logoSh);
	else if (g_selectProto == 1)
		gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_logoMir);
}

void init_logo_ctrl(GtkBuilder *builder, GtkWidget *widget, int widget_width, int widget_height)
{
	GdkPixbufFormat *formt = NULL;
	GdkScreen* screen;
	screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	int pic_width = 0;
	int pic_height = 0;
	if (scr_width == 1024 && scr_height == 768)
	{
		gdk_pixbuf_get_file_info("images2/1024x768/sh_logo.png", &pic_width, &pic_height);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		gdk_pixbuf_get_file_info(IMAGE_LOGO_SH, &pic_width, &pic_height);
		widget_height = 58;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
	{
		gdk_pixbuf_get_file_info("images2/1440x900/sh_logo.png", &pic_width, &pic_height);
	}else if ((scr_width == 1280 && scr_height == 720 ) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		gdk_pixbuf_get_file_info("images2/1280x720/sh_logo.png", &pic_width, &pic_height);
		//do code
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		gdk_pixbuf_get_file_info("images2/1366x768/sh_logo.png", &pic_width, &pic_height);
		//do code
	}
	int x = widget_width/2 - pic_width/2;
	int y = (widget_height - pic_height)/2;
	int ndelay = 0;
	if (g_selectProto == 0)
		ndelay = -2;
	else if (g_selectProto == 1)
		ndelay = 10;

	GObject *image_logo;
	image_logo = gtk_builder_get_object(builder, "image_logo");
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(image_logo), x + ndelay, y);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_logoSh);
}

static void create_surfaces()
{
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	if ((scr_width == 1024 && scr_height == 768 ))
	{
		surface1 = cairo_image_surface_create_from_png ("images2/1024x768/login.png");
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		surface1 = cairo_image_surface_create_from_png (IMAGE_MAIN_BACKGROUD);
	}else if (scr_width == 1440 && scr_height == 900 )
	{
		surface1 = cairo_image_surface_create_from_png ("images2/1440x900/login.png");
	}else if ((scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080) )
	{
		surface1 = cairo_image_surface_create_from_png ("images2/1600x900/login.png");
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768))
	{
		surface1 = cairo_image_surface_create_from_png ("images2/1280x720/login.png");
		//do code;
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768) ||
		(scr_width == 1366 && scr_height == 768) )
	{
		surface1 = cairo_image_surface_create_from_png ("images2/1366x768/login.png");
		//do code;
	}else if ((scr_width == 1280 && scr_height == 1024))
	{
		surface1 = cairo_image_surface_create_from_png ("images2/1280x1024/login.png");
	}
}

static void destroy_surfaces()
{
    if (surface1 != NULL)
		cairo_surface_destroy (surface1);
}

void setfontcolor(GtkWidget * widget, char *value)
{
	GdkRGBA color1;
	gdk_rgba_parse(&color1, value);
	gtk_widget_override_color(widget, GTK_STATE_FLAG_NORMAL, &color1);
}
void setctrlFont(GtkWidget * widget, int nsize)
{
	PangoFontDescription *font1 = pango_font_description_from_string("Microsoft YaHei");
	pango_font_description_set_size(font1, nsize * PANGO_SCALE);
	gtk_widget_override_font(widget, font1);
}

static void get_exitctrl(int scr_width, int scr_height, int *exit_width, int *exit_height)
{
	if ((scr_width == 1024 && scr_height == 768) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  ||
		(scr_width == 1600 && scr_height == 1080))
	{
	    *exit_width = 16;
		*exit_height = 18;
	}else if ((scr_width == 1920 && scr_height == 1080) ||
	  (scr_width == 1920 && scr_height == 1200))
	{
		*exit_width = 80;
		*exit_height = 30;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	  (scr_width == 1280 && scr_height == 1024) )
	{
		*exit_width = 17;
		*exit_height = 20;
		//do code
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
	   (scr_width == 1360 && scr_height == 768) )
	{
		//do code
		*exit_width = 20;
		*exit_height = 23;
	}
}

void get_logoutctrl(int scr_width, int scr_height, int *logout_width, int *logout_height)
{
	if (scr_width == 1024 && scr_height == 768)
	{
	    *logout_width =43;
		*logout_height = 14;
	}else if ((scr_width == 1920 && scr_height == 1080) ||
		(scr_width == 1920 && scr_height == 1200))
	{
		*logout_width = 80;
		*logout_height = 30;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  ||
	    (scr_width == 1600 && scr_height == 1080) )
	{
		*logout_width = 21;
		*logout_height = 22;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	  (scr_width == 1280 && scr_height == 1024) )
	{
		*logout_width = 17;
		*logout_height = 17;
		//do code
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
	  (scr_width == 1360 && scr_height == 768))
	{
		//do code
		*logout_width = 21;
		*logout_height = 22;
	}
}

static void loadcss()
{
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;
    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gsize bytes_written, bytes_read;
    GError *error = 0;
    gtk_css_provider_load_from_path (provider,
                                  g_filename_to_utf8(g_logincss, strlen(g_logincss), &bytes_read, &bytes_written, &error),
                                  NULL);
    //gtk_css_provider_load_from_path (provider, home, NULL);
    g_object_unref (provider);
}

//draw background
static gboolean on_draw_event (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
     //g_print("on_expose_logineven start.\n");
    //LogInfo("debug: on_expose_logineven start.\n");
    GdkWindow *window;
    cairo_t *cr;
    window = gtk_widget_get_window(widget);
    cr = gdk_cairo_create(window);
    cairo_set_source_surface (cr, surface1, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}

void initAddressCtrl(GtkBuilder *builder, GtkWidget *widget, int widget_height)
{
	GObject *label_ip;
	GObject *entry_ip;
	GObject *label_port;
	GObject *entry_port;
	label_ip = gtk_builder_get_object(builder, "label_ip");
	entry_ip = gtk_builder_get_object(builder, "entry_ip");
	label_port = gtk_builder_get_object(builder, "label_port");
	entry_port = gtk_builder_get_object(builder, "entry_port");
	int space_left = 184;
	int space_right = 180;
	int delay = 2;
	GdkScreen* screen;
	screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	if (scr_width == 1024 && scr_height == 768)
	{
		space_left = 145;
		delay = 10;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_ip), space_left, 4);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_ip), space_left + 20 + delay/2, 0);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_port), space_left + 20 + delay + 88 + delay/2, 4);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_port), space_left + 20 + delay + 90 + delay + 12 + delay, 0);
		gtk_widget_set_size_request(GTK_WIDGET(label_ip), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_ip), 60, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_port), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_port), 40, 20);
		gtk_entry_set_width_chars((GtkEntry *)entry_ip, 12);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
	    int height_edit = 30;
		height_edit = (widget_height - height_edit)/2;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_ip), space_left, 2);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_ip), space_left + 30 + delay , height_edit);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_port), space_left + 60 + delay + 150 + delay , 2);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_port), space_left + 60 + delay + 150 + delay + 60 + delay , height_edit);
		gtk_widget_set_size_request(GTK_WIDGET(label_ip), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_ip), 80, height_edit);
		gtk_widget_set_size_request(GTK_WIDGET(label_port), 60, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_port), 80, height_edit);
		gtk_entry_set_width_chars((GtkEntry *)entry_ip, 22);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  ||
	   (scr_width == 1600 && scr_height == 1080) )
	{
	    space_left = 145;
		int height_edit = 30;
		height_edit = (widget_height - height_edit)/2;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_ip), space_left, 2);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_ip), space_left + 30 + delay , height_edit - 2);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_port), space_left + 60 + delay + 100 + delay , 2);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_port), space_left + 60 + delay + 100 + delay + 60 + delay , height_edit - 2);
		gtk_widget_set_size_request(GTK_WIDGET(label_ip), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_ip), 80, height_edit);
		gtk_widget_set_size_request(GTK_WIDGET(label_port), 60, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_port), 80, height_edit);
		gtk_entry_set_width_chars((GtkEntry *)entry_ip, 15);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	    (scr_width == 1280 && scr_height == 1024))
	{
		space_left = 145;
		delay = 10;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_ip), space_left, 4);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_ip), space_left + 20 + delay/2, 0);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_port), space_left + 20 + delay + 88 + delay/2, 4);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_port), space_left + 20 + delay + 90 + delay + 12 + delay, 0);
		gtk_widget_set_size_request(GTK_WIDGET(label_ip), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_ip), 60, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_port), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_port), 40, 20);
		gtk_entry_set_width_chars((GtkEntry *)entry_ip, 12);
		//do code
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
	   (scr_width == 1360 && scr_height == 768))
	{
		//do code
		space_left = 130;
		delay = 10;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_ip), space_left, 4);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_ip), space_left + 20 + delay/2, 0);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_port), space_left + 20 + delay + 88 + delay/2 + delay*2, 4);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_port), space_left + 20 + delay + 90 + delay + 12 + delay*3, 0);
		gtk_widget_set_size_request(GTK_WIDGET(label_ip), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_ip), 60, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_port), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_port), 40, 20);
		gtk_entry_set_width_chars((GtkEntry *)entry_ip, 14);
	}
	//set text
	gtk_label_set_text(GTK_LABEL(label_ip), "IP");
	gtk_label_set_text(GTK_LABEL(label_port), "端口");
	//set font size
	int nsize = 14;
	if (scr_width == 1024 && scr_height == 768)
	{
		nsize = 8;
		setctrlFont(GTK_WIDGET(label_port), nsize);
		setctrlFont(GTK_WIDGET(label_ip), nsize);
		setctrlFont(GTK_WIDGET(entry_ip), 7);
		setctrlFont(GTK_WIDGET(entry_port), 7);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		nsize = 10;
		setctrlFont(GTK_WIDGET(label_port), 12);
		setctrlFont(GTK_WIDGET(label_ip), 12);
		setctrlFont(GTK_WIDGET(entry_ip), nsize);
		setctrlFont(GTK_WIDGET(entry_port), nsize);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  ||
	  (scr_width == 1600 && scr_height == 1080) )
	{
		nsize = 9;
		setctrlFont(GTK_WIDGET(label_port), 11);
		setctrlFont(GTK_WIDGET(label_ip), 11);
		setctrlFont(GTK_WIDGET(entry_ip), nsize);
		setctrlFont(GTK_WIDGET(entry_port), nsize);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	   (scr_width == 1280 && scr_height == 1024) )
	{
		//do code
		nsize = 9;
		setctrlFont(GTK_WIDGET(label_port), 11);
		setctrlFont(GTK_WIDGET(label_ip), 11);
		setctrlFont(GTK_WIDGET(entry_ip), nsize);
		setctrlFont(GTK_WIDGET(entry_port), nsize);
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
	  (scr_width == 1360 && scr_height == 768))
	{
		//do code
		nsize = 9;
		setctrlFont(GTK_WIDGET(label_port), 11);
		setctrlFont(GTK_WIDGET(label_ip), 11);
		setctrlFont(GTK_WIDGET(entry_ip), nsize);
		setctrlFont(GTK_WIDGET(entry_port), nsize);
	}
	setfontcolor(GTK_WIDGET(label_port), "#ffffff");
	setfontcolor(GTK_WIDGET(label_ip), "#ffffff");

}

void initUserCtrl(GtkBuilder *builder, GtkWidget *widget)
{
	GObject *label_user;
	GObject *comboboxtext_user;
	GObject *label_pass;
	GObject *entry_pass;
	GObject *eventbox_rempass;
	GObject *image_rempass;
	GObject *label_rempass;
	GObject *comboboxtext_entry;
	GObject *eventbox_auto;
	GObject *image_auto;
	GObject *label_auto;
	label_user = gtk_builder_get_object(builder, "label_user");
	comboboxtext_user = gtk_builder_get_object(builder, "comboboxtext_user");
	label_pass = gtk_builder_get_object(builder, "label_pass");
	entry_pass = gtk_builder_get_object(builder, "entry_pass");
	eventbox_rempass = gtk_builder_get_object(builder, "eventbox_rempass");
	label_rempass = gtk_builder_get_object(builder, "label_rempass");
	image_rempass = gtk_builder_get_object(builder, "image_rempass");
	comboboxtext_entry = gtk_builder_get_object(builder, "comboboxtext_entry");
	eventbox_auto = gtk_builder_get_object(builder, "eventbox_auto");
	image_auto = gtk_builder_get_object(builder, "image_auto");
	label_auto = gtk_builder_get_object(builder, "label_auto");
	int space_left = 180;
	int space_right = 180;
	int delay = 2;
	int delay_height = 30;
	int delay_image = 30;
	GdkScreen* screen;
	screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	if (scr_width == 1024 && scr_height == 768)
	{
	    space_left = 157;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_user), space_left + delay_image, 2*15 + 14);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_user), space_left + 40 + delay + delay_image, 2*15 + 10);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_pass), space_left + delay_image, 2*15 + delay_height + 22);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_pass), space_left + 40 + delay + delay_image, 2*15 + delay_height + 20);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_rempass), space_left + 40 + delay + delay_image*2, 2*15 + delay_height + 20 + 18 + delay_height/2);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_rempass), space_left + 40 + delay + 30 + delay + delay_image*2 - 10, 2*15 + delay_height + 20 + 20 + delay_height/2 - 2);
		//add by kevin 2016/11/4
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_auto), space_left + 40 + delay + delay_image*2 - 70, 2*15 + delay_height + 20 + 18 + delay_height/2 - 5);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_auto), space_left + 40 + delay + 30 + delay + delay_image*2 - 10 - 40 - 50, 2*15 + delay_height + 20 + 20 + delay_height/2 - 2 - 5);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_auto), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(image_auto), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(label_auto), 80, 30);
		//add end
		gtk_widget_set_size_request(GTK_WIDGET(label_user), 40, 20);
		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_user), 100, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_pass), 40, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_pass), 100, 20);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_rempass), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_rempass), 40, 20);
		gtk_widget_set_size_request(GTK_WIDGET(image_rempass), 30, 20);
		gtk_entry_set_width_chars((GtkEntry *)entry_pass, 8);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
	    space_left = 210;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_user), space_left + delay_image + 6, 2*15);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_user), space_left + 80 + delay + delay_image, 2*15 - 4);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_pass), space_left + delay_image, 2*15 + delay_height + 36);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_pass), space_left + 80 + delay + delay_image, 2*15 + delay_height + 30 + 8);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_rempass), space_left + 80 + delay + delay_image*2 + 10 + 20 + 10, 2*15 + delay_height + 30 + 30 + delay_height/2 + 10);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_rempass), space_left + 80 + delay + 30 + delay + delay_image*2 + 20 + 10, 2*15 + delay_height + 30 + 30 + delay_height/2 + 8);

		//add by kevin 2016/11/4
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_auto), space_left + 80 + delay + 30 + delay + delay_image*2 - 10 - 30 - 80 + 5 + 10, 2*15 + delay_height + 30 + 30 + delay_height/2 + 10);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_auto), space_left + 80 + delay + delay_image*2 + 15 - 10 - 80 + 10 + 10, 2*15 + delay_height + 30 + 30 + delay_height/2 + 8);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_auto), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(image_auto), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(label_auto), 80, 30);
		//add end
		gtk_widget_set_size_request(GTK_WIDGET(label_user), 80, 30);
		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_user), 200, 30);
		gtk_widget_set_size_request(GTK_WIDGET(label_pass), 80, 30);
		gtk_widget_set_size_request(GTK_WIDGET(entry_pass), 200, 30);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_rempass), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(label_rempass), 80, 30);
		gtk_widget_set_size_request(GTK_WIDGET(image_rempass), 30, 30);
		gtk_entry_set_width_chars((GtkEntry *)entry_pass, 15);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
	{
		space_left = 180;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_user), space_left + delay_image + 6, 2*15 + 14);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_user), space_left + 80 + delay + delay_image - 20, 2*15 + 11);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_pass), space_left + delay_image, 2*15 + delay_height + 36);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_pass), space_left + 80 + delay + delay_image - 10, 2*15 + delay_height + 30 + 8);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_rempass), space_left + 80 + delay + delay_image*2 + 15, 2*15 + delay_height + 30 + 30 + delay_height/2 + 6);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_rempass), space_left + 80 + delay + 30 + delay + delay_image*2, 2*15 + delay_height + 30 + 30 + delay_height/2 + 6);
		gtk_widget_set_size_request(GTK_WIDGET(label_user), 60, 30);
		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_user), 160, 30);
		gtk_widget_set_size_request(GTK_WIDGET(label_pass), 80, 30);
		gtk_widget_set_size_request(GTK_WIDGET(entry_pass), 110, 30);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_rempass), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(label_rempass), 80, 30);
		gtk_widget_set_size_request(GTK_WIDGET(image_rempass), 30, 30);
		//add by kevin 2016/11/4
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_auto), space_left + 80 + delay + 30 + delay + delay_image*2 - 10 - 30 - 80, 2*15 + delay_height + 30 + 30 + delay_height/2 + 6);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_auto), space_left + 80 + delay + delay_image*2 + 15 - 10 - 80, 2*15 + delay_height + 30 + 30 + delay_height/2 + 6);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_auto), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(image_auto), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(label_auto), 80, 30);
		//add end
		gtk_entry_set_width_chars((GtkEntry *)entry_pass, 12);
	}else if ((scr_width == 1280 && scr_height == 720 ) || (scr_width == 1280 && scr_height == 768) ||
	  (scr_width == 1280 && scr_height == 1024) )
	{
		//do code
		space_left = 157;
		if (scr_width == 1280 && scr_height == 1024)
		{
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_user), space_left + delay_image, 2*15 + 14 - 20);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_user), space_left + 40 + delay + delay_image, 2*15 + 10 - 20);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_pass), space_left + delay_image, 2*15 + delay_height + 22 - 15);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_pass), space_left + 40 + delay + delay_image, 2*15 + delay_height + 20 - 15);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_rempass), space_left + 40 + delay + delay_image*2, 2*15 + delay_height + 20 + 18 + delay_height/2 - 15);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_rempass), space_left + 40 + delay + 30 + delay + delay_image*2 - 10, 2*15 + delay_height + 20 + 20 + delay_height/2 - 2 - 15);
		}
		else
		{
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_user), space_left + delay_image, 2*15 + 14);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_user), space_left + 40 + delay + delay_image, 2*15 + 10);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_pass), space_left + delay_image, 2*15 + delay_height + 22);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_pass), space_left + 40 + delay + delay_image, 2*15 + delay_height + 20);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_rempass), space_left + 40 + delay + delay_image*2, 2*15 + delay_height + 20 + 18 + delay_height/2);
			gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_rempass), space_left + 40 + delay + 30 + delay + delay_image*2 - 10, 2*15 + delay_height + 20 + 20 + delay_height/2 - 2);
		}
		//add by kevin 2016/11/4
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_auto), space_left + 40 + delay + delay_image*2 - 80, 2*15 + delay_height + 20 + 18 + delay_height/2 - 20);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_auto), space_left + 40 + delay + 30 + delay + delay_image*2 - 10 - 40 - 50 - 8, 2*15 + delay_height + 20 + 20 + delay_height/2 - 2 - 20);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_auto), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(image_auto), 30, 30);
		gtk_widget_set_size_request(GTK_WIDGET(label_auto), 80, 30);
		//add end
		gtk_widget_set_size_request(GTK_WIDGET(label_user), 40, 20);
		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_user), 120, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_pass), 40, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_pass), 100, 20);
		//add by kevin
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_rempass), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_rempass), 40, 20);
		gtk_widget_set_size_request(GTK_WIDGET(image_rempass), 30, 20);
		//add end
		gtk_entry_set_width_chars((GtkEntry *)entry_pass, 8);
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
	  (scr_width == 1360 && scr_height == 768) )
	{
		//do code
		space_left = 137;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_user), space_left + delay_image + 8, 2*15 + 14);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(comboboxtext_user), space_left + 40 + delay + delay_image + 10, 2*15 + 10);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_pass), space_left + delay_image, 2*15 + delay_height + 22 + 10);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(entry_pass), space_left + 40 + delay + delay_image, 2*15 + delay_height + 20 + 12);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(eventbox_rempass), space_left + 40 + delay + delay_image*2, 2*15 + delay_height + 20 + 18 + delay_height/2 + 10);
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_rempass), space_left + 40 + delay + 30 + delay + delay_image*2 - 10, 2*15 + delay_height + 20 + 20 + delay_height/2 - 2 + 10);
		gtk_widget_set_size_request(GTK_WIDGET(label_user), 40, 20);
		gtk_widget_set_size_request(GTK_WIDGET(comboboxtext_user), 145, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_pass), 40, 20);
		gtk_widget_set_size_request(GTK_WIDGET(entry_pass), 100, 20);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_rempass), 30, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_rempass), 40, 20);
		gtk_widget_set_size_request(GTK_WIDGET(image_rempass), 30, 20);
		gtk_entry_set_width_chars((GtkEntry *)entry_pass, 8);
	}

	//set text
  	gtk_label_set_justify(GTK_LABEL(label_user), GTK_JUSTIFY_RIGHT);
  	gtk_label_set_justify(GTK_LABEL(label_pass), GTK_JUSTIFY_RIGHT);
	gtk_label_set_text(GTK_LABEL(label_user), "用户名");
	gtk_label_set_text(GTK_LABEL(label_pass), "密码");
	gtk_label_set_text(GTK_LABEL(label_rempass), "记住密码");
  gtk_label_set_text(GTK_LABEL(label_auto), "自动登录");
	//set font size
	int nsize = 12;
	if (scr_width == 1024 && scr_height == 768)
	{
		nsize = 7;
		setctrlFont(GTK_WIDGET(label_rempass), 6);
		setctrlFont(GTK_WIDGET(label_auto), 6);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200) )
	{
		setctrlFont(GTK_WIDGET(label_rempass), 10);
		setctrlFont(GTK_WIDGET(label_auto), 10);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
	{
		nsize = 10;
		setctrlFont(GTK_WIDGET(label_rempass), 9);
		setctrlFont(GTK_WIDGET(label_auto), 9);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	   (scr_width == 1280 && scr_height == 1024) )
	{
		//do code
		nsize = 9;
		setctrlFont(GTK_WIDGET(label_rempass), 8);
		setctrlFont(GTK_WIDGET(label_auto), 8);
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
	   (scr_width == 1360 && scr_height == 768) )
	{
		//do code
		nsize = 10;
		setctrlFont(GTK_WIDGET(label_rempass), 8);
	}
	setctrlFont(GTK_WIDGET(label_user), nsize);
	setctrlFont(GTK_WIDGET(label_pass), nsize);
	setctrlFont(GTK_WIDGET(entry_pass), nsize);
	setctrlFont(GTK_WIDGET(label_user), nsize);
	setctrlFont(GTK_WIDGET(comboboxtext_user), nsize);
	setfontcolor(GTK_WIDGET(label_user), "#ffffff");
	setfontcolor(GTK_WIDGET(label_pass), "#ffffff");
	setfontcolor(GTK_WIDGET(label_rempass), "#ffffff");
	setfontcolor(GTK_WIDGET(label_auto), "#ffffff");
}

void initLoginCtrl(GtkBuilder *builder, GtkWidget *widget)
{
	GObject *btn_login;
	GdkScreen* screen;
	screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	btn_login = gtk_builder_get_object(builder, "btn_login");
	int nwidth = 0;
	int nheight = 0;
	gtk_widget_get_size_request(widget, &nwidth, &nheight);
	printf("initLoginCtrl  get layout ctrl width=%d, height=%d.\n", nwidth, nheight);
	int nlogin_width = 400;
	int space_left = nwidth/2 - 400/2;
	int space_height = (nheight - 57)/2;
	if (scr_width == 1024 && scr_height == 768)
	{
		nlogin_width = 166;
		space_left = nwidth/2 - nlogin_width/2;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_login), space_left, space_height);
		gtk_widget_set_size_request(GTK_WIDGET(btn_login), nlogin_width, 30);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_login), space_left, space_height);
		gtk_widget_set_size_request(GTK_WIDGET(btn_login), nlogin_width, 57);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  ||
	  (scr_width == 1600 && scr_height == 1080))
	{
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_login), space_left, space_height);
		gtk_widget_set_size_request(GTK_WIDGET(btn_login), nlogin_width, 45);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	  (scr_width == 1280 && scr_height == 1024))
	{
		nlogin_width = 186;
		space_left = nwidth/2 - nlogin_width/2;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_login), space_left, space_height);
		gtk_widget_set_size_request(GTK_WIDGET(btn_login), nlogin_width, 30);
		//do code
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
	 (scr_width == 1360 && scr_height == 768))
	{
		//do code
		nlogin_width = 221;
		space_left = nwidth/2 - nlogin_width/2;
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_login), space_left, space_height);
		gtk_widget_set_size_request(GTK_WIDGET(btn_login), nlogin_width, 30);
	}
}

void SetCtrlPos(GtkBuilder *builder, int scr_layout3_height)
{
    int nwidth = 250;
	int nspace_height = 247;//170;
	int nspace_delay = 10;
	int ctrl_left  = 0;
	GObject *layout_logo;
	GObject *layout_mir;
	GObject *layout_ctrl;
	GObject *layout_login;
	GObject *layout3;
	GdkScreen* screen;
	screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	if (scr_width == 1024 && scr_height == 768)
	{
	    nwidth = nwidth*2;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		nwidth = nwidth*3;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
	{
		nwidth = nwidth*2.5;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		//do code
		 nwidth = nwidth*2;
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		//do code
		 nwidth = nwidth*2;
	}
	ctrl_left = scr_width/2 - nwidth/2;
	layout_logo = gtk_builder_get_object(builder, "layout_logo");
	layout_mir = gtk_builder_get_object(builder, "layout_mir");
	layout_ctrl = gtk_builder_get_object(builder, "layout_ctrl");
	layout_login = gtk_builder_get_object(builder, "layout_login");
	layout3 = gtk_builder_get_object(builder, "layout3");
	if (scr_width == 1024 && scr_height == 768)
	{
		nspace_height = 100;
		int ndelay = 0;
		if (g_selectProto == 0)
			ndelay = 40;
	  gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_logo), ctrl_left, nspace_height - 50/2 + ndelay);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_mir), ctrl_left, nspace_height + 50 + nspace_delay*2);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_ctrl), ctrl_left, nspace_height + 50 + nspace_delay + 32 + nspace_delay);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_login), ctrl_left, nspace_height + 50 + nspace_delay + 32 + nspace_delay + 160);
		gtk_widget_set_size_request(GTK_WIDGET(layout_ctrl), nwidth, 160);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200) ||  (scr_width == 1280 && scr_height == 1024))
	{
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_logo), ctrl_left, nspace_height - 50);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_mir), ctrl_left, nspace_height + 50 + nspace_delay);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_ctrl), ctrl_left, nspace_height + 50 + nspace_delay + 32 + nspace_delay);
		if (scr_width == 1280 && scr_height == 1024)
			gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_login), ctrl_left, nspace_height + 50 + nspace_delay + 32 + nspace_delay + 170 + nspace_delay);
		else
			gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_login), ctrl_left, nspace_height + 50 + nspace_delay + 32 + nspace_delay + 190 + nspace_delay);
		gtk_widget_set_size_request(GTK_WIDGET(layout_ctrl), nwidth, 180);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  ||
	   (scr_width == 1600 && scr_height == 1080))
	{
	    nspace_height = 150;
		if (g_interfacetype == 2)
			gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_logo), ctrl_left, nspace_height - 30);
		else
			gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_logo), ctrl_left, nspace_height - 20);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_mir), ctrl_left, nspace_height + 60 + nspace_delay);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_ctrl), ctrl_left, nspace_height + 50 + nspace_delay + 32 + nspace_delay);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_login), ctrl_left, nspace_height + 50 + nspace_delay + 32 + nspace_delay + 190 + nspace_delay);
		gtk_widget_set_size_request(GTK_WIDGET(layout_ctrl), nwidth, 180);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768))
	{
		//do code
		nspace_height = 100;
		int ndelay = 0;
		if (g_selectProto == 0)
			ndelay = 40;
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_logo), ctrl_left, nspace_height - 50/2 + ndelay);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_mir), ctrl_left, nspace_height + 25 + nspace_delay*2);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_ctrl), ctrl_left, nspace_height + 25 + nspace_delay + 32 + nspace_delay);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_login), ctrl_left, nspace_height + 25 + nspace_delay + 32 + nspace_delay + 160);
		gtk_widget_set_size_request(GTK_WIDGET(layout_ctrl), nwidth, 160);
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		//do code
		nspace_height = 95;
		int ndelay = 0;
		if (g_selectProto == 0)
			ndelay = 40;
	  	gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_logo), ctrl_left, nspace_height - 50/2 + ndelay);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_mir), ctrl_left, nspace_height + 50 + nspace_delay*2);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_ctrl), ctrl_left, nspace_height + 50 + nspace_delay + 32 + nspace_delay);
		gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(layout_login), ctrl_left, nspace_height + 50 + nspace_delay + 32 + nspace_delay + 160);
		gtk_widget_set_size_request(GTK_WIDGET(layout_ctrl), nwidth, 160);
	}
	gtk_widget_set_size_request(GTK_WIDGET(layout_logo), nwidth, 100);
	gtk_widget_set_size_request(GTK_WIDGET(layout_mir), nwidth, 40);
	gtk_widget_set_size_request(GTK_WIDGET(layout_login), nwidth, 58);
	printf("SetCtrlPos  layout_ctrl width=%d.\n", nwidth);
	//gtk_widget_hide(GTK_WIDGET(layout_login));
	initAddressCtrl(builder, GTK_WIDGET(layout_mir), 40);
	initUserCtrl(builder, GTK_WIDGET(layout_ctrl));
	initLoginCtrl(builder, GTK_WIDGET(layout_login));
	if (g_interfacetype == 2)
		init_logo_ctrl(builder, GTK_WIDGET(layout_logo), nwidth, 100);
	else
		init_logo_ctrl(builder, GTK_WIDGET(layout_logo), nwidth, 50);
	if (g_selectProto == 0)
		gtk_widget_hide(GTK_WIDGET(layout_mir));
	if (g_selectProto == 1) //mirsoft
		gtk_widget_show(GTK_WIDGET(layout_mir));
}


int init_ctrl_data(GtkBuilder *builder)
{
	//printf(" login window wwwwwwww  user: %s\n", serverInfo.szUser);
	GObject *image_rempass;
	GObject *entry_pass;
	GObject *comboboxtext_entry;
	GObject *image_auto;
	image_rempass = gtk_builder_get_object(builder, "image_rempass");
	image_auto = gtk_builder_get_object(builder, "image_auto");
	entry_pass = gtk_builder_get_object(builder, "entry_pass");
	comboboxtext_entry = gtk_builder_get_object(builder, "comboboxtext_entry");
	GObject *comboboxtext_user = gtk_builder_get_object (builder, "comboboxtext_user");
	struct LoginInfo info = {"", "", "", "", 3389, 0, 0, 0};
	if (g_selectProto == 0) //ShenCloud
	{
		GetLoginInfo(&info);
		if (strcmp(info.user, "") == 0)
			strcpy(info.user, "admin@internal");
		gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxtext_user, 0, info.user);
		gtk_entry_set_text(GTK_ENTRY(comboboxtext_entry), info.user);
		g_checkrepass = info.repass;
		g_checkautologin = info.autologin;
	}
	if (g_selectProto == 1 || g_selectProto == 3) //mirsoft
	{
		g_checkrepass = 0;
		if (g_selectProto == 1)
			GetMirLoginInfo(&info);
		if (g_selectProto == 3)
			GetVmareLoginInfo(&info);
		if (info.port == 0)
			info.port = 3389;
		GObject *entry_port;
		char szTmp[MAX_BUFF_SIZE] = {0};
		sprintf(szTmp, "%d", info.port);
		entry_port = gtk_builder_get_object(builder, "entry_port");
		gtk_entry_set_text(GTK_ENTRY(entry_port), szTmp);
		if (strcmp(info.user, "") >= 0)
		{
			gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxtext_user, 0, info.user);
			gtk_entry_set_text(GTK_ENTRY(comboboxtext_entry), info.user);
		}
		g_checkrepass = info.repass;
	}
	if (g_checkrepass == 1)
	{
		 gtk_entry_set_text(GTK_ENTRY(entry_pass), info.pass);
		 gtk_image_set_from_pixbuf(GTK_IMAGE(image_rempass), g_checkPressimage);
	}
	else if (g_checkrepass == 0)
	{
		 gtk_image_set_from_pixbuf(GTK_IMAGE(image_rempass), g_checkNorimage);
	}
	//161104
	if (g_checkautologin == 1)
	{
		 gtk_image_set_from_pixbuf(GTK_IMAGE(image_auto), g_checkPressimage);
	}
	else if (g_checkautologin == 0)
	{
		 gtk_image_set_from_pixbuf(GTK_IMAGE(image_auto), g_checkNorimage);
	}
	return 0;
}

static void signal_ctrlevent(GtkBuilder *builder)
{
	GObject *btn_logout;
	GObject *btn_shutdown;
	GObject *btn_login;
	GObject *image_btnlogin;
	GObject *image_loginout;
	GObject *image_shutdown;
	btn_logout = gtk_builder_get_object (builder, "btn_logout");
	btn_shutdown = gtk_builder_get_object (builder, "btn_shutdown");
	btn_login = gtk_builder_get_object (builder, "btn_login");
	image_btnlogin = gtk_builder_get_object (builder, "image_btnlogin");
	image_loginout = gtk_builder_get_object (builder, "image_loginout");
	image_shutdown = gtk_builder_get_object (builder, "image_shutdown");
	g_signal_connect(G_OBJECT(btn_login), "enter", G_CALLBACK(on_btn_login_enter), (GtkWidget *)image_btnlogin);
	g_signal_connect(G_OBJECT(btn_login), "pressed", G_CALLBACK(on_btn_login_pressed), (GtkWidget *)image_btnlogin);
    g_signal_connect(G_OBJECT(btn_login), "leave", G_CALLBACK(on_btn_login_leave), (GtkWidget *)image_btnlogin);
	//
	g_signal_connect(G_OBJECT(btn_logout), "enter", G_CALLBACK(on_btn_logout_enter), (GtkWidget *)image_loginout);
	g_signal_connect(G_OBJECT(btn_logout), "pressed", G_CALLBACK(on_btn_logout_pressed), (GtkWidget *)image_loginout);
    g_signal_connect(G_OBJECT(btn_logout), "leave", G_CALLBACK(on_btn_logout_leave), (GtkWidget *)image_loginout);
    //
	g_signal_connect(G_OBJECT(btn_shutdown), "enter", G_CALLBACK(on_btn_shutdown_enter), (GtkWidget *)image_shutdown);
	g_signal_connect(G_OBJECT(btn_shutdown), "pressed", G_CALLBACK(on_btn_shutdown_pressed), (GtkWidget *)image_shutdown);
    g_signal_connect(G_OBJECT(btn_shutdown), "leave", G_CALLBACK(on_btn_shutdown_leave), (GtkWidget *)image_shutdown);

	//checkbutton rember pass
	GObject *eventbox_rempass;
	GObject *eventbox_auto;
	eventbox_rempass = gtk_builder_get_object (builder, "eventbox_rempass");
	gtk_widget_set_events((GtkWidget *)eventbox_rempass, GDK_BUTTON_PRESS_MASK);
	eventbox_auto = gtk_builder_get_object (builder, "eventbox_auto");
	gtk_widget_set_events((GtkWidget *)eventbox_auto, GDK_BUTTON_PRESS_MASK);
	//checkbutton image signal
    GObject *image_rempass;
    GObject *image_auto;
    image_rempass = gtk_builder_get_object (builder, "image_rempass");
	image_auto = gtk_builder_get_object (builder, "image_auto");
    g_signal_connect (G_OBJECT (eventbox_rempass), "button_press_event",
                    G_CALLBACK (checkbutton_repass_press_callback), (GtkWidget *)image_rempass);
	g_signal_connect (G_OBJECT (eventbox_auto), "button_press_event",
                    G_CALLBACK (checkbutton_autologin_press_callback), (GtkWidget *)image_auto);
}

static void init_ctrlbtn_pos(GtkBuilder *builder, GtkLayout * widget2, GtkLayout * widget4, int layout2_height, int layout4_height)
{
	//cal subctrl button
	GdkScreen* screen;
    screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	int piclogout_width = 0;
	int piclogout_height = 0;
	int picexit_width = 0;
	int picexit_height = 0;
	GObject *btn_logout;
	GObject *btn_shutdown;
	btn_logout = gtk_builder_get_object (builder, "btn_logout");
	btn_shutdown = gtk_builder_get_object (builder, "btn_shutdown");
	int nspace_width = 40;
	if (scr_width == 1024 && scr_height == 768)
	{
		gdk_pixbuf_get_file_info("images2/1024x768/loginout_nor.png", &piclogout_width, &piclogout_height);
		gdk_pixbuf_get_file_info("images2/1024x768/exit_nor.png", &picexit_width, &picexit_height);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		gdk_pixbuf_get_file_info(IMAGE_BTN_PREV_NOR, &piclogout_width, &piclogout_height);
		gdk_pixbuf_get_file_info(IMAGE_BTN_SHUTDWON_NOR, &picexit_width, &picexit_height);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  ||
	  (scr_width == 1600 && scr_height == 1080))
	{
		gdk_pixbuf_get_file_info("images2/1440x900/loginout_nor.png", &piclogout_width, &piclogout_height);
		gdk_pixbuf_get_file_info("images2/1440x900/exit_nor.png", &picexit_width, &picexit_height);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	  (scr_width == 1280 && scr_height == 1024) )
	{
		//do code
		gdk_pixbuf_get_file_info("images2/1280x720/loginout_nor.png", &piclogout_width, &piclogout_height);
		gdk_pixbuf_get_file_info("images2/1280x720/exit_nor.png", &picexit_width, &picexit_height);
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		//do code
		gdk_pixbuf_get_file_info("images2/1366x768/loginout_nor.png", &piclogout_width, &piclogout_height);
		gdk_pixbuf_get_file_info("images2/1366x768/exit_nor.png", &picexit_width, &picexit_height);
	}
	gtk_widget_set_size_request((GtkWidget *)btn_shutdown, picexit_width, picexit_height);
	gtk_widget_set_size_request((GtkWidget *)btn_logout, piclogout_width, piclogout_height);
	//exit button
	int x = scr_width - nspace_width - picexit_width;
	int y = 0;
	if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
		y = (layout4_height - picexit_height)/2 + 30;
	else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
		y = (layout4_height - picexit_height)/2 + 30;
	else
		y = (layout4_height - picexit_height)/2 + 20;

	gtk_layout_move((GtkLayout *)widget4, GTK_WIDGET(btn_shutdown), x, y);
	//logout buttion
	x = scr_width - nspace_width - piclogout_width;
	y = (layout2_height - piclogout_height)/2;
	gtk_layout_move((GtkLayout *)widget2, GTK_WIDGET(btn_logout), x, y);

}

static void  on_entry_ip_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
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

static void  on_entry_port_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
{
    if (new_text[0]  > '9'  || new_text [0] < '0')
    {
        new_text[0] = '\0';
   	}
    return;
}

static void init_entry_event()
{
	GObject *entry_ip;
	GObject *entry_port;
	entry_ip = gtk_builder_get_object (g_builder, "entry_ip");
    entry_port = gtk_builder_get_object (g_builder, "entry_port");
	g_signal_connect(G_OBJECT(entry_ip), "insert-text", G_CALLBACK(on_entry_ip_insert_text), NULL);
	g_signal_connect(G_OBJECT(entry_port), "insert-text", G_CALLBACK(on_entry_port_insert_text), NULL);
}

void SY_loginwindow_main()
{
	if (showloginwindow == 1)
	   return;
	showloginwindow = 1;
	g_loginExit = 0;
	showSyMsgDlg11 = 0;
	GdkScreen* screen;
	GtkBuilder *builder;
	GObject *window;
	GError *error = NULL;
    screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	int scr_layout3_height = scr_height - scr_layout2_height - scr_layout4_height; //The height of the four circle area
	builder = gtk_builder_new();
	int nRet = gtk_builder_add_from_file (builder, "loginwindow.glade", &error);
	window = gtk_builder_get_object (builder, "window1");
	gtk_widget_set_app_paintable(GTK_WIDGET(window), TRUE);
	g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK (on_draw_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit_login), NULL);
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(on_key_press), NULL);
	gtk_widget_set_size_request(GTK_WIDGET(window), scr_width, scr_height);
	g_builder = builder;
	g_window = window;
	g_checkrepass = 0;
	initOvirtUrl();
	loadcss();
	create_surfaces();
	GtkAllocation allocat;
	GObject *layout1;
	layout1 = gtk_builder_get_object(builder, "layout1");
	GObject *layout2;
	layout2 = gtk_builder_get_object(builder, "layout2");
	GObject *layout3;
	layout3 = gtk_builder_get_object(builder, "layout3");
	//cal layout1
	GdkRGBA color1;
	gdk_rgba_parse(&color1, "#FF7F24");
	//gtk_widget_override_background_color(GTK_WIDGET(layout1), GTK_STATE_FLAG_NORMAL, &color1);
	gtk_widget_set_size_request(GTK_WIDGET(layout1), scr_width, scr_height);
	//cal layout2;
	GdkRGBA color2;
	gdk_rgba_parse(&color2, "#5F9EA0");
	//gtk_widget_override_background_color(GTK_WIDGET(layout2), GTK_STATE_FLAG_NORMAL, &color2);
	gtk_widget_set_size_request(GTK_WIDGET(layout2), scr_width, scr_layout2_height);
	//cal layout3;
	GdkRGBA color3;
	gdk_rgba_parse(&color3, "#DDDDDD");
	//gtk_widget_override_background_color(GTK_WIDGET(layout3), GTK_STATE_FLAG_NORMAL, &color3);
	gtk_widget_set_size_request(GTK_WIDGET(layout3), scr_width, scr_layout3_height);
	//cal layout4
	GObject *layout4;
	GdkRGBA color;
	gdk_rgba_parse(&color, "#BBFFFF");
	layout4 = gtk_builder_get_object (builder, "layout4");
	gtk_widget_get_allocation(GTK_WIDGET(layout4), &allocat);
	//gtk_widget_override_background_color(GTK_WIDGET(layout4), GTK_STATE_FLAG_NORMAL, &color);
	gtk_widget_set_size_request(GTK_WIDGET(layout4), scr_width, scr_layout4_height);
	GValue a = G_VALUE_INIT;
	g_value_init (&a, G_TYPE_INT);
	g_value_set_int (&a, scr_height - scr_layout4_height);
	gtk_container_child_set_property(GTK_CONTAINER(layout1), GTK_WIDGET(layout4), "y", &a);
//	//cal sub ctrl
//	int logout_width = 80;
//	int logout_height = 30;
//	int exit_width = 80;
//	int exit_height = 30;
//	GObject *btn_logout;
//	btn_logout = gtk_builder_get_object (builder, "btn_logout");
//	get_logoutctrl(scr_width, scr_height, &logout_width, &logout_height);
//	gtk_widget_set_size_request(GTK_WIDGET(btn_logout), logout_width, logout_height);
//	gtk_layout_put((GtkLayout *)layout2, GTK_WIDGET(btn_logout), scr_width - btn_inter_right - logout_width, scr_layout4_height - btn_inter_bottom - logout_height);

//	GObject *btn_shutdown;
//	btn_shutdown = gtk_builder_get_object (builder, "btn_shutdown");
//	get_exitctrl(scr_width, scr_height, &exit_width, &exit_height);
//	gtk_widget_set_size_request(GTK_WIDGET(btn_shutdown), exit_width, exit_height);
//	printf("ddddddd   exit_width = %d, exit_height = %d .\n", exit_width, exit_height);
//	gtk_layout_put((GtkLayout *)layout4, GTK_WIDGET(btn_shutdown), scr_width - btn_inter_right - exit_width, scr_layout4_height - btn_inter_bottom - exit_height);
	init_ctrlbtn_pos(builder, layout2, layout4, scr_layout2_height, scr_layout4_height);
	SetCtrlPos(builder, scr_layout3_height);
	loadImage();
	init_ctrl_image(builder);
	init_ctrl_data(builder);
	signal_ctrlevent(builder);
	init_entry_event();
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("images2/logo.png"));
	gtk_window_fullscreen(GTK_WINDOW(window));
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE); /* hide the title bar and the boder */
	//gtk_widget_show_all((GtkWidget *)window);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
     gtk_window_set_modal (GTK_WINDOW (window), TRUE);
	 gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_window_set_transient_for(GTK_WINDOW(window), g_mainWindow);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
	gtk_main();
    g_object_unref(G_OBJECT(builder));
	destroy_surfaces();
    showloginwindow = 0;
}

//test
//int main(int argc, char *argv[])
//{
//	gtk_init (&argc, &argv);
//	return 0;
//}
