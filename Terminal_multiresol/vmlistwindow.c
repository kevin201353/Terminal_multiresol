/***********
//filename: vmlistwindow.c
//author: zhaosenhua
//version: 1.0
//funciton list: 虚拟机列表
//description: ShenCloud 虚拟终端登录界面主程序，负责启动登录界面
//Date: 2016/6/21
**************/
#include <math.h>
#include "global.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include "tcpclient.h"
#include "type.h"


enum {
    NAME = 0,
    NAME_1,
    OS,
    STATUS,
    VCPU,
    MEMORY,
    IP,
    USB,
    COLUMNS
};

#define GET_ARRAY_LEN(array,len){len = (sizeof(array) / sizeof(array[0]));}

#define IMAGE_TREE_VMICON_NOR    	"images2/vm_icon.png"
#define IMAGE_BTN_START_NOR    		"images2/start.png"
#define IMAGE_BTN_START_PRES   		"images2/start_press.png"
#define IMAGE_BTN_CLOSE_NOR    		"images2/close.png"
#define IMAGE_BTN_CLOSE_PRES   		"images2/close_press.png"
#define IMAGE_BTN_DESKTOP_NOR  		"images2/desktop.png"
#define IMAGE_BTN_DESKTOP_PRES  	"images2/desktop_press.png"
#define IMAGE_BTN_SLEEP_NOR    		"images2/sleep.png"
#define IMAGE_BTN_SLEEP_PRES   		"images2/sleep_press.png"
#define IMAGE_BTN_EXIT_NOR     		"images2/exit_nor.png"
#define IMAGE_BTN_EXIT_PRES    		"images2/exit_press.png"
#define IMAGE_BTN_LOGINOUT_NOR  	"images2/loginout_nor.png"
#define IMAGE_BTN_LOGINOUT_PRES  	"images2/loginout_press.png"
#define IMAGE_VMLISTWINDOW_BACKGROUD  "images2/vmlistwindow.png"


GdkPixbuf *g_vmIconPix;
GtkListStore *g_store;
GtkTreeView *g_treeview;
volatile unsigned short g_sUpdateVmStatus = 0;
int showvmlistwindow = 0;
struct Vms_Update {
    char szvmid[MAX_BUFF_SIZE];
    unsigned short state;
    GtkTreeIter iter;
};
char g_vmName[MAX_BUFF_SIZE] = {0};
char g_shellcmd[MAX_BUFF_SIZE] = {0};
char g_szUser[MAX_BUFF_SIZE] = {0};  //login user
char g_szPass[MAX_BUFF_SIZE] = {0};  //login pass
char g_szVMid[MAX_BUFF_SIZE] = {0};
static int g_nstate = 0;
struct Vms_Update g_upVms[MAX_BUFF_VM];

static int columnwidth_array[9] = {80,200,200,100,150,100,100,200,200};

GdkPixbuf *g_restartPress;
GdkPixbuf *g_restartNor;
GdkPixbuf *g_startPress;
GdkPixbuf *g_startNor;
GdkPixbuf *g_closePress;
GdkPixbuf *g_closeNor;
GdkPixbuf *g_desktopPress;
GdkPixbuf *g_desktopNor;
GdkPixbuf *g_sleepPress;
GdkPixbuf *g_sleepNor;
GdkPixbuf *g_exitPress;
GdkPixbuf *g_exitNor;
GdkPixbuf *g_loginoutPress;
GdkPixbuf *g_loginoutNor;
static GObject *g_window = NULL;
struct itimerval itv;
unsigned int g_vmsCount = 0;


static void *thrd_func(void *arg);
static pthread_t tid;

void *thrd_checkstate(void *arg);
static pthread_t tid_state;

unsigned short g_exitvm = 0;
static cairo_surface_t *surface_vmlistwindow;

static void get_string_width(char *str, int* width, int* height);
static void init_vmctrlbtn_pos(GtkBuilder *builder, GtkWidget* widget, int lay_width, int lay_height);

extern void setctrlFont(GtkWidget * widget, int nsize);
extern void shell_exec(char *cmd);
static volatile int g_flushState = 0;
static void connectVm22();

static float g_scr_old_width = 1920.0;
static float g_scr_old_height = 1080.0;

//
static GtkTreeIter * g_iter = NULL;
static void initUpdateVms();
static void adddata();
GObject *g_label_login;
extern int detect_process();


void set_vm_network_type()
{
    //network type
    if (showvmlistwindow == 1)
    {
	    char szTmp[MAX_BUFF_SIZE] = {0};
	    sprintf(szTmp, "当前登录用户: %s", g_szUser);
		char szMsg2[1024*2] = {0};
		strcpy(szMsg2, g_MsgCall.szMsg);
		strcat(szMsg2, "  ");
		strcat(szMsg2, szTmp);
	    gtk_label_set_text(GTK_LABEL(g_label_login), szMsg2);
		//vm tree clear
		gtk_list_store_clear(g_store);
		SY_FreeVmsList();
		//reset login outer net or inter net
		dlnet_login();
		g_vmsCount = 0;
		initUpdateVms();
	    adddata();
		//reply to java
		char szMsg[BUF_MSG_LEN]= {0};
		strcpy(szMsg, "\napagentui.ThinviewNetworkTypeChangeReply####{\"success\":true}\n");
		write(1, szMsg, strlen(szMsg));
    }
} 

void exit_vimlist_win()
{
	g_sUpdateVmStatus = 0;
	g_exitvm = 1;
	showvmlistwindow = 0;
	cleanVms();
	if (g_window != NULL)
	{
	    gtk_widget_destroy((GtkWidget *)g_window);
		g_window = NULL;
	    gtk_main_quit();
	}
}

static int column_len()
{
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);	

	if (scr_width == 1024 && scr_height == 768)
	{
		columnwidth_array[0] = 60;
		columnwidth_array[1] = 110;
		columnwidth_array[2] = 120;
		columnwidth_array[3] = 60;
		columnwidth_array[4] = 60;
		columnwidth_array[5] = 60;
		columnwidth_array[6] = 60;
		columnwidth_array[7] = 120;
		columnwidth_array[8] = 90;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		columnwidth_array[0] = 80;
		columnwidth_array[1] = 200;
		columnwidth_array[2] = 200;
		columnwidth_array[3] = 100;
		columnwidth_array[4] = 150;
		columnwidth_array[5] = 100;
		columnwidth_array[6] = 100;
		columnwidth_array[7] = 200;
		columnwidth_array[8] = 200;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 ) )
	{
		if ((scr_width == 1600 && scr_height == 900))
		{
			columnwidth_array[0] = 80;
			columnwidth_array[1] = 200;
			columnwidth_array[2] = 200;
			columnwidth_array[3] = 100;
			columnwidth_array[4] = 150;
			columnwidth_array[5] = 100;
			columnwidth_array[6] = 100;
			columnwidth_array[7] = 200;
			columnwidth_array[8] = 200;
		}else
		{
			columnwidth_array[0] = 80;
			columnwidth_array[1] = 160;
			columnwidth_array[2] = 120;
			columnwidth_array[3] = 90;
			columnwidth_array[4] = 120;
			columnwidth_array[5] = 80;
			columnwidth_array[6] = 90;
			columnwidth_array[7] = 160;
			columnwidth_array[8] = 60;
		}
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	   (scr_width == 1280 && scr_height == 1024))
	{
		columnwidth_array[0] = 80;
		columnwidth_array[1] = 140;
		columnwidth_array[2] = 140;
		columnwidth_array[3] = 90;
		columnwidth_array[4] = 90;
		columnwidth_array[5] = 90;
		columnwidth_array[6] = 90;
		columnwidth_array[7] = 140;
		columnwidth_array[8] = 80;
	}else if ((scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) ||
		 (scr_width == 1360 && scr_height == 768))
	{
		columnwidth_array[0] = 80;
		columnwidth_array[1] = 140;
		columnwidth_array[2] = 140;
		columnwidth_array[3] = 90;
		columnwidth_array[4] = 90;
		columnwidth_array[5] = 90;
		columnwidth_array[6] = 90;
		columnwidth_array[7] = 140;
		columnwidth_array[8] = 80;
	}else if ((scr_width == 1680 && scr_height == 1050))
	{
		columnwidth_array[0] = 80;
		columnwidth_array[1] = 200;
		columnwidth_array[2] = 200;
		columnwidth_array[3] = 100;
		columnwidth_array[4] = 150;
		columnwidth_array[5] = 100;
		columnwidth_array[6] = 100;
		columnwidth_array[7] = 200;
		columnwidth_array[8] = 200;
	}
}

