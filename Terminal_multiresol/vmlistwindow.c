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

enum {
    NAME = 0,
    NAME_1,
    OS,
    TAB,
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
unsigned short g_sUpdateVmStatus = 0;
static int showvmlistwindow = 0;
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
struct Vms_Update g_upVms[MAX_BUFF_SIZE];

static int columnwidth_array[9] = {80,200,200,100,150,100,100,200,200};

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
		columnwidth_array[2] = 90;
		columnwidth_array[3] = 60;
		columnwidth_array[4] = 60;
		columnwidth_array[5] = 60;
		columnwidth_array[6] = 60;
		columnwidth_array[7] = 100;
		columnwidth_array[8] = 60;
	}else if (scr_width == 1920 && scr_height == 1080)
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
	}else if (scr_width == 1440 && scr_height == 900)
	{
		columnwidth_array[0] = 80;
		columnwidth_array[1] = 140;
		columnwidth_array[2] = 180;
		columnwidth_array[3] = 90;
		columnwidth_array[4] = 120;
		columnwidth_array[5] = 90;
		columnwidth_array[6] = 90;
		columnwidth_array[7] = 180;
		columnwidth_array[8] = 100;
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

	GdkScreen* screen;
	screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
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
		layout3_width = 700;
		layout3_height = scr_height - layout1_height;
		layout4_width = scr_width - layout3_width;
		layout4_height = scr_height - layout1_height;
		treeview_vm_width = layout3_width;
		treeview_vm_height = layout3_height;

	}else if (scr_width == 1920 && scr_height == 1080)
	{
		layout1_width = scr_width;
		layout1_height = 80;
		grid2_width = scr_width;
		grid2_height = scr_height - layout1_height;
		layout3_width = 1522;
		layout3_height = scr_height - layout1_height;
		layout4_width = scr_width - layout3_width;
		layout4_height = scr_height - layout1_height;
		treeview_vm_width = layout3_width;
		treeview_vm_height = layout3_height;
	}else if (scr_width == 1440 && scr_height == 900 || scr_width == 1600 && scr_height == 900)
	{
		layout1_width = scr_width;
		layout1_height = 60;
		grid2_width = scr_width;
		grid2_height = scr_height - layout1_height;
		layout3_width = 1100;
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
	if (scr_width == 1024 && scr_height == 768)
	{
		gdk_pixbuf_get_file_info("images2/1024x768/loginout_nor.png", &pic_width, &pic_height);
		nspace_delay = 20;
	}else if (scr_width == 1920 && scr_height == 1080)
	{
		gdk_pixbuf_get_file_info(IMAGE_BTN_LOGINOUT_NOR, &pic_width, &pic_height);
		nspace_delay = 30;
	}else if (scr_width == 1440 && scr_height == 900 || scr_width == 1600 && scr_height == 900)
	{
		gdk_pixbuf_get_file_info("images2/1440x900/loginout_nor.png", &pic_width, &pic_height);
		nspace_delay = 20;
	}
	LogInfo("init_ctrl_posit 000 layout1_width=%d, pic_width=%d, pic_height=%d.\n", layout1_width, pic_width, pic_height);
	x = layout1_width - pic_width - ndelay;
	y = (layout1_height - pic_height)/2;
	gtk_widget_set_size_request(GTK_WIDGET(button_loginout), pic_width, pic_height);
	gtk_widget_set_size_request(GTK_WIDGET(button_exit), pic_width, pic_height);
	gtk_layout_move((GtkLayout *)layout1, GTK_WIDGET(button_exit), x, y);
	x = layout1_width - (pic_width * 2) - ndelay - nspace_delay;
	gtk_layout_move((GtkLayout *)layout1, GTK_WIDGET(button_loginout), x, y);
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
		surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1024x768/vmlistwindow.png");
	}else if (scr_width == 1920 && scr_height == 1080)
	{
		surface_vmlistwindow = cairo_image_surface_create_from_png (IMAGE_VMLISTWINDOW_BACKGROUD);
	}else if (scr_width == 1440 && scr_height == 900)
	{
		surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1440x900/vmlistwindow.png");
	}else if (scr_width == 1600 && scr_height == 900)
	{
		surface_vmlistwindow = cairo_image_surface_create_from_png ("images2/1600x900/vmlistwindow.png");
	}
}