static void init_ctrl_posit(GtkBuilder *builder)
{
    GObject *grid2;
	GObject *layout1;
    GObject *layout3;
    GObject *layout4;
	GObject *treeview_vm;
    grid2 = gtk_builder_get_object (builder, "grid2");
    layout3 = gtk_builder_get_object (builder, "layout3");
    layout4 = gtk_builder_get_object (builder, "layout4");
	layout1 = gtk_builder_get_object (builder, "layout1");
	treeview_vm = gtk_builder_get_object (builder, "treeview_vm");
	GtkWidget *scrolled_win;
	scrolled_win = gtk_builder_get_object(builder, "scrolledwindow1");

	GdkScreen* screen;
	screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);

	float factor_x = (float)scr_width/g_scr_old_width;
	float factor_y = (float)scr_height/g_scr_old_height;
	
	gtk_widget_set_size_request(GTK_WIDGET(g_window), scr_width, scr_height);
	int layout1_width = 0;
	int layout1_height = 0;
	int grid2_width = 0;
	int grid2_height = 0;
	int layout3_width = 0;
	int layout3_height = 0;
	int layout4_width = 0;
	int layout4_height = 0;
	int treeview_vm_width = 0;
	int treeview_vm_height = 0;

	if (scr_width == 1024 && scr_height == 768)
	{
        layout1_width = scr_width;
		layout1_height = 42;
		grid2_width = scr_width;
		grid2_height = scr_height - layout1_height;
		layout3_width = 750;
		layout3_height = scr_height - layout1_height;
		layout4_width = scr_width - layout3_width;
		layout4_height = scr_height - layout1_height;
		treeview_vm_width = layout3_width;
		treeview_vm_height = layout3_height;

	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		layout1_width = scr_width;
		layout1_height = 80;
		grid2_width = scr_width;
		grid2_height = scr_height - layout1_height;
		layout3_width = 1500;
		layout3_height = scr_height - layout1_height;
		layout4_width = scr_width - layout3_width;
		layout4_height = scr_height - layout1_height;
		treeview_vm_width = layout3_width;
		treeview_vm_height = layout3_height;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   ||
	  (scr_width == 1600 && scr_height == 1080))
	{
		layout1_width = scr_width;
		layout1_height = 60;
		grid2_width = scr_width;
		grid2_height = scr_height - layout1_height;
		if ((scr_width == 1600 && scr_height == 900))
			layout3_width = 1200;
		else
			layout3_width = 1060;
		layout3_height = scr_height - layout1_height;
		layout4_width = scr_width - layout3_width;
		layout4_height = scr_height - layout1_height;
		treeview_vm_width = layout3_width;
		treeview_vm_height = layout3_height;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	  (scr_width == 1280 && scr_height == 1024))
	{
		layout1_width = scr_width;
		layout1_height = 42;
		grid2_width = scr_width;
		grid2_height = scr_height - layout1_height;
		layout3_width = 980;
		layout3_height = scr_height - layout1_height;
		layout4_width = scr_width - layout3_width;
		layout4_height = scr_height - layout1_height;
		treeview_vm_width = layout3_width;
		treeview_vm_height = layout3_height;
	}else if ((scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) ||
		(scr_width == 1360 && scr_height == 768))
	{
		layout1_width = scr_width;
		layout1_height = 50;
		grid2_width = scr_width;
		grid2_height = scr_height - layout1_height;
		layout3_width = 1000;
		layout3_height = scr_height - layout1_height;
		layout4_width = scr_width - layout3_width;
		layout4_height = scr_height - layout1_height;
		treeview_vm_width = layout3_width;
		treeview_vm_height = layout3_height;
	}else if ((scr_width == 1680 && scr_height == 1050))
	{
		layout1_width = scr_width;
		layout1_height = 80;
		grid2_width = scr_width;
		grid2_height = scr_height - layout1_height;
		layout3_width = 1200;
		layout3_height = scr_height - layout1_height;
		layout4_width = scr_width - layout3_width;
		layout4_height = scr_height - layout1_height;
		treeview_vm_width = layout3_width;
		treeview_vm_height = layout3_height;
	}
	gtk_widget_set_size_request(GTK_WIDGET(grid2), grid2_width, grid2_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout1), layout1_width, layout1_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout4), layout4_width, layout4_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout3), layout3_width, layout3_height);
	gtk_widget_set_size_request(GTK_WIDGET(treeview_vm),layout3_width, layout3_height);
	gtk_widget_set_size_request(GTK_WIDGET(scrolled_win), layout3_width, layout3_height);

	GtkAdjustment *adjust = gtk_adjustment_new(1, 2, layout3_height - 2, 5, 5, 50);
	gtk_scrolled_window_set_vadjustment((GtkScrolledWindow *)scrolled_win, adjust);
	//loginout, exit buttion
	GObject *button_loginout;
	GObject *button_exit;
	button_loginout = gtk_builder_get_object (builder, "button_loginout");
	button_exit = gtk_builder_get_object (builder, "button_exit");

	//get button pic width and height
	int x, y = 0;
	int ndelay = 30;
	int pic_width = 0;
	int pic_height = 0;
	int nspace_delay = 15;
	int login_user_width = 0;
	if (scr_width == 1024 && scr_height == 768)
	{
		gdk_pixbuf_get_file_info("images2/1024x768/loginout_nor.png", &pic_width, &pic_height);
		nspace_delay = 20;
		login_user_width = 30;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		gdk_pixbuf_get_file_info(IMAGE_BTN_LOGINOUT_NOR, &pic_width, &pic_height);
		nspace_delay = 30;
		login_user_width = 150;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   ||
	   (scr_width == 1600 && scr_height == 1080) )
	{
		gdk_pixbuf_get_file_info("images2/1440x900/loginout_nor.png", &pic_width, &pic_height);
		nspace_delay = 20;
		login_user_width = 60;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	  (scr_width == 1280 && scr_height == 1024) )
	{
		gdk_pixbuf_get_file_info("images2/1280x720/loginout_nor.png", &pic_width, &pic_height);
		nspace_delay = 20;
		login_user_width = 30;
	}else if ((scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		gdk_pixbuf_get_file_info("images2/1366x768/loginout_nor.png", &pic_width, &pic_height);
		nspace_delay = 20;
		login_user_width = 60;
	}else if ((scr_width == 1680 && scr_height == 1050))
	{
		gdk_pixbuf_get_file_info(IMAGE_BTN_LOGINOUT_NOR, &pic_width, &pic_height);
		nspace_delay = 30;
		login_user_width = 150;
	}
	LogInfo("init_ctrl_posit 000 layout1_width=%d, pic_width=%d, pic_height=%d.\n", layout1_width, pic_width, pic_height);
	x = layout1_width - pic_width - ndelay;
	y = (layout1_height - pic_height)/2;
	gtk_widget_set_size_request(GTK_WIDGET(button_loginout), pic_width, pic_height);
	gtk_widget_set_size_request(GTK_WIDGET(button_exit), pic_width, pic_height);
	gtk_layout_move((GtkLayout *)layout1, GTK_WIDGET(button_exit), x, y);
	x = layout1_width - (pic_width * 2) - ndelay - nspace_delay;
	gtk_layout_move((GtkLayout *)layout1, GTK_WIDGET(button_loginout), x, y);


	//add
	int width =0;
	int height = 0;
	GObject *label_login;
    label_login = gtk_builder_get_object (builder, "label_login");
	gtk_widget_get_size_request(GTK_WIDGET(label_login), width, height);
	LogInfo("vmlist window init_ctrl_posit, label_login old: factor_x=%f, width = %d.", factor_x, width); 
#ifdef DOUBLENET
	if (width == 0)
		width = 100;
	int expand = width * 2;
	gtk_widget_set_size_request(GTK_WIDGET(label_login), expand, height);
    x = layout1_width - (pic_width * 4) - ndelay*2 - nspace_delay - login_user_width - expand;
#else
	x = layout1_width - (pic_width * 4) - ndelay*2 - nspace_delay - login_user_width;
#endif
	x *= factor_x;
	y += 10;
	y *= factor_y;
	width *= factor_x;
	height *= factor_y;
	LogInfo("vmlist window init_ctrl_posit, label_login: x=%d, width = %d.", x, width);
	gtk_layout_move((GtkLayout *)layout1, GTK_WIDGET(label_login), x, y);
	init_vmctrlbtn_pos(builder, layout4, layout4_width, layout4_height);
}

static void create_surfaces()
{
	GdkScreen* screen;
	screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	if (scr_width == 1024 && scr_height == 768)
	{
		//surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1024x768/vmlistwindow.png");
		if (g_interfacetype == 2)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1024x768/vmlistwindow.png");
		else if (g_interfacetype == 0)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1024x768/vmlistwindow_sh.png");
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		//surface_vmlistwindow = cairo_image_surface_create_from_png (IMAGE_VMLISTWINDOW_BACKGROUD);
		if (g_interfacetype == 2)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/vmlistwindow.png");
		else if (g_interfacetype == 0)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/vmlistwindow_sh.png");
	}else if (scr_width == 1440 && scr_height == 900)
	{
		if (g_interfacetype == 2)
		{
			//surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1440x900/zdy/vmlistwindow.png");
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1440x900/vmlistwindow.png");
		}
		else if (g_interfacetype == 0)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1440x900/sh/vmlistwindow.png");
		//surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1440x900/vmlistwindow.png");
	}else if ((scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080))
	{
	    if (g_interfacetype == 2)
	    	{
			//surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1600x900/zdy/vmlistwindow.png");
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1600x900/vmlistwindow.png");
	    	}
		else if (g_interfacetype == 0)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1600x900/sh/vmlistwindow.png");

		//surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1600x900/vmlistwindow.png");
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	 (scr_width == 1280 && scr_height == 1024) )
	{
		if (g_interfacetype == 2)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1280x720/vmlistwindow.png");
		else
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1280x720/vmlistwindow_sh.png");
	}else if ((scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) ||
		 (scr_width == 1360 && scr_height == 768) )
	{
		if (g_interfacetype == 2)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1366x768/vmlistwindow.png");
		else
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1366x768/vmlistwindow_sh.png");
	}else if ((scr_width == 1680 && scr_height == 1050))
	{
		if (g_interfacetype == 2)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1680x1050/vmlistwindow.png");
		else if (g_interfacetype == 0)
			surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1680x1050/vmlistwindow.png");
	}
}

static void destroy_surfaces()
{
	cairo_surface_destroy (surface_vmlistwindow);
}

void cleanVms()
{
#if 0
    for (int i=0; i<MAX_BUFF_VM; i++)
    {
         memset(g_upVms[i].szvmid, 0, MAX_BUFF_SIZE * sizeof(char));
         g_upVms[i].state = 0;
    }
    g_vmsCount = 0;
#endif
}

void *thrd_func(void *arg)
{
	init_long_session();
    for(;;)
    {
      if (g_sUpdateVmStatus)
      {
          UpdateVmsStatus();
      }
      if (g_exitvm)
      {
         break;
      }
      sleep(3);
    }
	close_long_session();
    pthread_exit(NULL); //退出线程
}

static void initUpdateVms()
{
	for (int i=0; i<MAX_BUFF_VM; i++)
    {
   		memset(g_upVms[i].szvmid, 0, MAX_BUFF_SIZE);
    }
}

//增加要更新的虚拟机到列表
unsigned short AddUpdateVms(char * vmid)
{
    for (int i=0; i<MAX_BUFF_VM; i++)
    {
        if (strcmp(g_upVms[i].szvmid, vmid) == 0)
           return -1;
    }
    for (int i=0; i<MAX_BUFF_VM; i++)
    {
        if (strcmp(g_upVms[i].szvmid, "") == 0)
        {
            strcpy(g_upVms[i].szvmid, vmid);
            GtkTreeIter iter;
            GtkTreeSelection* select = gtk_tree_view_get_selection(g_treeview);
            GtkListStore *store;
            GtkTreeModel *model;
            store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(g_treeview)));
            model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_treeview));
            if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(select), &model, &iter))
            {
                //gtk_list_store_set(g_store, &iter, STATUS, (GValue *)szTmp, -1);
                g_upVms[i].iter = iter;
            }
            g_vmsCount++;
            break;
        }
    }

    int len = 0;
    //printf("Add update vms 3333333  len : %d\n", g_vmsCount);
    for (int i = 0; i < MAX_BUFF_VM; i++)
    {
       if (strcmp(g_upVms[i].szvmid, "") > 0)
       {
           //printf("Add update vms 3333333  vmid : %s\n", g_upVms[i].szvmid);
           len++;
       }
    }
    if (g_vmsCount != len)
      g_vmsCount = len;

    return 0;
}

void set_timer()
{
    itv.it_interval.tv_sec = 5;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 2;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, NULL);
}

void close_timer()
{
    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &itv, NULL);
}

static int count = 0;
void signal_handler(int m)
{
    if (g_sUpdateVmStatus)
        UpdateVmsStatus();
    if (count > 65534)
       count = 0;
    count ++;
    //printf("signal_handler count : %d\n", count);
}

void GetVmsId()
{
    list_for_each(plist, &head)
    {
        struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
        //printf("connect vms g_vmName find vms id = %s.\n", g_vmName);
        if (strcmp(node->val.name, g_vmName) == 0)
        {
            memset(g_szVMid, 0, sizeof(g_szVMid));
            //printf("vmlist window get vms id: %s\n", node->val.vmid);
            strcpy(g_szVMid, node->val.vmid);
		   g_nstate = node->val.status;
        }
    }
}

int UpdateVmsStatus()
{
	if (Ovirt_GetLongVms(ovirt_url, g_szUser, g_szPass) < 0)
	{
	  LogInfo("UpdateVmsStatus Ovirt get vms failed.\n");
	  return -1;
	}
	if (SY_GetVms() < 0)
	{
	    LogInfo("UpdateVmsStatus SY_GetVms failed.\n");
	    return -1;
	}
	int cur_vmsCount = 0;
	int nstate = 0;
	list_for_each(plist, &head)
	{
	    if (g_exitvm)
			break;
	    struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
		if (NULL != node)
		{
		   //LogInfo("UpdateVmsStatus, node->val.vmid: %s, g_upVms[%d].szvmid: %s.", node->val.vmid, i, g_upVms[i].szvmid);
		   	if (cur_vmsCount >= g_vmsCount)
		   	{
				break;
			}
	        if (strcmp(node->val.vmid, g_upVms[cur_vmsCount].szvmid) == 0)
	        {
	            //LogInfo("Debug: UpdateVmsStatus, vms status : %d , old nstate : %d.\n.", node->val.status, g_upVms[i].state);
	            char szTmp[20] = {0};
	            if (node->val.status != g_upVms[cur_vmsCount].state)
	            {
	                nstate = node->val.status;
					if (nstate == 1)
						strcpy(szTmp, "运行");
					else if (nstate == 0)
						strcpy(szTmp, "已关机");
					else if (nstate == 2)
						strcpy(szTmp, "已暂停");
					else if (nstate == 3)
						strcpy(szTmp, "正在启动");
					else if (nstate == 4)
						strcpy(szTmp, "正在关机");
					else if (nstate == 7)
						strcpy(szTmp, "正在重启");
					else if (nstate == 8)
						strcpy(szTmp, "初始化中");
					else if (nstate == 10)
						strcpy(szTmp, "正在迁移");
					else if (nstate == 11)
						strcpy(szTmp, "错误");
					else if (nstate == 12)
						strcpy(szTmp, "镜像锁定");
					else if (nstate == 13)
						strcpy(szTmp, "非法镜像");
	                gtk_list_store_set(g_store, &g_upVms[cur_vmsCount].iter, STATUS, (GValue *)szTmp, -1);
	                g_upVms[cur_vmsCount].state = node->val.status;
	            }//if
	            gtk_list_store_set(g_store, &g_upVms[cur_vmsCount].iter, IP, (GValue *)node->val.ip, -1);
	        }
			cur_vmsCount++;
		}
	} //list
    return 0;
}

//更新所有虚拟机的状态
//目前只做虚拟机状态检测
//程序每隔5秒向服务器检测一次
void SetState(char *vmName, const unsigned short nstate);
int vmstate_update()
{
      LogInfo("debug: vmsstate_update thrd enter.\n");
      if (Ovirt_GetVmsTmp(ovirt_url, g_szUser, g_szPass) < 0)
      {
          LogInfo("debug: vmsstate_update: get vms failed.\n");
          return -1;
      }
      if (SY_GetVms2() < 0)
      {
          LogInfo("debug: vmsstate_update: get vms xml data failed.\n");
          return -1;
      }
      LogInfo("debug: vmsstate_update thrd enter 1111.\n");
      if (g_vmsComCount <= 0)
      {
          LogInfo("debug: vmsstate_update vms arrary is == null .\n");
          return -1;
      }
	  int cur_vmscount = 0;
      list_for_each(plist, &head)
      {
            if (g_exitvm)
				break;
			if (cur_vmscount >= g_vmsComCount)
				break;
            struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
			if (NULL != node)
			{
	            if (strcmp(node->val.vmid, g_vmsComUpdate[cur_vmscount].vmid) == 0 /*&&
	                 strcmp(node->val.name, g_vmsComUpdate[cur_vmscount].name) == 0*/)
	            {
	                LogInfo("debug: vms status : %d , nstate : %d.\n", node->val.status, g_vmsComUpdate[cur_vmscount].status);
	                if (node->val.status != g_vmsComUpdate[cur_vmscount].status)
	                {
	                    LogInfo("debug: vmlist window UpdateVmsStatus vms name: %s, vms old status : %d , new nstate : %d.\n", node->val.name, node->val.status, g_vmsComUpdate[cur_vmscount].status);
	                    SetState(node->val.name, g_vmsComUpdate[cur_vmscount].status);
	                    node->val.status = g_vmsComUpdate[cur_vmscount].status;
	                }
	            }
			}
			cur_vmscount++;
      }//list
      g_flushState = 0;
      return 0;
}

void SetState(char *vmName, const unsigned short nstate)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkListStore *store;
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(g_treeview));
    gtk_tree_model_get_iter_first(model, &iter);//迭代器定位到第一行
    store = GTK_LIST_STORE(model);
    char* pitem;
    char szTmp[MAX_BUFF_SIZE] = {0};
	if (nstate == 1)
	 strcpy(szTmp, "运行");
	else if (nstate == 0)
	 strcpy(szTmp, "已关机");
	else if (nstate == 2)
	 strcpy(szTmp, "已暂停");
	else if (nstate == 3)
	 strcpy(szTmp, "正在启动");
	else if (nstate == 4)
	 strcpy(szTmp, "正在关机");
	else if (nstate == 7)
	 strcpy(szTmp, "正在重启");
	else if (nstate == 8)
	 strcpy(szTmp, "初始化中");
	else if (nstate == 10)
	 strcpy(szTmp, "正在迁移");
	else if (nstate == 11)
	 strcpy(szTmp, "错误");
	else if (nstate == 12)
	 strcpy(szTmp, "镜像锁定");
	else if (nstate == 13)
	 strcpy(szTmp, "非法镜像");
    //遍历虚拟机列表所有行
    do {
         gtk_tree_model_get (model, &iter, NAME_1, &pitem, -1);
         if (strcmp(pitem, vmName) == 0)
         {
            gtk_list_store_set(store, &iter, STATUS, (GValue *)szTmp, -1);
         }
    } while(gtk_tree_model_iter_next(model, &iter));
}

void connectVms()
{
    //printf("$$$$$$$　connectVms enter. \n");
    list_for_each(plist, &head)
    {
        struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
        //printf("connect vms g_vmName 000 = %s.\n", g_vmName);
         char szTicket[MAX_BUFF_SIZE] = {0};
        if (strcmp(node->val.name, g_vmName) == 0)
        {
        	SetSymsgContextVm(LOGIN_STATUS_CONNECTING);
            LogInfo("Debug: connectVms g_szUser: = %s,  g_szPass = %s.\n", g_szUser, g_szPass);
		   if (node->val.status != 0)
		   {
		   #if 0
				if ( Ovirt_GetVmTicket(ovirt_url, g_szUser, g_szPass, node->val.vmid) < 0)
				{
					SetSymsgContext(SY_OVIRT_GETVMSTICKET_FAILED);
					return;
				}
				if ( SY_GetVmsTicket(szTicket) < 0)
				{
					SetSymsgContext(SY_OVIRT_GETVMSTICKET_FAILED);
					return;
				}
		  #endif
				//find vm
#ifdef DEMONMODE
				struct ServerInfo stinfo;
				GetServerInfo2(&stinfo);
				if (stinfo.demon == 1)
					strcpy(node->val.ip, stinfo.szIP);
#endif
				if (strcmp(szTicket, "") == 0)
				{
					strcpy(szTicket, "abc");
				}
				sprintf(g_shellcmd, "spicy -h %s -p %d -w %s -f >> %s", node->val.ip, node->val.port, szTicket, "/var/log/shencloud/spicy.log 2>&1");
				LogInfo("Debug:vm list window connect vms : %s. \n", g_shellcmd);
				if (g_workflag == 1)
				{
					strcpy(report.uname, g_szUser);
					strcpy(report.vname, g_vmName);
					report.action = 2;
					send_data(report);
				}
				SetSymsgContextVm(LOGIN_SUCCESSED);
				//shell_exec(g_shellcmd);
				if (g_workflag == 1)
				{
					wirte_conflag_data("/tmp/syp_reconnect", g_shellcmd);
					wirte_conflag_data("/tmp/host", node->val.ip);
				}
				//int nRet = system(g_shellcmd);
				shell_exec(g_shellcmd);
//#ifdef  ARM
//				system("sudo rk3188_clean_display");
//#endif
				//thrd_monitor(g_shellcmd);
				//printf("Debug: vmlist connect vm system return = %d.\n", nRet);
				if (g_workflag == 1)
				{
					report.action = 3;
					send_data(report);
				}
				break;
		   	}else
		   	{
		   		SetSymsgContextVm(LOGIN_STATUS_VM_DESKTOP);
				SetSymsgContextVm(LOGIN_SUCCESSED);
			}
        }//if
    }//list
}

static void on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(g_treeview));
    switch(event->keyval)
    {
        case GDK_KEY_Return:
            {
             	//g_sUpdateVmStatus = 0;
				//connectVm22();
            }
            break;
	   case GDK_KEY_Up:
	   	  {
			gtk_tree_model_iter_previous (GTK_TREE_MODEL(g_store), &g_iter);
			gtk_tree_selection_select_iter (selection, &g_iter);
		  }
	      break;
	   case GDK_KEY_Down:
	   	 {
			gtk_tree_model_iter_next (GTK_TREE_MODEL(g_store), &g_iter);
			gtk_tree_selection_select_iter (selection, &g_iter);
	     }
	     break;
        default:
            break;
    }
}

static gboolean on_gtk_main_quit_vmlist(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    g_sUpdateVmStatus = 0;
    g_exitvm = 1;
    cleanVms();
	if (g_window != NULL)
	{
	    gtk_widget_destroy((GtkWidget *)g_window);
		g_window = NULL;
	    gtk_main_quit();
	}
    return TRUE;
}