static void destroy_surfaces()
{
	cairo_surface_destroy (surface_vmlistwindow);
}

void cleanVms()
{
    for (int i=0; i<MAX_BUFF_SIZE; i++)
    {
         memset(g_upVms[i].szvmid, 0, MAX_BUFF_SIZE * sizeof(char));
         g_upVms[i].state = 0;
    }
    g_vmsCount = 0;
}

void *thrd_checkstate(void *arg)
{
  //  printf("New process:  PID: %d,TID: %u.\n",getpid(),pthread_self()); //why pthread_self
    printf("New process checkstate:  PID: %d, TID: %u.\n", getpid(), tid_state); //why pthread_self
    LogInfo("debug: New process checkstate:  PID: %d, TID: %u.\n", getpid(), tid_state);
    for(;;)
    {
      vmstate_update();
      if (g_exitvm)
      {
         break;
      }
      sleep(8);
    }
	printf("exit tid_state thrd func .\n");
    pthread_exit(NULL); //退出线程
}

void *thrd_func(void *arg)
{
  //  printf("New process:  PID: %d,TID: %u.\n",getpid(),pthread_self()); //why pthread_self
  //  printf("New process11 :  PID: %d, TID: %u.\n", getpid(), tid); //why pthread_self
  //  LogInfo("debug: New process11 :  PID: %d, TID: %u.\n", getpid(), tid);
    for(;;)
    {
      if (g_sUpdateVmStatus)
          UpdateVmsStatus();
      if (g_exitvm)
      {
         break;
      }
	  //LogInfo(" Debug: vmlist window thrd_func 000 g_szUser : %s,  g_szPass: %s .\n", g_szUser, g_szPass);
      sleep(5);
    }
	printf("exit thrd func .\n");
    pthread_exit(NULL); //退出线程
}
//增加要更新的虚拟机到列表
unsigned short AddUpdateVms(char * vmid)
{
    for (int i=0; i<MAX_BUFF_SIZE; i++)
    {
        if (strcmp(g_upVms[i].szvmid, vmid) == 0)
           return -1;
    }
    for (int i=0; i<MAX_BUFF_SIZE; i++)
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
    printf("Add update vms 3333333  len : %d\n", g_vmsCount);
    for (int i = 0; i < MAX_BUFF_SIZE; i++)
    {
       if (strcmp(g_upVms[i].szvmid, "") > 0)
       {
           printf("Add update vms 3333333  vmid : %s\n", g_upVms[i].szvmid);
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
    printf("signal_handler count : %d\n", count);
}

void GetVmsId()
{
    list_for_each(plist, &head)
    {
        struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
        printf("connect vms g_vmName find vms id = %s.\n", g_vmName);
        if (strcmp(node->val.name, g_vmName) == 0)
        {
            memset(g_szVMid, 0, sizeof(g_szVMid));
            printf("vmlist window get vms id: %s\n", node->val.vmid);
            strcpy(g_szVMid, node->val.vmid);
        }
    }
}

int UpdateVmsStatus()
{
    for (int i=0; i<g_vmsCount; i++)
    {
         if (g_exitvm)
			break;
        //LogInfo(" Debug: vmlist window UpdateVmsStatus ovirt getvm2 g_szUser : %s,  g_szPass: %s .\n", g_szUser, g_szPass);
        if (Ovirt_GetVm2(ovirt_url, g_szUser, g_szPass, g_upVms[i].szvmid) < 0)
        {
            printf("Update vms status failed.\n");
            return -1;
        }
        int nstate = SY_GetVmState(g_upVms[i].szvmid);
        printf(" 333333 update vm id: %s,  vm state: %d .\n", g_upVms[i].szvmid, nstate);
        list_for_each(plist, &head)
        {
            if (g_exitvm)
				break;
            struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
            if (strcmp(node->val.vmid, g_upVms[i].szvmid) == 0)
            {
                printf("vms status : %d , nstate : %d.\n", node->val.status, nstate);
                char szTmp[20] = {0};
                if (node->val.status != nstate)
                {
                   printf("vmlist window UpdateVmsStatus vms name: %s, vms status : %d , nstate : %d.\n", node->val.name, node->val.status, nstate);
                    if (nstate == 0)
                        strcpy(szTmp, "关闭");
                    else if (nstate == 1)
                        strcpy(szTmp, "运行中");
                    else if (nstate == 2)
                        strcpy(szTmp, "待机");
                    else if (nstate == 3)
                        strcpy(szTmp, "正在启动");
                    else if (nstate == 4)
                        strcpy(szTmp, "正在关闭");
                    else if (nstate == 5)
                        strcpy(szTmp, "正在保存");
                    gtk_list_store_set(g_store, &g_upVms[i].iter, STATUS, (GValue *)szTmp, -1);
                    node->val.status = nstate;
                }//if
            }
        } //list
    }//for
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
          //printf("vmsstate_update: get vms failed.\n");
          LogInfo("debug: vmsstate_update: get vms failed.\n");
          return -1;
      }
      if (SY_GetVms2() < 0)
      {
          //printf("vmsstate_update: get vms xml data failed.\n");
          LogInfo("debug: vmsstate_update: get vms xml data failed.\n");
          return -1;
      }
      LogInfo("debug: vmsstate_update thrd enter 1111.\n");

      if (g_vmsComCount <= 0)
      {
          LogInfo("debug: vmsstate_update vms arrary is == null .\n");
          return -1;
      }
     for (int i=0; i<g_vmsComCount; i++)
     {
          list_for_each(plist, &head)
          {
                if (g_exitvm)
					break;
                //LogInfo("debug: vmsstate_update thrd enter 3333.\n");
                struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
                if (strcmp(node->val.vmid, g_vmsComUpdate[i].vmid) == 0 /*&&
                     strcmp(node->val.name, g_vmsComUpdate[i].name) == 0*/)
                {
                    //printf("vms old status : %d , new nstate : %d.\n", node->val.status, g_vmsComUpdate[i].status);
                    LogInfo("debug: vms status : %d , nstate : %d.\n", node->val.status, g_vmsComUpdate[i].status);
                    if (node->val.status != g_vmsComUpdate[i].status)
                    {
                        //do something;
                        //printf("vmlist window UpdateVmsStatus vms name: %s, vms old status : %d , new nstate : %d.\n", node->val.name, node->val.status, g_vmsComUpdate[i].status);
                        LogInfo("debug: vmlist window UpdateVmsStatus vms name: %s, vms old status : %d , new nstate : %d.\n", node->val.name, node->val.status, g_vmsComUpdate[i].status);
                        SetState(node->val.name, g_vmsComUpdate[i].status);
                        node->val.status = g_vmsComUpdate[i].status;
                    }
                }
          }//list
      } //for
      LogInfo("debug: vmsstate_update thrd enter 4444.\n");
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
    if (nstate == 0)
        strcpy(szTmp, "关闭");
    else if (nstate == 1)
        strcpy(szTmp, "运行中");
    else if (nstate == 2)
        strcpy(szTmp, "待机");
    else if (nstate == 3)
        strcpy(szTmp, "正在启动");
    else if (nstate == 4)
        strcpy(szTmp, "正在关闭");
    else if (nstate == 5)
        strcpy(szTmp, "正在保存");
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
        if (strcmp(node->val.name, g_vmName) == 0)
        {
            LogInfo("Debug: connectVms g_szUser: = %s,  g_szPass = %s.\n", g_szUser, g_szPass);
            Ovirt_GetVmTicket(ovirt_url, g_szUser, g_szPass, node->val.vmid);
            char szTicket[MAX_BUFF_SIZE] = {0};
            SY_GetVmsTicket(szTicket);
            //find vm
            sprintf(g_shellcmd, "spicy -h %s -p %d -w %s -f >> %s", node->val.ip, node->val.port, szTicket, "/var/log/shencloud/spicy.log");
            LogInfo("Debug:vm list window connect vms : %s. \n", g_shellcmd);
            system(g_shellcmd);
            break;
        }
    }
}