static void  on_btn_start_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_startPress);
}

static void  on_btn_start_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_startNor);
}

static void  on_btn_close_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_closePress);
}

static void  on_btn_close_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_closeNor);
}

static void  on_btn_desktop_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_desktopPress);
}

static void  on_btn_desktop_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_desktopNor);
}

static void  on_btn_sleep_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_sleepPress);
}

static void  on_btn_sleep_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_sleepNor);
}


static void  on_btn_restart_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_restartPress);
}

static void  on_btn_restart_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_restartNor);
}

void  vm_oper_tips(int nflag)
{
	char szMsg[MAX_BUFF_SIZE] = {0};
	int nRet = 0;
	switch(nflag)
	{
		case 0:
			{
				Ovirt_ShutdownVms(ovirt_url, g_szUser, g_szPass, g_szVMid);
			}
			break;
		case 1:
			{
				Ovirt_StartVms(ovirt_url, g_szUser, g_szPass, g_szVMid);
			}
			break;
		case 2:
			{
				Ovirt_SuspendVms(ovirt_url, g_szUser, g_szPass, g_szVMid);
			}
			break;
		case 3:  //reboot
			{
				Ovirt_RebootVms(ovirt_url, g_szUser, g_szPass, g_szVMid);
			}
			break;
	}
	SY_GetPostResult(&nRet, szMsg);
	if (nRet < 0)
	{
		SYMsgDialog2(7, szMsg);
	}
}

//nflag:  1 ����  0  �ر�   2  ����
static void operate_vm(int nflag)
{
	int nstatus = 0;
	LogInfo("operate_vm nflag: %d, g_vmName: %s.", nflag, g_vmName);
	list_for_each(plist, &head)
	{
		struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
		if (strcmp(node->val.name, g_vmName) == 0)
		{
			nstatus = node->val.status;
			break;
		}
	}
	vm_oper_tips(nflag);
	g_sUpdateVmStatus = 1;
}

static void  on_btn_start_clicked(GtkButton *button,  gpointer   user_data)
{
   LogInfo("vml list window button start clicked, g_szUser = %s, g_szPass =%s.\n", g_szUser, g_szPass);
   operate_vm(1);
}

static void  on_btn_close_clicked(GtkButton *button,  gpointer   user_data)
{
   //printf("vml list window button close clicked.\n");
   operate_vm(0);
}

static void connectVm22()
{
    list_for_each(plist, &head)
    {
        struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
        char szTicket[MAX_BUFF_SIZE] = {0};
        if (strcmp(node->val.name, g_vmName) == 0)
        {
            LogInfo("Debug: connectVms g_szUser: = %s,  g_szPass = %s.\n", g_szUser, g_szPass);
		   if (node->val.status != 0)
		   {
		   #if 0
				if ( Ovirt_GetVmTicket(ovirt_url, g_szUser, g_szPass, node->val.vmid) < 0)
				{
					SYMsgDialog2(7, "获取连接虚拟机密码失败！");
					return;
				}
				if ( SY_GetVmsTicket(szTicket) < 0)
				{
					SYMsgDialog2(7, "获取连接虚拟机密码失败！");
					return;
				}
		   #endif
				//find vm
#ifdef DEMONMODE
				struct ServerInfo stinfo;
				GetServerInfo2(&stinfo);
				if (stinfo.demon == 1)
					strcpy(node->val.ip, stinfo.szIP);
#endif
				if (strcmp(szTicket, "") == 0)
				{
					strcpy(szTicket, "abc");
				}
				sprintf(g_shellcmd, "spicy -h %s -p %d -w %s -f > %s", node->val.ip, node->val.port, szTicket, "/var/log/shencloud/spicy.log 2>&1");
				LogInfo("Debug:vm list window connect vms : %s. \n", g_shellcmd);
				if (g_workflag == 1)
				{
					strcpy(report.uname, g_szUser);
					strcpy(report.vname, g_vmName);
					report.action = 2;
					send_data(report);
				}
				if (g_workflag == 1)
				{
					wirte_conflag_data("/tmp/syp_reconnect", g_shellcmd);
					wirte_conflag_data("/tmp/host", node->val.ip);
				}
#ifdef MEETING
				char szMsg[BUF_MSG_LEN]= {0};
				sprintf(szMsg, "\napagentui.ThinviewConnect####{\"vmid\":\"%s\",\"username\": \"%s\", \"vmName\":\"%s\"}\n", node->val.vmid, g_szUser, g_vmName);
				write(1, szMsg, strlen(szMsg));
#endif

				system(g_shellcmd);
 
#ifdef  ARM
				system("sudo hk-rk3188_clean_display");
#endif
				LogInfo("spicy end,  hk-rk3188_clean_display");

				if (g_workflag == 1)
				{
					report.action = 3;
					send_data(report);
				}
				g_sUpdateVmStatus = 1;
				break;
		   	}else
		   	{
		   		SYMsgDialog2(7, "连接失败!");
				g_sUpdateVmStatus = 1;
			}
        	}
	}
	LogInfo("Debug: connectVm22 end !!!");
}

void dlnet_vm_connect()
{
	g_sUpdateVmStatus = 0;

	if (strcmp(g_vmName, "") != 0)
	{
		if (g_nVmCount == 1)
		{
			//直接连接虚拟机
			list_for_each(plist, &head)
			{
				struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
				if (node != NULL)
				{
#ifdef DEMONMODE
					struct ServerInfo stinfo;
					GetServerInfo2(&stinfo);
					if (stinfo.demon == 1)
						strcpy(node->val.ip, stinfo.szIP);
#endif
#ifdef MEETING
					char szMsg[BUF_MSG_LEN]= {0};
					sprintf(szMsg, "\napagentui.ThinviewConnect####{\"vmid\":\"%s\",\"username\": \"%s\", \"vmName\":\"%s\"}\n", node->val.vmid, g_szUser, node->val.name);
					write(1, szMsg, strlen(szMsg));
#endif
					char szcmd[1024] = {0};
					sprintf(szcmd, "spicy -h %s -p %d -f > %s", node->val.ip, node->val.port, "/var/log/shencloud/spicy.log 2>&1");
					LogInfo(szcmd);
					system(szcmd);
					break;
				}
			}
		}else
		{
			connectVm22();
		}
	}
	g_sUpdateVmStatus = 1;
}

void *thrd_connect_vmdesktop(void *arg)
{
    connectVm22();
}


static void  on_btn_desktop_clicked(GtkButton *button,               gpointer user_data)
{
	g_sUpdateVmStatus = 0;
	if (detect_process() == 0)
	{
		if ( pthread_create(&tid_state, NULL, thrd_connect_vmdesktop, NULL) !=0 ) 
		{
			LogInfo("on_btn_desktop_clicked, Create checkstate thread error!\n");
		}
		LogInfo("on_btn_desktop_clicked end !!!!!!!");
	}
}

static void  on_btn_sleep_clicked(GtkButton *button,  gpointer   user_data)
{
}

static void  on_btn_reboot_clicked(GtkButton *button,  gpointer   user_data)
{
   operate_vm(3);
}

static void  on_btn_logout_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginoutPress);
   //主意： 程序中使用同一个session
   //退回到启动界面
   //用户登出事件
   	char szMsg[BUF_MSG_LEN]= {0};
	sprintf(szMsg, "\napagentui.ThinviewLogoutReply####{}\n");
	write(1, szMsg, strlen(szMsg));
	g_sUpdateVmStatus = 0;
	g_exitvm = 1;
	showvmlistwindow = 0;
	cleanVms();
	if (g_window != NULL)
	{
	    gtk_widget_destroy((GtkWidget *)g_window);
		g_window = NULL;
	    gtk_main_quit();
	}
}

static void  on_btn_logout_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginoutNor);
}

static void  on_btn_logout_enter(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginoutPress);
}

static void  on_btn_exit_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_exitPress);
   SYMsgDialog2(11, "您确定要关闭系统吗？");
}

static void  on_btn_exit_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_exitNor);
}

static void  on_btn_exit_enter(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_exitPress);
}

static void  on_vmlist_changed(GtkWidget *widget,  GtkTreeModel *model)
{
   //GtkTreeIter iter;
   char *value;
   if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &g_iter)) {
       gtk_tree_model_get(model, &g_iter, NAME_1, &value, -1);
       memset(g_vmName, 0, sizeof(g_vmName));
       memcpy(g_vmName, value, strlen(value));
       g_free(value);
       GetVmsId();
   }
}

//为登录主窗口绘制背景图
gboolean on_expose_vmlistevent (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    //g_print("vmlist on_expose_event start.\n");
    //LogInfo("debug: vmlist on_expose_event start.\n");
    GdkWindow *window;
    cairo_t *cr;
    window = gtk_widget_get_window(widget); //gtk3中GtkWidget中已经没有window成员了
    if (window == NULL)
		return FALSE;
    cr = gdk_cairo_create(window);
  //pattern1 = cairo_pattern_create_for_surface (surface1);

    /*
    cairo_set_source (cr, pattern1);
    cairo_pattern_set_extend (cairo_get_source (cr),
                              CAIRO_EXTEND_REPEAT);
    cairo_rectangle (cr, 20, 20, 100, 100);
    cairo_fill (cr);*/
    cairo_set_source_surface (cr, surface_vmlistwindow, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}

void init_vmlistctrl_image(GtkBuilder *builder)
{
	GObject * image_loginout = gtk_builder_get_object (builder, "image_loginout");
    GObject * image_exit = gtk_builder_get_object (builder, "image_exit");
	GdkScreen* screen;
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_loginout), g_loginoutNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_exit), g_exitNor);
	//button
	GObject * image_start = gtk_builder_get_object (builder, "image_start");
	GObject * image_close = gtk_builder_get_object (builder, "image_close");
	GObject * image_desktop = gtk_builder_get_object (builder, "image_desktop");
	GObject * image_sleep = gtk_builder_get_object (builder, "image_sleep");
	GObject * image_reboot = gtk_builder_get_object (builder, "image_reboot");
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_start), g_startNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_close), g_closeNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_desktop), g_desktopNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_sleep), g_sleepNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_reboot), g_restartNor);
}

static void init_vmctrlbtn_pos(GtkBuilder *builder, GtkWidget* widget, int lay_width, int lay_height)
{
	GObject * btn_start = gtk_builder_get_object (builder, "btn_start");
	GObject * btn_close = gtk_builder_get_object (builder, "btn_close");
	GObject * btn_desktop = gtk_builder_get_object (builder, "btn_desktop");
	GObject * btn_sleep = gtk_builder_get_object (builder, "btn_sleep");

	GObject * label_start = gtk_builder_get_object (builder, "label_start");
	GObject * label_close = gtk_builder_get_object (builder, "label_close");
	GObject * label_desktop = gtk_builder_get_object (builder, "label_desktop");
	GObject * label_sleep = gtk_builder_get_object (builder, "label_sleep");

	//add reboot button
	GObject * btn_reboot = gtk_builder_get_object(builder, "btn_reboot");
	GObject * label_reboot = gtk_builder_get_object(builder, "label_reboot");

	//get ctrl image width
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
    int pic_start_width = 0;
	int pic_start_height = 0;
	int pic_close_width = 0;
	int pic_close_height = 0;
	int pic_desktop_width = 0;
	int pic_desktop_height = 0;
	int pic_sleep_width = 0;
	int pic_sleep_height = 0;
	int pic_restart_width = 0;
	int pic_restart_height = 0;
	int font_size = 12;

	if (scr_width == 1024 && scr_height == 768)
	{
		gdk_pixbuf_get_file_info("images2/1024x768/start.png", &pic_start_width, &pic_start_height);
		gdk_pixbuf_get_file_info("images2/1024x768/close.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1024x768/desktop.png", &pic_desktop_width, &pic_desktop_height);
		gdk_pixbuf_get_file_info("images2/1024x768/sleep.png", &pic_sleep_width, &pic_sleep_height);
		gdk_pixbuf_get_file_info("images2/1024x768/restart.png", &pic_restart_width, &pic_restart_height);
		font_size = 8;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200) || (scr_width == 1680 && scr_height == 1050))
	{
		gdk_pixbuf_get_file_info("images2/start.png", &pic_start_width, &pic_start_height);
		gdk_pixbuf_get_file_info("images2/close.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/desktop.png", &pic_desktop_width, &pic_desktop_height);
		gdk_pixbuf_get_file_info("images2/sleep.png", &pic_sleep_width, &pic_sleep_height);
		gdk_pixbuf_get_file_info("images2/restart.png", &pic_restart_width, &pic_restart_height);
		font_size = 12;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   ||
	  (scr_width == 1600 && scr_height == 1080))
	{
		gdk_pixbuf_get_file_info("images2/1440x900/start.png", &pic_start_width, &pic_start_height);
		gdk_pixbuf_get_file_info("images2/1440x900/close.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1440x900/desktop.png", &pic_desktop_width, &pic_desktop_height);
		gdk_pixbuf_get_file_info("images2/1440x900/sleep.png", &pic_sleep_width, &pic_sleep_height);
		gdk_pixbuf_get_file_info("images2/1440x900/restart.png", &pic_restart_width, &pic_restart_height);
		font_size = 10;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		gdk_pixbuf_get_file_info("images2/1280x720/start.png", &pic_start_width, &pic_start_height);
		gdk_pixbuf_get_file_info("images2/1280x720/close.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1280x720/desktop.png", &pic_desktop_width, &pic_desktop_height);
		gdk_pixbuf_get_file_info("images2/1280x720/sleep.png", &pic_sleep_width, &pic_sleep_height);
		gdk_pixbuf_get_file_info("images2/1280x720/restart.png", &pic_restart_width, &pic_restart_height);
		font_size = 9;
	}else if ((scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		gdk_pixbuf_get_file_info("images2/1366x768/start.png", &pic_start_width, &pic_start_height);
		gdk_pixbuf_get_file_info("images2/1366x768/close.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1366x768/desktop.png", &pic_desktop_width, &pic_desktop_height);
		gdk_pixbuf_get_file_info("images2/1366x768/sleep.png", &pic_sleep_width, &pic_sleep_height);
		gdk_pixbuf_get_file_info("images2/1366x768/restart.png", &pic_restart_width, &pic_restart_height);
		font_size = 11;
	}
	setctrlFont(GTK_WIDGET(label_start), font_size);
	setctrlFont(GTK_WIDGET(label_close), font_size);
	setctrlFont(GTK_WIDGET(label_desktop), font_size);
	setctrlFont(GTK_WIDGET(label_sleep), font_size);
	setctrlFont(GTK_WIDGET(label_reboot), font_size);
	//button and image
	gtk_widget_set_size_request(GTK_WIDGET(btn_start), pic_start_width, pic_start_height);
	gtk_widget_set_size_request(GTK_WIDGET(btn_close), pic_close_width, pic_close_height);
	gtk_widget_set_size_request(GTK_WIDGET(btn_desktop), pic_desktop_width, pic_desktop_height);
	gtk_widget_set_size_request(GTK_WIDGET(btn_sleep), pic_sleep_width, pic_sleep_height);
	gtk_widget_set_size_request(GTK_WIDGET(btn_reboot), pic_restart_width, pic_restart_height);
	//label
	int sz_start_width = 0;
	int sz_start_height = 0;
	get_string_width("启动", &sz_start_width, &sz_start_height);
    int sz_close_width = 0;
	int sz_close_height = 0;
	get_string_width("关闭", &sz_close_width, &sz_close_height);
    int sz_desktop_width = 0;
	int sz_desktop_height = 0;
	get_string_width("桌面", &sz_desktop_width, &sz_desktop_height);
    int sz_sleep_width  = 0;
	int sz_sleep_height = 0;
	get_string_width("待机", &sz_sleep_width, &sz_sleep_height);
	int sz_reboot_width = 0;
	int sz_reboot_height = 0;
	get_string_width("重启", &sz_reboot_width, &sz_reboot_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_start), sz_start_width, sz_start_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_close), sz_close_width, sz_close_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_desktop), sz_desktop_width, sz_desktop_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_sleep), sz_sleep_width, sz_sleep_height);
	//cal ctrl position
	int center = lay_width/2 - 10;
	int top = 20;
	int delay = 10;
	int x,y = 0;
	x = center - delay/2 - sz_close_width/2 + delay;
	y = top;
	if (scr_width == 1280 && scr_height == 1024)
	{
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_reboot), x - 6, y - 4);
	}else if (scr_width == 1024 && scr_height == 768)
	{
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_reboot), x - 11, y - 4);
	}else
	{
		gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_reboot), x - 3, y - 2);
	}
	x = center - delay/2 - sz_close_width/2 - delay - pic_close_width + delay*2;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_reboot), x, y);

	x = center - delay/2 - sz_close_width/2 - delay - pic_close_width - delay - sz_close_width + delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_close), x, y);
	x = center - delay/2 - sz_close_width/2 - delay - pic_close_width - delay - sz_close_width - delay - pic_close_width + delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_close), x, y);
	x = center - delay/2 - sz_close_width/2 - delay - pic_close_width - delay - sz_close_width - delay - pic_close_width - delay - sz_close_width + delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_start), x, y);
	x = center - delay/2 - sz_close_width/2 - delay - pic_close_width - delay - sz_close_width - delay - pic_close_width - delay - sz_close_width - delay - pic_start_width + delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_start), x, y);
	//dir right
	x = center + delay/2 + sz_close_width/2 + delay*2;
	y = top;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_desktop), x, y);
	x = center + delay/2 + sz_close_width/2 + pic_desktop_width + delay*2 + delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_desktop), x, y);
	x = center + delay/2 + sz_close_width/2 + pic_desktop_width + delay*2 + sz_desktop_width + delay + delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_sleep), x, y);
	x = center + delay/2 + sz_close_width/2 + pic_desktop_width + delay*2 + sz_desktop_width + delay + pic_sleep_width + delay + delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_sleep), x, y);
}

static void get_string_width(char *str, int* width, int* height)
{

//	GtkWidget *label = NULL;
//	PangoLayout *layout = NULL;
//	label = gtk_label_new ("");
//	layout = gtk_widget_create_pango_layout (label, str);
//    pango_layout_get_pixel_size(layout, width, height);
//    LogInfo("get_string_width: %s pixel size is width = %d, height = %d.\n", str, *width, *height);
//	g_object_unref(layout);
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	if (scr_width == 1024 && scr_height == 768)
	{
		*width = 20;
		*height = 20;
	}else if ((scr_width == 1920 && scr_height == 1080) ||
				(scr_width == 1440 && scr_height == 900) ||
				(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  ||
				(scr_width == 1600 && scr_height == 1080) )
	{
		*width = 30;
		*height = 20;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	   (scr_width == 1280 && scr_height == 1024) )
	{
		*width = 20;
		*height = 20;
	}else if ((scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768) )
	{
		*width = 30;
		*height = 20;
	}else if ((scr_width == 1680 && scr_height == 1050))
	{
		*width = 30;
		*height = 20;
	}
}

static void setup_tree_view(GtkWidget*);
static void loadcss2()
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
                                  g_filename_to_utf8(g_home_css, strlen(g_home_css), &bytes_read, &bytes_written, &error),
                                  NULL);
    //gtk_css_provider_load_from_path (provider, home, NULL);
    g_object_unref (provider);
}

static void loadcss()
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

	 GdkScreen* screen1;
	 screen1 = gdk_screen_get_default();
     int scr_width = gdk_screen_get_width(screen1);
     int scr_height = gdk_screen_get_height(screen1);
	if (scr_width == 1024 && scr_height == 768)
	{
		gtk_css_provider_load_from_data (provider,"column-header .button {\n"
                                         "background-color: transparent;\n"
                                         "background-image: none;\n"
                                         "border-width: 0px;\n"
                                         "color: #5C82A2;\n"
                                         "font-size: 10px;\n"
                                         "padding: 0px;\n"
                                         "}\n"
                                         "GtkTreeView {\n"
                                         "border-style:none;\n"
                                         "font-size: 9px;\n"
                                         "color: #5C82A2;\n"
                                         "padding: 1px;\n", -1, NULL);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200) || (scr_width == 1680 && scr_height == 1050))
	{
	/*
		gtk_css_provider_load_from_path (provider,
								 g_filename_to_utf8(g_home_css, strlen(g_home_css), &bytes_read, &bytes_written, &error),
								 NULL);*/

		gtk_css_provider_load_from_data (provider,"column-header .button {\n"
                                         "background-color: transparent;\n"
                                         "background-image: none;\n"
                                         "border-width: 0px;\n"
                                         "color: #5C82A2;\n"
                                         "font-size: 14px;\n"
                                         "padding: 0px;\n"
                                         "height-request: 12px; \n"
                                         "}\n"
                                         "GtkTreeView {\n"
                                         "border-style:none;\n"
                                         "font-size: 13px;\n"
                                         "color: #5C82A2;\n"
                                         "padding: 1px;\n", -1, NULL);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080))
	{
		gtk_css_provider_load_from_data (provider,"column-header .button {\n"
                                         "background-color: transparent;\n"
                                         "background-image: none;\n"
                                         "border-width: 0px;\n"
                                         "color: #5C82A2;\n"
                                         "font-size: 12px;\n"
                                         "padding: 0px;\n"
                                         "height-request: 12px; \n"
                                         "}\n"
                                         "GtkTreeView {\n"
                                         "border-style:none;\n"
                                         "font-size: 11px;\n"
                                         "color: #5C82A2;\n"
                                         "padding: 1px;\n", -1, NULL);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	   (scr_width == 1280 && scr_height == 1024))
	{
		gtk_css_provider_load_from_data (provider,"column-header .button {\n"
                                         "background-color: transparent;\n"
                                         "background-image: none;\n"
                                         "border-width: 0px;\n"
                                         "color: #5C82A2;\n"
                                         "font-size: 11px;\n"
                                         "padding: 0px;\n"
                                         "height-request: 12px; \n"
                                         "}\n"
                                         "GtkTreeView {\n"
                                         "border-style:none;\n"
                                         "font-size: 10px;\n"
                                         "color: #5C82A2;\n"
                                         "padding: 1px;\n", -1, NULL);
	}else if ((scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) ||
		(scr_width == 1360 && scr_height == 768))
	{
		gtk_css_provider_load_from_data (provider,"column-header .button {\n"
                                         "background-color: transparent;\n"
                                         "background-image: none;\n"
                                         "border-width: 0px;\n"
                                         "color: #5C82A2;\n"
                                         "font-size: 12px;\n"
                                         "padding: 0px;\n"
                                         "height-request: 12px; \n"
                                         "}\n"
                                         "GtkTreeView {\n"
                                         "border-style:none;\n"
                                         "font-size: 10px;\n"
                                         "color: #5C82A2;\n"
                                         "padding: 1px;\n", -1, NULL);
	}
	 //gtk_css_provider_load_from_path (provider, home, NULL);
	 g_object_unref (provider);
}