static void on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    switch(event->keyval)
    {
        case GDK_KEY_Return:
            {
             	g_sUpdateVmStatus = 0;
   				connectVms();
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
	gtk_widget_destroy((GtkWidget *)g_window);
    gtk_main_quit();
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

static void  on_btn_start_clicked(GtkButton *button,  gpointer   user_data)
{
   LogInfo("vml list window button start clicked, g_szUser = %s, g_szPass =%s.\n", g_szUser, g_szPass);
   if (Ovirt_StartVms(ovirt_url, g_szUser, g_szPass, g_szVMid) < 0)
   {
       //MsgDailog("启动虚拟机失败！");
       SYMsgDialog(7, "启动虚拟机失败！");
   }
   g_sUpdateVmStatus = 1;
   AddUpdateVms(g_szVMid);
}

static void  on_btn_close_clicked(GtkButton *button,  gpointer   user_data)
{
   printf("vml list window button close clicked.\n");
   if (Ovirt_ShutdownVms(ovirt_url, g_szUser, g_szPass, g_szVMid) < 0)
   {
      //MsgDailog("关闭虚拟机失败！");
      SYMsgDialog(7, "关闭虚拟机失败！");
   }
   g_sUpdateVmStatus = 1;
   AddUpdateVms(g_szVMid);
}

static void  on_btn_btn_desktop_clicked(GtkButton *button,  gpointer   user_data)
{
   printf("vml list window button desktop clicked.\n");
   g_sUpdateVmStatus = 0;
   connectVms();
}

static void  on_btn_sleep_clicked(GtkButton *button,  gpointer   user_data)
{
   printf("vml list window button sleep clicked.\n");
   if (Ovirt_SuspendVms(ovirt_url, g_szUser, g_szPass, g_szVMid) < 0)
   {
      //MsgDailog("虚拟机待机失败！");
      SYMsgDialog(7, "虚拟机待机失败！");
   }
   g_sUpdateVmStatus = 1;
   AddUpdateVms(g_szVMid);
}

static void  on_btn_logout_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginoutPress);
   //主意： 程序中使用同一个session
   //退回到启动界面
   g_sUpdateVmStatus = 0;
   g_exitvm = 1;
   showvmlistwindow = 0;
   cleanVms();
   gtk_widget_destroy((GtkWidget *)g_window);
   gtk_main_quit();
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
   printf("on_btn_shutdown_pressed .\n");
   SYMsgDialog(11, "您确定要关闭系统吗？");
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
   GtkTreeIter iter;
   char *value;
   if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)) {
       gtk_tree_model_get(model, &iter, NAME_1, &value, -1);
       //printf("list changed 4444444  NAME_1: %s\n", value);
       memset(g_vmName, 0, sizeof(g_vmName));
       memcpy(g_vmName, value, strlen(value));
       //printf("list changed 4444444  NAME_1  g_vmName: %s\n", value);
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
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_start), g_startNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_close), g_closeNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_desktop), g_desktopNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_sleep), g_sleepNor);
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
	int font_size = 12;
	
	if (scr_width == 1024 && scr_height == 768)
	{
		gdk_pixbuf_get_file_info("images2/1024x768/start.png", &pic_start_width, &pic_start_height);
		gdk_pixbuf_get_file_info("images2/1024x768/close.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1024x768/desktop.png", &pic_desktop_width, &pic_desktop_height);
		gdk_pixbuf_get_file_info("images2/1024x768/sleep.png", &pic_sleep_width, &pic_sleep_height);
		font_size = 8;
	}else if (scr_width == 1920 && scr_height == 1080)
	{
		gdk_pixbuf_get_file_info("images2/start.png", &pic_start_width, &pic_start_height);
		gdk_pixbuf_get_file_info("images2/close.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/desktop.png", &pic_desktop_width, &pic_desktop_height);
		gdk_pixbuf_get_file_info("images2/sleep.png", &pic_sleep_width, &pic_sleep_height);
		font_size = 12;
	}else if (scr_width == 1440 && scr_height == 900 || scr_width == 1600 && scr_height == 900)
	{
		gdk_pixbuf_get_file_info("images2/1440x900/start.png", &pic_start_width, &pic_start_height);
		gdk_pixbuf_get_file_info("images2/1440x900/close.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1440x900/desktop.png", &pic_desktop_width, &pic_desktop_height);
		gdk_pixbuf_get_file_info("images2/1440x900/sleep.png", &pic_sleep_width, &pic_sleep_height);
		font_size = 10;
	}
	setctrlFont(GTK_WIDGET(label_start), font_size);
	setctrlFont(GTK_WIDGET(label_close), font_size);
	setctrlFont(GTK_WIDGET(label_desktop), font_size);
	setctrlFont(GTK_WIDGET(label_sleep), font_size);
	//button and image
	gtk_widget_set_size_request(GTK_WIDGET(btn_start), pic_start_width, pic_start_height);
	gtk_widget_set_size_request(GTK_WIDGET(btn_close), pic_close_width, pic_close_height);
	gtk_widget_set_size_request(GTK_WIDGET(btn_desktop), pic_desktop_width, pic_desktop_height);
	gtk_widget_set_size_request(GTK_WIDGET(btn_sleep), pic_sleep_width, pic_sleep_height);
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
	gtk_widget_set_size_request(GTK_WIDGET(label_start), sz_start_width, sz_start_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_close), sz_close_width, sz_close_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_desktop), sz_desktop_width, sz_desktop_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_sleep), sz_sleep_width, sz_sleep_height);
	//cal ctrl position
	int center = lay_width/2;
	int top = 20;
	int delay = 10;
	int x,y = 0;
	x = center - delay/2 - sz_close_width/2;
	y = top;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_close), x, y);
	x = center - delay/2 - sz_close_width/2 - delay - pic_close_width;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_close), x, y);
	x = center - delay/2 - sz_close_width/2 - delay - pic_close_width - delay - sz_start_width;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_start), x, y);
	x = center - delay/2 - sz_close_width/2 - delay - pic_close_width - delay - pic_start_width - sz_start_width - delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_start), x, y);
	//dir right
	x = center + delay/2 + sz_close_width/2;
	y = top;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_desktop), x, y);
	x = center + delay/2 + sz_close_width/2 + pic_desktop_width + delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(label_desktop), x, y);
	x = center + delay/2 + sz_close_width/2 + pic_desktop_width + delay + sz_desktop_width + delay;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_sleep), x, y);
	x = center + delay/2 + sz_close_width/2 + pic_desktop_width + delay + sz_desktop_width + delay + pic_sleep_width + delay;
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
	}else if (scr_width == 1920 && scr_height == 1080 || 
				scr_width == 1440 && scr_height == 900 || 
				scr_width == 1600 && scr_height == 900)
	{
		*width = 30;
		*height = 20;
	}
}