static void loadImage()
{
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);

	if (scr_width == 1024 && scr_height == 768)
	{
		g_startPress = gdk_pixbuf_new_from_file("images2/1024x768/start_press.png", NULL);
	    g_startNor = gdk_pixbuf_new_from_file("images2/1024x768/start.png", NULL);
	    g_closeNor = gdk_pixbuf_new_from_file("images2/1024x768/close.png", NULL);
	    g_closePress = gdk_pixbuf_new_from_file("images2/1024x768/close_press.png", NULL);
	    g_desktopNor = gdk_pixbuf_new_from_file("images2/1024x768/desktop.png", NULL);
	    g_desktopPress = gdk_pixbuf_new_from_file("images2/1024x768/desktop_press.png", NULL);
	    g_sleepNor = gdk_pixbuf_new_from_file("images2/1024x768/sleep.png", NULL);
	    g_sleepPress = gdk_pixbuf_new_from_file("images2/1024x768/sleep_press.png", NULL);
	    g_restartNor = gdk_pixbuf_new_from_file("images2/1024x768/restart.png", NULL);
	    g_restartPress = gdk_pixbuf_new_from_file("images2/1024x768/restart_press.png", NULL);

	    g_exitNor = gdk_pixbuf_new_from_file("images2/1024x768/exit_nor.png", NULL);
	    g_exitPress = gdk_pixbuf_new_from_file("images2/1024x768/exit_press.png", NULL);
	    g_loginoutNor = gdk_pixbuf_new_from_file("images2/1024x768/loginout_nor.png", NULL);
	    g_loginoutPress = gdk_pixbuf_new_from_file("images2/1024x768/loginout_press.png", NULL);
		g_vmIconPix = gdk_pixbuf_new_from_file("images2/1024x768/vm_icon.png", NULL);
	}
	else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200) || (scr_width == 1680 && scr_height == 1050))
	{
		g_startPress = gdk_pixbuf_new_from_file(IMAGE_BTN_START_PRES, NULL);
	    g_startNor = gdk_pixbuf_new_from_file(IMAGE_BTN_START_NOR, NULL);
	    g_closeNor = gdk_pixbuf_new_from_file(IMAGE_BTN_CLOSE_NOR, NULL);
	    g_closePress = gdk_pixbuf_new_from_file(IMAGE_BTN_CLOSE_PRES, NULL);
	    g_desktopNor = gdk_pixbuf_new_from_file(IMAGE_BTN_DESKTOP_NOR, NULL);
	    g_desktopPress = gdk_pixbuf_new_from_file(IMAGE_BTN_DESKTOP_PRES, NULL);
	    g_sleepNor = gdk_pixbuf_new_from_file(IMAGE_BTN_SLEEP_NOR, NULL);
	    g_sleepPress = gdk_pixbuf_new_from_file(IMAGE_BTN_SLEEP_PRES, NULL);
	    g_restartNor = gdk_pixbuf_new_from_file("images2/restart.png", NULL);
	    g_restartPress = gdk_pixbuf_new_from_file("images2/restart_press.png", NULL);

	    g_exitNor = gdk_pixbuf_new_from_file(IMAGE_BTN_EXIT_NOR, NULL);
	    g_exitPress = gdk_pixbuf_new_from_file(IMAGE_BTN_EXIT_PRES, NULL);
	    g_loginoutNor = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGINOUT_NOR, NULL);
	    g_loginoutPress = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGINOUT_PRES, NULL);
		g_vmIconPix = gdk_pixbuf_new_from_file(IMAGE_TREE_VMICON_NOR, NULL);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   ||
	   (scr_width == 1600 && scr_height == 1080) )
	{
		g_startPress = gdk_pixbuf_new_from_file("images2/1440x900/start_press.png", NULL);
	    g_startNor = gdk_pixbuf_new_from_file("images2/1440x900/start.png", NULL);
	    g_closeNor = gdk_pixbuf_new_from_file("images2/1440x900/close.png", NULL);
	    g_closePress = gdk_pixbuf_new_from_file("images2/1440x900/close_press.png", NULL);
	    g_desktopNor = gdk_pixbuf_new_from_file("images2/1440x900/desktop.png", NULL);
	    g_desktopPress = gdk_pixbuf_new_from_file("images2/1440x900/desktop_press.png", NULL);
	    g_sleepNor = gdk_pixbuf_new_from_file("images2/1440x900/sleep.png", NULL);
	    g_sleepPress = gdk_pixbuf_new_from_file("images2/1440x900/sleep_press.png", NULL);
		g_restartNor = gdk_pixbuf_new_from_file("images2/1440x900/restart.png", NULL);
	    g_restartPress = gdk_pixbuf_new_from_file("images2/1440x900/restart_press.png", NULL);

	    g_exitNor = gdk_pixbuf_new_from_file("images2/1440x900/exit_nor.png", NULL);
	    g_exitPress = gdk_pixbuf_new_from_file("images2/1440x900/exit_press.png", NULL);
	    g_loginoutNor = gdk_pixbuf_new_from_file("images2/1440x900/loginout_nor.png", NULL);
	    g_loginoutPress = gdk_pixbuf_new_from_file("images2/1440x900/loginout_press.png", NULL);
		g_vmIconPix = gdk_pixbuf_new_from_file("images2/1440x900/vm_icon.png", NULL);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	 (scr_width == 1280 && scr_height == 1024) )
	{
		g_startPress = gdk_pixbuf_new_from_file("images2/1280x720/start_press.png", NULL);
	    g_startNor = gdk_pixbuf_new_from_file("images2/1280x720/start.png", NULL);
	    g_closeNor = gdk_pixbuf_new_from_file("images2/1280x720/close.png", NULL);
	    g_closePress = gdk_pixbuf_new_from_file("images2/1280x720/close_press.png", NULL);
	    g_desktopNor = gdk_pixbuf_new_from_file("images2/1280x720/desktop.png", NULL);
	    g_desktopPress = gdk_pixbuf_new_from_file("images2/1280x720/desktop_press.png", NULL);
	    g_sleepNor = gdk_pixbuf_new_from_file("images2/1280x720/sleep.png", NULL);
	    g_sleepPress = gdk_pixbuf_new_from_file("images2/1280x720/sleep_press.png", NULL);
		g_restartNor = gdk_pixbuf_new_from_file("images2/1280x720/restart.png", NULL);
		g_restartPress = gdk_pixbuf_new_from_file("images2/1280x720/restart_press.png", NULL);


	    g_exitNor = gdk_pixbuf_new_from_file("images2/1280x720/exit_nor.png", NULL);
	    g_exitPress = gdk_pixbuf_new_from_file("images2/1280x720/exit_press.png", NULL);
	    g_loginoutNor = gdk_pixbuf_new_from_file("images2/1280x720/loginout_nor.png", NULL);
	    g_loginoutPress = gdk_pixbuf_new_from_file("images2/1280x720/loginout_press.png", NULL);
		g_vmIconPix = gdk_pixbuf_new_from_file("images2/1280x720/vm_icon.png", NULL);
	}else if ((scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) ||
		(scr_width == 1360 && scr_height == 768))
	{
		g_startPress = gdk_pixbuf_new_from_file("images2/1366x768/start_press.png", NULL);
	    g_startNor = gdk_pixbuf_new_from_file("images2/1366x768/start.png", NULL);
	    g_closeNor = gdk_pixbuf_new_from_file("images2/1366x768/close.png", NULL);
	    g_closePress = gdk_pixbuf_new_from_file("images2/1366x768/close_press.png", NULL);
	    g_desktopNor = gdk_pixbuf_new_from_file("images2/1366x768/desktop.png", NULL);
	    g_desktopPress = gdk_pixbuf_new_from_file("images2/1366x768/desktop_press.png", NULL);
	    g_sleepNor = gdk_pixbuf_new_from_file("images2/1366x768/sleep.png", NULL);
	    g_sleepPress = gdk_pixbuf_new_from_file("images2/1366x768/sleep_press.png", NULL);
		g_restartNor = gdk_pixbuf_new_from_file("images2/1366x768/restart.png", NULL);
		g_restartPress = gdk_pixbuf_new_from_file("images2/1366x768/restart_press.png", NULL);

	    g_exitNor = gdk_pixbuf_new_from_file("images2/1366x768/exit_nor.png", NULL);
	    g_exitPress = gdk_pixbuf_new_from_file("images2/1366x768/exit_press.png", NULL);
	    g_loginoutNor = gdk_pixbuf_new_from_file("images2/1366x768/loginout_nor.png", NULL);
	    g_loginoutPress = gdk_pixbuf_new_from_file("images2/1366x768/loginout_press.png", NULL);
		g_vmIconPix = gdk_pixbuf_new_from_file("images2/1366x768/vm_icon.png", NULL);
	}
}


static gint my_button_handler(GtkWidget *widget,GdkEvent *event)
{
	GtkMenu *menu;
	GdkEventButton *event_button;

	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (GTK_IS_MENU (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);

	// The "widget" is the menu that was supplied when
	// g_signal_connect_swapped() was called.
	menu = GTK_MENU(widget);

	if (event->type == GDK_BUTTON_PRESS)
	{
	  event_button = (GdkEventButton *) event;
	  if (event_button->button == GDK_BUTTON_SECONDARY)
	    {
	      gtk_menu_popup(menu, NULL, NULL, NULL, NULL,
	                      event_button->button, event_button->time);
	      return TRUE;
	    }
	}
	return FALSE;
}

static gboolean on_label_start_clicked(GtkButton *button,  gpointer user_data)
{
	LogInfo("on_label_start_clicked start.");
	on_btn_start_clicked(NULL, NULL);
	return TRUE;
}
static gboolean on_label_close_clicked(GtkButton *button,  gpointer user_data)
{
	on_btn_close_clicked(NULL, NULL);
	return TRUE;
}

static gboolean on_label_desktop_clicked(GtkButton *button,  gpointer user_data)
{
	on_btn_desktop_clicked(NULL, NULL);
	return TRUE;
}

static gboolean on_label_sleep_clicked(GtkButton *button,  gpointer user_data)
{
	on_btn_sleep_clicked(NULL, NULL);
	return TRUE;
}
static gboolean on_label_reboot_clicked(GtkButton *button,  gpointer user_data)
{
	on_btn_reboot_clicked(NULL, NULL);
	return TRUE;
}

static void  on_label_start_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_startPress);
}
static void  on_label_start_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_startNor);
}
static void  on_label_close_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_closePress);
}
static void  on_label_close_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_closeNor);
}

static void  on_label_desktop_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_desktopPress);
}
static void  on_label_desktop_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_desktopNor);
}
static void  on_label_sleep_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_sleepPress);
}
static void  on_label_sleep_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_sleepNor);
}
static void  on_label_restart_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_restartPress);
}
static void  on_label_restart_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_restartNor);
}



void bind_ctrl_event(GtkBuilder *builder)
{
	if (NULL == builder)
		return;
	GObject *label_start = NULL;
    GObject *label_close = NULL;
    GObject *label_desktop = NULL;
    GObject *label_sleep = NULL;
    GObject *label_reboot = NULL;
	label_start = gtk_builder_get_object (builder, "label_start");
    label_close = gtk_builder_get_object (builder, "label_close");
    label_desktop = gtk_builder_get_object (builder, "label_desktop");
    label_sleep = gtk_builder_get_object (builder, "label_sleep");
    label_reboot = gtk_builder_get_object (builder, "label_reboot");
	
	GObject *image_start = NULL;
    GObject *image_close = NULL;
    GObject *image_desktop = NULL;
    GObject *image_sleep = NULL;
    GObject *image_reboot = NULL;

    image_start = gtk_builder_get_object (builder, "image_start");
    image_close = gtk_builder_get_object (builder, "image_close");
    image_desktop = gtk_builder_get_object (builder, "image_desktop");
    image_sleep = gtk_builder_get_object (builder, "image_sleep");
	image_reboot = gtk_builder_get_object (builder, "image_reboot");
	
	if (NULL != label_start)
	{
		g_signal_connect(G_OBJECT(label_start), "clicked", G_CALLBACK(on_label_start_clicked), NULL);
		g_signal_connect(G_OBJECT(label_start), "pressed", G_CALLBACK(on_label_start_pressed), (GtkWidget *)image_start);
    	g_signal_connect(G_OBJECT(label_start), "released", G_CALLBACK(on_label_start_released), (GtkWidget *)image_start);
	}
	if (NULL != label_close)
	{
		g_signal_connect(G_OBJECT(label_close), "clicked", G_CALLBACK(on_label_close_clicked), NULL);
		g_signal_connect(G_OBJECT(label_close), "pressed", G_CALLBACK(on_label_close_pressed), (GtkWidget *)image_close);
    	g_signal_connect(G_OBJECT(label_close), "released", G_CALLBACK(on_label_close_released), (GtkWidget *)image_close);
	}
	if (NULL != label_desktop)
	{
		g_signal_connect(G_OBJECT(label_desktop), "clicked", G_CALLBACK(on_label_desktop_clicked), NULL);
		g_signal_connect(G_OBJECT(label_desktop), "pressed", G_CALLBACK(on_label_desktop_pressed), (GtkWidget *)image_desktop);
    	g_signal_connect(G_OBJECT(label_desktop), "released", G_CALLBACK(on_label_desktop_released), (GtkWidget *)image_desktop);
	}
	if (NULL != label_sleep)
	{
		g_signal_connect(G_OBJECT(label_sleep), "clicked", G_CALLBACK(on_label_sleep_clicked), NULL);
		g_signal_connect(G_OBJECT(label_sleep), "pressed", G_CALLBACK(on_label_sleep_pressed), (GtkWidget *)image_sleep);
    	g_signal_connect(G_OBJECT(label_sleep), "released", G_CALLBACK(on_label_sleep_released), (GtkWidget *)image_sleep);
	}
	if (NULL != label_reboot)
	{
		g_signal_connect(G_OBJECT(label_reboot), "clicked", G_CALLBACK(on_label_reboot_clicked), NULL);
		g_signal_connect(G_OBJECT(label_reboot), "pressed", G_CALLBACK(on_label_restart_pressed), (GtkWidget *)image_reboot);
    	g_signal_connect(G_OBJECT(label_reboot), "released", G_CALLBACK(on_label_restart_released), (GtkWidget *)image_reboot);
	}
}

void SY_vmlistwindow_main()
{
    if (showvmlistwindow == 1)
       return;
    showvmlistwindow = 1;
    GObject *window;
    GtkBuilder *builder;
    GObject *treeview;
    GtkTreeModel *model;
    GtkTreeSelection *selection;
    GObject *btn_start;
    GObject *btn_close;
    GObject *btn_desktop;
    GObject *btn_sleep;
	GObject *btn_reboot;

    GObject *label_start;
    GObject *label_close;
    GObject *label_desktop;
    GObject *label_sleep;
    GObject *label_reboot;
    builder = gtk_builder_new ();
    GError *errort = NULL;
    gtk_builder_add_from_file (builder, "vmlistwindow.glade", &errort);
    window = gtk_builder_get_object (builder, "vmlist_window");
    gtk_widget_set_app_paintable((GtkWidget * )window, TRUE);
    treeview = gtk_builder_get_object (builder, "treeview_vm");
	column_len();
    setup_tree_view(GTK_WIDGET(treeview));
    g_window = window;
    g_treeview = treeview;
    g_exitvm = 0;
    g_flushState = 0;
	memset(g_upVms, 0, sizeof(g_upVms));
	memset(g_vmName, 0, sizeof(g_vmName));

    btn_start = gtk_builder_get_object (builder, "btn_start");
    btn_close = gtk_builder_get_object (builder, "btn_close");
    btn_desktop = gtk_builder_get_object (builder, "btn_desktop");
    btn_sleep = gtk_builder_get_object (builder, "btn_sleep");
    btn_reboot = gtk_builder_get_object (builder, "btn_reboot");

    label_start = gtk_builder_get_object (builder, "label_start");
    label_close = gtk_builder_get_object (builder, "label_close");
    label_desktop = gtk_builder_get_object (builder, "label_desktop");
    label_sleep = gtk_builder_get_object (builder, "label_sleep");
    label_reboot = gtk_builder_get_object (builder, "label_reboot");
	init_ctrl_posit(builder);
    create_surfaces();
	loadImage();
	loadcss();
	loadcss2();
	g_vmsCount = 0;
	initUpdateVms();
    adddata();
	init_vmlistctrl_image(builder);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(g_store));
    //读取登录信息
    Parsexml("user", g_szUser, 0);
    Parsexml("password", g_szPass, 0);

    GObject *label_login;
    label_login = gtk_builder_get_object (builder, "label_login");
	g_label_login = label_login;
    char szTmp[MAX_BUFF_SIZE] = {0};
    sprintf(szTmp, "当前登录用户: %s", g_szUser);
	int width = 0;
	int height = 0;
	gtk_widget_get_size_request(GTK_WIDGET(label_login), width, height);
	char szMsg2[1024*2] = {0};
	strcpy(szMsg2, g_MsgCall.szMsg);
	strcat(szMsg2, "  ");
	strcat(szMsg2, szTmp);
	LogInfo("vmlist window init szMsg2 = %s.", szMsg2);
    gtk_label_set_text(GTK_LABEL(label_login), szMsg2);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