static void setup_tree_view(GtkWidget*);
static void adddata();
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
                                         "font-size: 11px;\n"
                                         "padding: 0px;\n"
                                         "}\n"
                                         "GtkTreeView {\n"
                                         "border-style:none;\n"
                                         "font-size: 10px;\n"
                                         "color: #5C82A2;\n"
                                         "padding: 1px;\n", -1, NULL);
	}else if (scr_width == 1920 && scr_height == 1080)
	{
		gtk_css_provider_load_from_path (provider,
								 g_filename_to_utf8(g_home_css, strlen(g_home_css), &bytes_read, &bytes_written, &error),
								 NULL);
	}else if (scr_width == 1440 && scr_height == 900 || scr_width == 1600 && scr_height == 900)
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

	    g_exitNor = gdk_pixbuf_new_from_file("images2/1024x768/exit_nor.png", NULL);
	    g_exitPress = gdk_pixbuf_new_from_file("images2/1024x768/exit_press.png", NULL);
	    g_loginoutNor = gdk_pixbuf_new_from_file("images2/1024x768/loginout_nor.png", NULL);
	    g_loginoutPress = gdk_pixbuf_new_from_file("images2/1024x768/loginout_press.png", NULL);
		g_vmIconPix = gdk_pixbuf_new_from_file("images2/1024x768/vm_icon.png", NULL);
	}
	else if (scr_width == 1920 && scr_height == 1080)
	{
		g_startPress = gdk_pixbuf_new_from_file(IMAGE_BTN_START_PRES, NULL);
	    g_startNor = gdk_pixbuf_new_from_file(IMAGE_BTN_START_NOR, NULL);
	    g_closeNor = gdk_pixbuf_new_from_file(IMAGE_BTN_CLOSE_NOR, NULL);
	    g_closePress = gdk_pixbuf_new_from_file(IMAGE_BTN_CLOSE_PRES, NULL);
	    g_desktopNor = gdk_pixbuf_new_from_file(IMAGE_BTN_DESKTOP_NOR, NULL);
	    g_desktopPress = gdk_pixbuf_new_from_file(IMAGE_BTN_DESKTOP_PRES, NULL);
	    g_sleepNor = gdk_pixbuf_new_from_file(IMAGE_BTN_SLEEP_NOR, NULL);
	    g_sleepPress = gdk_pixbuf_new_from_file(IMAGE_BTN_SLEEP_PRES, NULL);

	    g_exitNor = gdk_pixbuf_new_from_file(IMAGE_BTN_EXIT_NOR, NULL);
	    g_exitPress = gdk_pixbuf_new_from_file(IMAGE_BTN_EXIT_PRES, NULL);
	    g_loginoutNor = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGINOUT_NOR, NULL);
	    g_loginoutPress = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGINOUT_PRES, NULL);
		g_vmIconPix = gdk_pixbuf_new_from_file(IMAGE_TREE_VMICON_NOR, NULL);
	}else if (scr_width == 1440 && scr_height == 900 || scr_width == 1600 && scr_height == 900)
	{
		g_startPress = gdk_pixbuf_new_from_file("images2/1440x900/start_press.png", NULL);
	    g_startNor = gdk_pixbuf_new_from_file("images2/1440x900/start.png", NULL);
	    g_closeNor = gdk_pixbuf_new_from_file("images2/1440x900/close.png", NULL);
	    g_closePress = gdk_pixbuf_new_from_file("images2/1440x900/close_press.png", NULL);
	    g_desktopNor = gdk_pixbuf_new_from_file("images2/1440x900/desktop.png", NULL);
	    g_desktopPress = gdk_pixbuf_new_from_file("images2/1440x900/desktop_press.png", NULL);
	    g_sleepNor = gdk_pixbuf_new_from_file("images2/1440x900/sleep.png", NULL);
	    g_sleepPress = gdk_pixbuf_new_from_file("images2/1440x900/sleep_press.png", NULL);

	    g_exitNor = gdk_pixbuf_new_from_file("images2/1440x900/exit_nor.png", NULL);
	    g_exitPress = gdk_pixbuf_new_from_file("images2/1440x900/exit_press.png", NULL);
	    g_loginoutNor = gdk_pixbuf_new_from_file("images2/1440x900/loginout_nor.png", NULL);
	    g_loginoutPress = gdk_pixbuf_new_from_file("images2/1440x900/loginout_press.png", NULL);
		g_vmIconPix = gdk_pixbuf_new_from_file("images2/1440x900/vm_icon.png", NULL);
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

    GObject *label_start;
    GObject *label_close;
    GObject *label_desktop;
    GObject *label_sleep;
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

    btn_start = gtk_builder_get_object (builder, "btn_start");
    btn_close = gtk_builder_get_object (builder, "btn_close");
    btn_desktop = gtk_builder_get_object (builder, "btn_desktop");
    btn_sleep = gtk_builder_get_object (builder, "btn_sleep");

    label_start = gtk_builder_get_object (builder, "label_start");
    label_close = gtk_builder_get_object (builder, "label_close");
    label_desktop = gtk_builder_get_object (builder, "label_desktop");
    label_sleep = gtk_builder_get_object (builder, "label_sleep");
	init_ctrl_posit(builder);
    create_surfaces();
	loadImage();
	loadcss();
    adddata();
	init_vmlistctrl_image(builder);
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(g_store));

    //读取登录信息
    Parsexml("user", g_szUser, 0);
    Parsexml("password", g_szPass, 0);

    GObject *label_login;
    label_login = gtk_builder_get_object (builder, "label_login");
    char *szTmp[MAX_BUFF_SIZE] = {0};
    sprintf(szTmp, "当前登录用户: %s", g_szUser);
    gtk_label_set_text(GTK_LABEL(label_login), szTmp);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    g_signal_connect(selection, "changed", G_CALLBACK(on_vmlist_changed), model);
    g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK (on_expose_vmlistevent), NULL);
    g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(on_gtk_main_quit_vmlist), NULL);

    //button event
    gtk_label_set_text(GTK_LABEL(label_start), "启动");
    gtk_label_set_text(GTK_LABEL(label_close), "关闭");
    gtk_label_set_text(GTK_LABEL(label_desktop), "桌面");
    gtk_label_set_text(GTK_LABEL(label_sleep), "待机");
    g_signal_connect(G_OBJECT(btn_start), "clicked", G_CALLBACK(on_btn_start_clicked), NULL);
    g_signal_connect(G_OBJECT(btn_close), "clicked", G_CALLBACK(on_btn_close_clicked), NULL);
    g_signal_connect(G_OBJECT(btn_desktop), "clicked", G_CALLBACK(on_btn_btn_desktop_clicked), NULL);
    g_signal_connect(G_OBJECT(btn_sleep), "clicked", G_CALLBACK(on_btn_sleep_clicked), NULL);
    //
    GObject *image_start;
    GObject *image_close;
    GObject *image_desktop;
    GObject *image_sleep;

    image_start = gtk_builder_get_object (builder, "image_start");
    image_close = gtk_builder_get_object (builder, "image_close");
    image_desktop = gtk_builder_get_object (builder, "image_desktop");
    image_sleep = gtk_builder_get_object (builder, "image_sleep");
    g_signal_connect(G_OBJECT(btn_start), "pressed", G_CALLBACK(on_btn_start_pressed), (GtkWidget *)image_start);
    g_signal_connect(G_OBJECT(btn_start), "released", G_CALLBACK(on_btn_start_released), (GtkWidget *)image_start);

    g_signal_connect(G_OBJECT(btn_close), "pressed", G_CALLBACK(on_btn_close_pressed), (GtkWidget *)image_close);
    g_signal_connect(G_OBJECT(btn_close), "released", G_CALLBACK(on_btn_close_released), (GtkWidget *)image_close);

    g_signal_connect(G_OBJECT(btn_desktop), "pressed", G_CALLBACK(on_btn_desktop_pressed), (GtkWidget *)image_desktop);
    g_signal_connect(G_OBJECT(btn_desktop), "released", G_CALLBACK(on_btn_desktop_released), (GtkWidget *)image_desktop);

    g_signal_connect(G_OBJECT(btn_sleep), "pressed", G_CALLBACK(on_btn_sleep_pressed), (GtkWidget *)image_sleep);
    g_signal_connect(G_OBJECT(btn_sleep), "released", G_CALLBACK(on_btn_sleep_released), (GtkWidget *)image_sleep);

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
    LogInfo("vmlistwindow screen width: %d, height: %d\n", g_screen_width, g_screen_height);
    g_signal_connect(G_OBJECT(g_window), \
                      "key-press-event", \
                      G_CALLBACK(on_key_press), NULL);
	gtk_window_fullscreen(GTK_WINDOW(window));
    //全屏显示结束

    g_sUpdateVmStatus = 0;
    //signal(SIGALRM, signal_handler);
    memset(g_upVms, 0, sizeof(g_upVms));

    //set_timer();
    // 创建线程tid，且线程函数由thrd_func指向，是thrd_func的入口点，即马上执行此线程函数
    if ( pthread_create(&tid, NULL, thrd_func, NULL) !=0 ) {
        printf("Create thread error!\n");
    }

    //开起向服务器检测虚拟机状态线程， 这与上面的线程处理不一样， 上一个是在启动，关闭，待机
    //请求后的处理线程。
  /*  if ( pthread_create(&tid_state, NULL, thrd_checkstate, NULL) !=0 ) {
        printf("Create checkstate thread error!\n");
    }*/
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
    g_object_unref (G_OBJECT (g_store));
	destroy_surfaces();
    showvmlistwindow = 0;
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
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("标签", renderer, "text", TAB, NULL);
    gtk_tree_view_column_set_fixed_width (column, columnwidth_array[3]);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
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
    column = gtk_tree_view_column_new_with_attributes("IP地址", renderer, "text", IP, NULL);
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
  /*
   * Create a new tree model with three columns, as string,
   * gint and guint.
   */
  GtkTreeIter  iter;
  g_store = gtk_list_store_new(COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                      G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  /* Add all of the products to the GtkListStore. */
  //vms data deal with
  //insert vms data into list
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
      char szStatus[MAX_BUFF_SIZE] = {0};
      char szMemory[MAX_BUFF_SIZE] = {0};
      char szUsb[MAX_BUFF_SIZE] = {0};
      char szvcpu[MAX_BUFF_SIZE] = {0};
      if (node->val.status == 1)
         memcpy(szStatus, "运行中", MAX_BUFF_SIZE);
      else if (node->val.status == 0)
         memcpy(szStatus, "关闭", MAX_BUFF_SIZE);
      else if (node->val.status == 2)
         memcpy(szStatus, "待机", MAX_BUFF_SIZE);

      sprintf(szMemory, "%ld GB", node->val.memory);

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
                        TAB, "测试",
                        STATUS, szStatus,
                        VCPU, szvcpu,
                        MEMORY, szMemory,
                        IP,  node->val.ip,
                        USB, szUsb,
                        -1);
  }
}