//	gtk_tree_model_get_iter_first(model, &g_iter);
//  	gtk_tree_selection_select_iter (selection, &g_iter);
    g_signal_connect(selection, "changed", G_CALLBACK(on_vmlist_changed), model);
    g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK (on_expose_vmlistevent), NULL);
    g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(on_gtk_main_quit_vmlist), NULL);

    //button event
    gtk_button_set_label(GTK_BUTTON(label_start), "启动");
    gtk_button_set_label(GTK_BUTTON(label_close), "关闭");
    gtk_button_set_label(GTK_BUTTON(label_desktop), "桌面");
    gtk_button_set_label(GTK_BUTTON(label_sleep), "刷新");
    gtk_button_set_label(GTK_BUTTON(label_reboot), "重启");

    g_signal_connect(G_OBJECT(btn_start), "clicked", G_CALLBACK(on_btn_start_clicked), NULL);
    g_signal_connect(G_OBJECT(btn_close), "clicked", G_CALLBACK(on_btn_close_clicked), NULL);
    g_signal_connect(G_OBJECT(btn_desktop), "clicked", G_CALLBACK(on_btn_desktop_clicked), NULL);
    g_signal_connect(G_OBJECT(btn_sleep), "clicked", G_CALLBACK(on_btn_sleep_clicked), NULL);
	g_signal_connect(G_OBJECT(btn_reboot), "clicked", G_CALLBACK(on_btn_reboot_clicked), NULL);
	bind_ctrl_event(builder);
    //
    GObject *image_start;
    GObject *image_close;
    GObject *image_desktop;
    GObject *image_sleep;
    GObject *image_reboot;
    image_start = gtk_builder_get_object (builder, "image_start");
    image_close = gtk_builder_get_object (builder, "image_close");
    image_desktop = gtk_builder_get_object (builder, "image_desktop");
    image_sleep = gtk_builder_get_object (builder, "image_sleep");
	image_reboot = gtk_builder_get_object (builder, "image_reboot");
    g_signal_connect(G_OBJECT(btn_start), "pressed", G_CALLBACK(on_btn_start_pressed), (GtkWidget *)image_start);
    g_signal_connect(G_OBJECT(btn_start), "released", G_CALLBACK(on_btn_start_released), (GtkWidget *)image_start);

    g_signal_connect(G_OBJECT(btn_close), "pressed", G_CALLBACK(on_btn_close_pressed), (GtkWidget *)image_close);
    g_signal_connect(G_OBJECT(btn_close), "released", G_CALLBACK(on_btn_close_released), (GtkWidget *)image_close);

    g_signal_connect(G_OBJECT(btn_desktop), "pressed", G_CALLBACK(on_btn_desktop_pressed), (GtkWidget *)image_desktop);
    g_signal_connect(G_OBJECT(btn_desktop), "released", G_CALLBACK(on_btn_desktop_released), (GtkWidget *)image_desktop);

    g_signal_connect(G_OBJECT(btn_sleep), "pressed", G_CALLBACK(on_btn_sleep_pressed), (GtkWidget *)image_sleep);
    g_signal_connect(G_OBJECT(btn_sleep), "released", G_CALLBACK(on_btn_sleep_released), (GtkWidget *)image_sleep);
    g_signal_connect(G_OBJECT(btn_reboot), "pressed", G_CALLBACK(on_btn_restart_pressed), (GtkWidget *)image_reboot);
    g_signal_connect(G_OBJECT(btn_reboot), "released", G_CALLBACK(on_btn_restart_released), (GtkWidget *)image_reboot);

    GObject * button_loginout = gtk_builder_get_object (builder, "button_loginout");
    GObject * button_exit = gtk_builder_get_object (builder, "button_exit");
    GObject * image_loginout = gtk_builder_get_object (builder, "image_loginout");
    GObject * image_exit = gtk_builder_get_object (builder, "image_exit");
	//
	g_signal_connect(G_OBJECT(button_loginout), "enter", G_CALLBACK(on_btn_logout_enter), (GtkWidget *)image_loginout);
	g_signal_connect(G_OBJECT(button_loginout), "pressed", G_CALLBACK(on_btn_logout_pressed), (GtkWidget *)image_loginout);
    g_signal_connect(G_OBJECT(button_loginout), "leave", G_CALLBACK(on_btn_logout_leave), (GtkWidget *)image_loginout);
    //
	g_signal_connect(G_OBJECT(button_exit), "enter", G_CALLBACK(on_btn_exit_enter), (GtkWidget *)image_exit);
	g_signal_connect(G_OBJECT(button_exit), "pressed", G_CALLBACK(on_btn_exit_pressed), (GtkWidget *)image_exit);
    g_signal_connect(G_OBJECT(button_exit), "leave", G_CALLBACK(on_btn_exit_leave), (GtkWidget *)image_exit);

    //全屏显示
    //gtk_window_set_keep_above(GTK_WINDOW(g_window), TRUE);
    //gtk_window_set_default_size(GTK_WINDOW(g_window), g_screen_width, g_screen_height);
    gtk_window_set_decorated(GTK_WINDOW(g_window), FALSE); /* hide the title bar and the boder */
    //LogInfo("vmlistwindow screen width: %d, height: %d\n", g_screen_width, g_screen_height);
    g_signal_connect(G_OBJECT(g_window), \
                      "key-press-event", \
                      G_CALLBACK(on_key_press), NULL);

#if 0
	//flush menu
	GtkWidget *menu_item;
	GtkWidget * menu = gtk_menu_new();
	menu_item = gtk_menu_item_new_with_label("刷新");
	gtk_menu_shell_append(GTK_MENU(menu), menu_item);
	gtk_widget_show(menu_item);
	gtk_widget_show(menu);
	g_signal_connect_swapped(G_OBJECT(treeview),"button_press_event",G_CALLBACK(my_button_handler),G_OBJECT(menu));
	g_signal_connect(GTK_MENU_ITEM(menu_item), "activate", G_CALLBACK(on_flush_vmlist),"刷新");
#endif

	gtk_window_fullscreen(GTK_WINDOW(window));
    //全屏显示结束

    g_sUpdateVmStatus = 1;
    //signal(SIGALRM, signal_handler);
    //set_timer();
    // 创建线程tid，且线程函数由thrd_func指向，是thrd_func的入口点，即马上执行此线程函数
    if ( pthread_create(&tid, NULL, thrd_func, NULL) !=0 ) {
        printf("Create thread error!\n");
    }
    if (g_workflag == 1)
	{
		start_tcpclient();
		strcpy(report.uname, g_szUser);
		strcpy(report.vname, g_vmName);
		report.action = 1;
		send_data(report);
	}

	//add 170727
//#ifdef MEETING
#if 1
		char szMsg[BUF_MSG_LEN]= {0};
		sprintf(szMsg, "\napagentui.ThinviewLogin####{\"username\": \"%s\"}\n", g_szUser);
		write(1, szMsg, strlen(szMsg));
#endif
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
    g_object_unref (G_OBJECT (g_store));
	destroy_surfaces();
    showvmlistwindow = 0;
	if (g_workflag == 1)
	{
		strcpy(report.uname, g_szUser);
		strcpy(report.vname, g_vmName);
		report.action = 4;
		send_data(report);
		close_tcpclient();
	}
	//Oivrt_closeSession();
}

/* 功能:      设置控件字体大小
* widget:    需要改变字体的控件
* size:      字体大小
* is_button: TRUE代表控件为按钮，FALSE为其它控件
*/
/*static void set_widget_font_size(GtkWidget *widget, int size, GtkTreeViewColumn *column, GtkCellRenderer *renderer)
{
   GtkWidget *labelChild;
   PangoFontDescription *font;
   gint fontSize = size;

   font = pango_font_description_from_string("Microsoft YaHei");          //"Sans"字体名
   pango_font_description_set_size(font, fontSize*PANGO_SCALE);//设置字体大小

   //设置label的字体，这样这个GtkButton上面显示的字体就变了
   gtk_widget_modify_font(GTK_WIDGET(column), font);
   //gtk_widget_override_font(GTK_WIDGET(labelChild), font);

   PangoAttrList *attrls = NULL;
   attrls = pango_attr_list_new();
   PangoAttribute *attr = NULL;
   attr = pango_attr_foreground_new(65535,65535,65535); //rgb
   pango_attr_list_change(attrls, attr);
   //gtk_label_set_attributes(GTK_LABEL(labelChild), attrls);
   gtk_tree_view_column_set_attributes(column, renderer, attrls);
   pango_attr_list_unref(attrls); // 释放
   pango_font_description_free(font);
}
*/
static void setup_tree_view(GtkWidget *treeview) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer_icon;

    /* Create a new GtkCellRendererText, add it to the tree view column
     * and append the column to the tree view.
     */
    renderer_icon = gtk_cell_renderer_pixbuf_new();
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("", renderer_icon, "pixbuf", NAME, NULL);
    //g_object_set(G_OBJECT(renderer), "foreground", "red", NULL);
    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[0]);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("虚拟机", renderer, "text", NAME_1, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[1]);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("OS", renderer, "text", OS, NULL);
    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[2]);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
//    renderer = gtk_cell_renderer_text_new();
//    column = gtk_tree_view_column_new_with_attributes("标签", renderer, "text", TAB, NULL);
//    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[3]);
//    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
//    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("状态", renderer, "text", STATUS, NULL);
    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[4]);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("vCpu", renderer, "text", VCPU, NULL);
    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[5]);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("内存", renderer, "text", MEMORY, NULL);
    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[6]);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("主机地址", renderer, "text", IP, NULL);
    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[7]);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("USB策略", renderer, "text", USB, NULL);
    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[8]);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

void adddata()
{
  /* Add all of the products to the GtkListStore. */
  //vms data deal with
  //insert vms data into list
  GtkTreeIter  iter;
  g_store = gtk_list_store_new(COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                      G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  list_for_each(plist, &head)
  {
      struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
    /*  printf("main get vms name = %s.\n", node->val.name);
      printf("main get vms os = %s.\n", node->val.os);
      printf("main get vms status = %d.\n", node->val.status);
      printf("main get vms cpu count = %d.\n", node->val.vcpu);
      printf("main get vms memory = %ld.\n", node->val.memory);
      printf("main get vms ip = %s.\n", node->val.ip);
      printf("main get vms usb strategy = %d.\n", node->val.usb);*/

      strcpy(g_upVms[g_vmsCount].szvmid, node->val.vmid);
	  //LogInfo("adddata, g_upVms, g_vmsCont : %d, vmsid : %s", g_vmsCount, g_upVms[g_vmsCount].szvmid);
      char szStatus[MAX_BUFF_SIZE] = {0};
      char szMemory[MAX_BUFF_SIZE] = {0};
      char szUsb[MAX_BUFF_SIZE] = {0};
      char szvcpu[MAX_BUFF_SIZE] = {0};
      if (node->val.status == 1)
         strcpy(szStatus, "运行");
      else if (node->val.status == 0)
         strcpy(szStatus, "已关机");
      else if (node->val.status == 2)
         strcpy(szStatus, "已暂停");
      else if (node->val.status == 3)
         strcpy(szStatus, "正在启动");
      else if (node->val.status == 4)
         strcpy(szStatus, "正在关机");
	  else if (node->val.status == 7)
		 strcpy(szStatus, "正在重启");
	  else if (node->val.status == 8)
		 strcpy(szStatus, "初始化中");
	  else if (node->val.status == 10)
		 strcpy(szStatus, "正在迁移");
	  else if (node->val.status == 11)
		 strcpy(szStatus, "错误");
	  else if (node->val.status == 12)
		 strcpy(szStatus, "镜像锁定");
	  else if (node->val.status == 13)
		 strcpy(szStatus, "非法镜像");
	  
	  if ((node->val.memory - (int)node->val.memory) > 0)
      	sprintf(szMemory, "%.2f GB", node->val.memory);
	  else
	  	sprintf(szMemory, "%.0f GB", node->val.memory);

      if (node->val.usb == 1)
        memcpy(szUsb, "支持", MAX_BUFF_SIZE);
      else if (node->val.usb == 0)
        memcpy(szUsb, "不支持", MAX_BUFF_SIZE);

      sprintf(szvcpu, "%d", node->val.vcpu);
      gtk_list_store_append(g_store, &iter);
      gtk_list_store_set(g_store, &iter,
                        NAME, g_vmIconPix,
                        NAME_1, node->val.name,
                        OS, node->val.os,
                        STATUS, szStatus,
                        VCPU, szvcpu,
                        MEMORY, szMemory,
                        IP,  node->val.ip,
                        USB, szUsb,
                        -1);
	  g_upVms[g_vmsCount].iter = iter;
	  g_vmsCount++;
  }
}
