#include <math.h>
#include "global.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "msg.h"


#define MAX_CTRL_SIZE   10
#define IMAGE_BTN_SH_NOR           	  "images2/sh_nor.png"
#define IMAGE_BTN_SH_PRES          	  "images2/sh_press.png"

#define IMAGE_BTN_MIR_NOR          	  "images2/mirsoft_nor.png"
#define IMAGE_BTN_MIR_PRES         	  "images2/mirsoft_press.png"

#define IMAGE_BTN_SHUTDWON_NOR     	  "images2/exit_nor.png"
#define IMAGE_BTN_SHUTDWON_PRES    	  "images2/exit_press.png"

#define IMAGE_BTN_VMARE_NOR        	  "images2/vm_nor.png"
#define IMAGE_BTN_VMARE_PRES       	  "images2/vm_press.png"

#define IMAGE_BTN_CIT_NOR          	  "images2/cit_nor.png"
#define IMAGE_BTN_CIT_PRES         	  "images2/cit_press.png"

#define IMAGE_SETTING_NOR          	  "images2/set_nor.png"
#define IMAGE_SETTING_PRESS           "images2/set_press.png"

#define IMAGE_MAIN_BACKGROUD          "images2/login.png"
#define IMAGE_TOPWINDOW_BACKGROUD     "images2/topwindow.png"
#define IMAGE_VMLISTWINDOW_BACKGROUD  "images2/vmlistwindow.png"


//int base_width = 1920;
//int base_height = 1080;
int btn_inter_right = 80;
int btn_inter_bottom = 80;
int btn_space_width = 30;
int btn_width = 80;
int btn_height = 30;
GObject *psubCtrl[MAX_CTRL_SIZE] = {0};
int scr_top = 0;
int scr_layout2_height = 100;
int scr_layout4_height = 150;

static GdkPixbuf *g_shPress;
static GdkPixbuf *g_shNor;

static GdkPixbuf *g_mirPress;
static GdkPixbuf *g_mirNor;

static GdkPixbuf *g_vmarePress;
static GdkPixbuf *g_vmareNor;

static GdkPixbuf *g_citPress;
static GdkPixbuf *g_citNor;

static GdkPixbuf *g_setPress;
static GdkPixbuf *g_setNor;

static GdkPixbuf *g_shutdownPress;
static GdkPixbuf *g_shutdownNor;

GdkPixbuf *g_netstatus_Up;
GdkPixbuf *g_netstatus_Down;

static cairo_surface_t *surface1 = NULL;
static cairo_surface_t *surface_topwindow = NULL;
static cairo_surface_t *surface_vmlistwindow = NULL;
static GtkBuilder *g_builder;
static pthread_t tid;
static GObject *g_window;

extern void Net_status_checking(GtkBuilder *builder, GtkWidget *widget);
extern GdkPixbuf *create_pixbuf(const gchar * filename);
int g_mainExit = 0;
extern GObject *g_settingwindow;
static gint my_button_handler(GtkWidget *widget,GdkEvent *event);
extern int GetManufactureType();

void msg_respose_win(gpointer data)
{
	if (g_selectProto == 0 && g_nVmCount > 1)
	{
	    printf("terminate  44444444  2222222 8888.\n");
		report_msg.action = MSG_WINDOW_CLOSE;
		server_send_msg(report_msg);
		SY_vmlistwindow_main();
	}
}

static void setfontcolor(GtkWidget * widget, char *value)
{
	GdkRGBA color1;
	gdk_rgba_parse(&color1, value);
	gtk_widget_override_color(widget, GTK_STATE_FLAG_NORMAL, &color1);
}

static void setctrlFont(GtkWidget * widget, int nsize)
{
	PangoFontDescription *font1 = pango_font_description_from_string("Microsoft YaHei");
	pango_font_description_set_size(font1, nsize * PANGO_SCALE);
	gtk_widget_override_font(widget, font1);
}

static gboolean update_label (GtkLabel *label)
{
	//gtk_entry_progress_pulse (label);
	// time_t   timep;
	// time   (&timep);
	// printf( "%s ",ctime(&timep));
	struct tm *ptm;
	long ts;
	int y,m,d,h,n,s;
	ts = time(NULL);
	ptm = localtime(&ts);
	y = ptm-> tm_year+1900;     //��
	m = ptm-> tm_mon+1;        //��
	d = ptm-> tm_mday;         //��
	h = ptm-> tm_hour;         //ʱ
	n = ptm-> tm_min;          //��
	s = ptm-> tm_sec;          //��
	char sztime[100] = {0};
	sprintf(sztime, "%d : %02d", h, n);
	gtk_label_set_text(label, sztime);
	return FALSE;
}

static gboolean terminate (GThread *thread)
{
  g_thread_join (thread);
  return FALSE;
}

static void *thread_func (GtkLabel *label)
{
  for (;;)
    {
      //usleep (100000); /* 0.1 s */
      if (g_mainExit == 1)
	  	break;
      sleep(1);
      gdk_threads_add_idle ((GSourceFunc)update_label, label);
    }

  /* Make sure this thread is joined properly */
  gdk_threads_add_idle ((GSourceFunc)terminate, g_thread_self());
  return NULL;
}

static void init_sys_time_ctrl(GtkBuilder *builder, GtkWidget *widget)
{
	GtkWidget *label_time;
	label_time = gtk_label_new("88 : 88");
	GObject *layout2 = gtk_builder_get_object(builder, "layout2");
	int width = 0;
	int height = 0;
	gtk_widget_get_size_request(GTK_WIDGET(layout2), &width, &height);
	int x,y = 0;
	int label_width = 80;
	int label_height = 30;
	x = width - 20 - label_width;
	y = (height - label_height)/2;
	gtk_layout_put((GtkLayout *)widget, label_time, x, y);
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	int font_size = 0;
	if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
		font_size = 14;
	else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
		font_size = 12;
	else if (scr_width == 1024 && scr_height == 768)
		font_size = 12;
	else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
		font_size = 12;
	else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) || (scr_width == 1360 && scr_height == 768) )
		font_size = 12;
	setctrlFont(GTK_WIDGET(label_time), font_size);
    setfontcolor(GTK_WIDGET(label_time), "#ffffff");
	g_thread_new ("dummy", (GThreadFunc)thread_func, label_time);
}

static void *thrd_Vmarefunc(void *arg)
{
    int nRet = Run_VmwareView("admin", "123456", "127.0.0.1", "Test");
    if (nRet < 0)
    {
        LogInfo("Debug: login window connect vmare failed, nRet: %d. \n", nRet);
        SYMsgDialog(11, "vmare 登录失败!");
        return -1;
    }
}

//set Sensitive
void SettopSensitive(int sitive)
{
    GObject *btn_vm;
    GObject *btn_cit;
    GObject *btn_mir;
    GObject *btn_sh;
    GObject *btn_setting;
    GObject *btn_shutdown;
    btn_vm = gtk_builder_get_object(g_builder, "btn_vm");
    btn_cit = gtk_builder_get_object(g_builder, "btn_cit");
    btn_mir = gtk_builder_get_object(g_builder, "btn_mir");
    btn_sh = gtk_builder_get_object(g_builder, "btn_sh");
    btn_setting = gtk_builder_get_object(g_builder, "btn_setting");
    btn_shutdown = gtk_builder_get_object(g_builder, "btn_shutdown");
    gtk_widget_set_sensitive(GTK_WIDGET(btn_vm), sitive);
    gtk_widget_set_sensitive(GTK_WIDGET(btn_cit), sitive);
    gtk_widget_set_sensitive(GTK_WIDGET(btn_mir), sitive);
    gtk_widget_set_sensitive(GTK_WIDGET(btn_sh), sitive);
    gtk_widget_set_sensitive(GTK_WIDGET(btn_setting), sitive);
    gtk_widget_set_sensitive(GTK_WIDGET(btn_shutdown), sitive);
}

static void show_loginwindow()
{
	SY_loginwindow_main();
}

static void  on_btn_setting_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_setNor);
}

extern int showsettingwindow;

static void  on_btn_setting_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_setPress);
   SettopSensitive(0);
   SY_Setting_main();
   SettopSensitive(1);
}

static void  on_btn_setting_enter(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_setPress);
}

static void  on_btn_exit_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shutdownNor);
}

static void  on_btn_exit_pressed(GtkButton *button,  gpointer   user_data)
{
	gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shutdownPress);
	SettopSensitive(0);
	SYMsgDialog2(11, "您确定要关闭系统吗？");
	SettopSensitive(1);
}

static void  on_btn_exit_enter(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shutdownPress);
}

static void  on_btn_vm_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_vmareNor);
}

static void  on_btn_vm_pressed(GtkButton *button,  gpointer   user_data)
{
	gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_vmarePress);
	g_selectProto = 3;
	// �����߳�tid�����̺߳�����thrd_funcָ������thrd_func�����ڵ㣬������ִ�д��̺߳���
	if ( pthread_create(&tid, NULL, thrd_Vmarefunc, NULL) !=0 ) {
		printf("Create vmare thread error!\n");
	}
}

static void on_btn_vm_enter(GtkButton *button, gpointer  user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_vmarePress);
}

static void  on_btn_cit_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_citNor);
}

static void  on_btn_cit_pressed(GtkButton *button,  gpointer   user_data)
{
   	gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_citPress);
	printf("cit gtkimage check mouser button released.\n");
	g_selectProto = 2;
	if (g_citExit == 0)
		SY_logincit_main();
}

static void on_btn_cit_enter(GtkButton *button, gpointer  user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_citPress);
}

static void  on_btn_sh_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shNor);
}

static void  on_btn_sh_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shNor);
   g_selectProto = 0;
   show_loginwindow();
}

static void on_btn_sh_enter(GtkButton *button, gpointer  user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shPress);
}

static void  on_btn_mir_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_mirNor);
}

static void  on_btn_mir_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_mirPress);
   g_selectProto = 1;
   show_loginwindow();
}

static void on_btn_mir_enter(GtkButton *button, gpointer  user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_mirPress);
}

static void get_cirfour(int scr_width, int scr_height, int *cir_width, int *cir_height)
{
	if (scr_width == 1024 && scr_height == 768)
	{
	    *cir_width = 135;
		*cir_height = 135;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		*cir_width = 250;
		*cir_height = 250;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
	{
		*cir_width = 187;
		*cir_height = 187;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		*cir_width = 150;
		*cir_height = 150;
		//do code
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		//do code
		*cir_width = 178;
		*cir_height = 178;
	}
}

static void setctrl_size(GtkWidget *widget, int width, int height)
{
	gtk_widget_set_size_request(widget, width, height);
}

static void create_surfaces()
{
    LogInfo("debug: create_surfaces start load png.\n");
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200)  ||
		(scr_width == 1280 && scr_height == 1024) )
	{
	    surface1 = cairo_image_surface_create_from_png (IMAGE_MAIN_BACKGROUD);
	    if (surface1 == NULL)
	    {
	        LogInfo("Error: create_surfaces load loginwindow png failed.\n");
	    }
	    surface_topwindow = cairo_image_surface_create_from_png (IMAGE_TOPWINDOW_BACKGROUD);
	    if (surface_topwindow == NULL)
	    {
	        LogInfo("Error: create_surfaces load topwindow png failed.\n");
	    }

	    surface_vmlistwindow = cairo_image_surface_create_from_png (IMAGE_VMLISTWINDOW_BACKGROUD);
	    if (surface_vmlistwindow == NULL)
	    {
	        LogInfo("Error: create_surfaces load vmlistwindow png failed.\n");
	    }
	}else  if (scr_width == 1024 && scr_height == 768 )
	{
		surface1 = cairo_image_surface_create_from_png (IMAGE_MAIN_BACKGROUD);
	    if (surface1 == NULL)
	    {
	        LogInfo("Error: create_surfaces load loginwindow png failed.\n");
	    }
	    surface_topwindow = cairo_image_surface_create_from_png ("images2/1024x768/topwindow.png");
	    if (surface_topwindow == NULL)
	    {
	        LogInfo("Error: create_surfaces load topwindow png failed.\n");
	    }

	    surface_vmlistwindow = cairo_image_surface_create_from_png (IMAGE_VMLISTWINDOW_BACKGROUD);
	    if (surface_vmlistwindow == NULL)
	    {
	        LogInfo("Error: create_surfaces load vmlistwindow png failed.\n");
	    }
	}else if (scr_width == 1440 && scr_height == 900)
	{
		surface_topwindow = cairo_image_surface_create_from_png ("images2/1440x900/topwindow.png");
	    if (surface_topwindow == NULL)
	    {
	        LogInfo("Error: create_surfaces load topwindow png failed.\n");
	    }
	}else if ((scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
	{
		surface_topwindow = cairo_image_surface_create_from_png ("images2/1600x900/topwindow.png");
	    if (surface_topwindow == NULL)
	    {
	        LogInfo("Error: create_surfaces load topwindow png failed.\n");
	    }
		 LogInfo("debug: create_surfaces load png , current resolution is 1600x900.\n");
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768))
	{
         surface_topwindow = cairo_image_surface_create_from_png ("images2/1280x720/topwindow.png");
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
			(scr_width == 1360 && scr_height == 768) )
	{
		surface_topwindow = cairo_image_surface_create_from_png ("images2/1366x768/topwindow.png");
	}
    LogInfo("debug: create_surfaces load png end.\n");
}

static void destroy_surfaces()
{
    g_print ("destroying surfaces");
    LogInfo("debug: destroying surfaces.\n");
    cairo_surface_destroy (surface1);
    cairo_surface_destroy (surface_topwindow);
    cairo_surface_destroy (surface_vmlistwindow);
	//
	g_object_unref(g_shPress);
	g_object_unref(g_shNor);
	g_object_unref(g_mirPress);
	g_object_unref(g_mirNor);
	g_object_unref(g_vmarePress);
	g_object_unref(g_vmareNor);
	g_object_unref(g_shutdownPress);
	g_object_unref(g_shutdownNor);
	g_object_unref(g_citPress);
	g_object_unref(g_citNor);
	g_object_unref(g_setPress);
	g_object_unref(g_setNor);
}


//cal starup btn position
void calctrlposition(GObject *parent, GObject **pctrl)
{
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);

	int i=0;
	int nCount = 0;
	for (; i<MAX_CTRL_SIZE; i++)
	{
		if (*(pctrl + i) != 0)
		{
			nCount++;
		}
	}
	i=nCount;
	int ndex = 0;
	short first = 1;
	for(; i>=0; i--)
	{
		if (pctrl[i] != 0)
		{
		   ndex++;
		   if (first == 1)
		   {
		   	  gtk_layout_move((GtkLayout *)parent, GTK_WIDGET(pctrl[i]), scr_width - btn_inter_right - btn_width*ndex - btn_space_width*(ndex -1), scr_layout4_height - btn_inter_bottom - btn_height);

		   }else
		   	  gtk_layout_move((GtkLayout *)parent, GTK_WIDGET(pctrl[i]), scr_width - btn_inter_right - btn_width*ndex - btn_space_width*ndex/2, scr_layout4_height - btn_inter_bottom - btn_height);
		   printf("cal subctrl : x: =%d, y: =%d .\n", scr_width - btn_inter_right - btn_width*ndex - btn_space_width*ndex, scr_layout4_height - btn_inter_bottom - btn_height);
		   first = 2;
		}
	}
}

void cal_circlectrl_hide(GtkBuilder *builder)
{
	GObject *btn_vm;
	btn_vm = gtk_builder_get_object(builder, "btn_vm");
	GObject *btn_cit;
	btn_cit = gtk_builder_get_object(builder, "btn_cit");
	GObject *btn_mir;
	btn_mir = gtk_builder_get_object(builder, "btn_mir");
	if (g_interfacetype == 2)
	{
		gtk_widget_hide(GTK_WIDGET(btn_vm));
		gtk_widget_hide(GTK_WIDGET(btn_cit));
		gtk_widget_hide(GTK_WIDGET(btn_mir));
	}
}

//cal the height of the four circle area
void calcircleposition(GObject *parent, GtkBuilder *builder)
{
	int inter_space = 60;
	int pic_width = 250;
	int pic_height = 250;

	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
		 inter_space = 60;
	else if ((scr_width == 1024 && scr_height == 768) || (scr_width == 1440 && scr_height == 900) ||
		    (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
		 inter_space = 62;
	else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
		 inter_space = 62;
	else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
		 inter_space = 62;
	get_cirfour(scr_width, scr_height, &pic_width, &pic_height);
	printf("ddddddd   pic_width = %d, pic_height = %d .\n", pic_width, pic_height);
	int cir_height = scr_height - scr_layout4_height - scr_layout2_height;

	//resolution: 1920x1080
	int center = scr_width/2; //x
	int cir_y = cir_height/2 - pic_height/2;

	GObject *btn_vm;
	btn_vm = gtk_builder_get_object(builder, "btn_vm");
	GObject *btn_cit;
	btn_cit = gtk_builder_get_object(builder, "btn_cit");
	GObject *btn_mir;
	btn_mir = gtk_builder_get_object(builder, "btn_mir");
	GObject *btn_sh;
	btn_sh = gtk_builder_get_object(builder, "btn_sh");
	setctrl_size((GtkWidget *)btn_vm, pic_width, pic_height);
	setctrl_size((GtkWidget *)btn_cit, pic_width, pic_height);
	setctrl_size((GtkWidget *)btn_mir, pic_width, pic_height);
	setctrl_size((GtkWidget *)btn_sh, pic_width, pic_height);
	if (g_interfacetype == 2)
	{
		gtk_widget_set_sensitive(GTK_WIDGET(btn_vm), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(btn_cit), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(btn_mir), TRUE);
		int x = center - pic_width/2;
		gtk_layout_move((GtkLayout *)parent, GTK_WIDGET(btn_sh), x, cir_y);
	}
	else if (g_interfacetype == 0)
	{
		//left
		gtk_layout_move((GtkLayout *)parent, GTK_WIDGET(btn_vm),  center - inter_space/2 - pic_width*2 - inter_space, cir_y);
		gtk_layout_move((GtkLayout *)parent, GTK_WIDGET(btn_cit), center - inter_space/2 - pic_width, cir_y);
		//right
		gtk_layout_move((GtkLayout *)parent, GTK_WIDGET(btn_mir), center + inter_space/2, cir_y);
		gtk_layout_move((GtkLayout *)parent, GTK_WIDGET(btn_sh),  center + inter_space/2 + pic_width + inter_space, cir_y);
		int tmp_width = 0;
		int tmp_height = 0;
		gtk_widget_get_size_request((GtkWidget *)btn_vm, &tmp_width, &tmp_height);
		printf("ddddddd   tmp_width = %d, tmp_height = %d .\n", tmp_width, tmp_height);
	}

}


static void loadImage()
{
	//load png
	GdkScreen* screen;
    screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
	    g_shPress = gdk_pixbuf_new_from_file(IMAGE_BTN_SH_PRES, NULL);
	    g_shNor = gdk_pixbuf_new_from_file(IMAGE_BTN_SH_NOR, NULL);

	    g_mirPress = gdk_pixbuf_new_from_file(IMAGE_BTN_MIR_PRES, NULL);
	    g_mirNor = gdk_pixbuf_new_from_file(IMAGE_BTN_MIR_NOR, NULL);

	    g_vmarePress = gdk_pixbuf_new_from_file(IMAGE_BTN_VMARE_PRES, NULL);
	    g_vmareNor = gdk_pixbuf_new_from_file(IMAGE_BTN_VMARE_NOR, NULL);

	    g_shutdownPress = gdk_pixbuf_new_from_file(IMAGE_BTN_SHUTDWON_PRES, NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file(IMAGE_BTN_SHUTDWON_NOR, NULL);

	    g_citPress = gdk_pixbuf_new_from_file(IMAGE_BTN_CIT_PRES, NULL);
	    g_citNor = gdk_pixbuf_new_from_file(IMAGE_BTN_CIT_NOR, NULL);

	    g_setPress = gdk_pixbuf_new_from_file(IMAGE_SETTING_PRESS, NULL);
	    g_setNor = gdk_pixbuf_new_from_file(IMAGE_SETTING_NOR, NULL);

		g_netstatus_Up = gdk_pixbuf_new_from_file("images2/netstatus_up.png", NULL);
	    g_netstatus_Down = gdk_pixbuf_new_from_file("images2/netstatus_down.png", NULL);
	}else if (scr_width == 1024 && scr_height == 768)
	{
	    printf("dfdfdfdfdfdfdfdfdfdfdf.\n");
		g_shPress = gdk_pixbuf_new_from_file("images2/1024x768/sh_press.png", NULL);
	    g_shNor = gdk_pixbuf_new_from_file("images2/1024x768/sh_nor.png", NULL);

	    g_mirPress = gdk_pixbuf_new_from_file("images2/1024x768/mirsoft_press.png", NULL);
	    g_mirNor = gdk_pixbuf_new_from_file("images2/1024x768/mirsoft_nor.png", NULL);

	    g_vmarePress = gdk_pixbuf_new_from_file("images2/1024x768/vm_press.png", NULL);
	    g_vmareNor = gdk_pixbuf_new_from_file("images2/1024x768/vm_nor.png", NULL);

	    g_shutdownPress = gdk_pixbuf_new_from_file("images2/1024x768/exit_press.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1024x768/exit_nor.png", NULL);

	    g_citPress = gdk_pixbuf_new_from_file("images2/1024x768/cit_press.png", NULL);
	    g_citNor = gdk_pixbuf_new_from_file("images2/1024x768/cit_nor.png", NULL);

	    g_setPress = gdk_pixbuf_new_from_file("images2/1024x768/set_press.png", NULL);
	    g_setNor = gdk_pixbuf_new_from_file("images2/1024x768/set_nor.png", NULL);
		g_netstatus_Up = gdk_pixbuf_new_from_file("images2/1024x768/netstatus_up.png", NULL);
	    g_netstatus_Down = gdk_pixbuf_new_from_file("images2/1024x768/netstatus_down.png", NULL);
	}
	else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
	{
		g_shPress = gdk_pixbuf_new_from_file("images2/1440x900/sh_press.png", NULL);
	    g_shNor = gdk_pixbuf_new_from_file("images2/1440x900/sh_nor.png", NULL);

	    g_mirPress = gdk_pixbuf_new_from_file("images2/1440x900/mirsoft_press.png", NULL);
	    g_mirNor = gdk_pixbuf_new_from_file("images2/1440x900/mirsoft_nor.png", NULL);

	    g_vmarePress = gdk_pixbuf_new_from_file("images2/1440x900/vm_press.png", NULL);
	    g_vmareNor = gdk_pixbuf_new_from_file("images2/1440x900/vm_nor.png", NULL);

	    g_shutdownPress = gdk_pixbuf_new_from_file("images2/1440x900/exit_press.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1440x900/exit_nor.png", NULL);

	    g_citPress = gdk_pixbuf_new_from_file("images2/1440x900/cit_press.png", NULL);
	    g_citNor = gdk_pixbuf_new_from_file("images2/1440x900/cit_nor.png", NULL);

	    g_setPress = gdk_pixbuf_new_from_file("images2/1440x900/set_press.png", NULL);
	    g_setNor = gdk_pixbuf_new_from_file("images2/1440x900/set_nor.png", NULL);

		g_netstatus_Up = gdk_pixbuf_new_from_file("images2/1440x900/netstatus_up.png", NULL);
	    g_netstatus_Down = gdk_pixbuf_new_from_file("images2/1440x900/netstatus_down.png", NULL);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		g_shPress = gdk_pixbuf_new_from_file("images2/1280x720/sh_press.png", NULL);
	    g_shNor = gdk_pixbuf_new_from_file("images2/1280x720/sh_nor.png", NULL);

	    g_mirPress = gdk_pixbuf_new_from_file("images2/1280x720/mirsoft_press.png", NULL);
	    g_mirNor = gdk_pixbuf_new_from_file("images2/1280x720/mirsoft_nor.png", NULL);

	    g_vmarePress = gdk_pixbuf_new_from_file("images2/1280x720/vm_press.png", NULL);
	    g_vmareNor = gdk_pixbuf_new_from_file("images2/1280x720/vm_nor.png", NULL);

	    g_shutdownPress = gdk_pixbuf_new_from_file("images2/1280x720/exit_press.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1280x720/exit_nor.png", NULL);

	    g_citPress = gdk_pixbuf_new_from_file("images2/1280x720/cit_press.png", NULL);
	    g_citNor = gdk_pixbuf_new_from_file("images2/1280x720/cit_nor.png", NULL);

	    g_setPress = gdk_pixbuf_new_from_file("images2/1280x720/set_press.png", NULL);
	    g_setNor = gdk_pixbuf_new_from_file("images2/1280x720/set_nor.png", NULL);

		g_netstatus_Up = gdk_pixbuf_new_from_file("images2/1280x720/netstatus_up.png", NULL);
	    g_netstatus_Down = gdk_pixbuf_new_from_file("images2/1280x720/netstatus_down.png", NULL);
		//do code
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		//do code
	    g_shPress = gdk_pixbuf_new_from_file("images2/1366x768/sh_press.png", NULL);
	    g_shNor = gdk_pixbuf_new_from_file("images2/1366x768/sh_nor.png", NULL);

	    g_mirPress = gdk_pixbuf_new_from_file("images2/1366x768/mirsoft_press.png", NULL);
	    g_mirNor = gdk_pixbuf_new_from_file("images2/1366x768/mirsoft_nor.png", NULL);

	    g_vmarePress = gdk_pixbuf_new_from_file("images2/1366x768/vm_press.png", NULL);
	    g_vmareNor = gdk_pixbuf_new_from_file("images2/1366x768/vm_nor.png", NULL);

	    g_shutdownPress = gdk_pixbuf_new_from_file("images2/1366x768/exit_press.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1366x768/exit_nor.png", NULL);

	    g_citPress = gdk_pixbuf_new_from_file("images2/1366x768/cit_press.png", NULL);
	    g_citNor = gdk_pixbuf_new_from_file("images2/1366x768/cit_nor.png", NULL);

	    g_setPress = gdk_pixbuf_new_from_file("images2/1366x768/set_press.png", NULL);
	    g_setNor = gdk_pixbuf_new_from_file("images2/1366x768/set_nor.png", NULL);

		g_netstatus_Up = gdk_pixbuf_new_from_file("images2/1366x768/netstatus_up.png", NULL);
	    g_netstatus_Down = gdk_pixbuf_new_from_file("images2/1366x768/netstatus_down.png", NULL);
	}
}

void setcirctrl_image(GtkBuilder *builder)
{
	GObject *image_vm;
	GObject *image_cit;
	GObject *image_mirsoft;
	GObject *image_sh;
    image_vm = gtk_builder_get_object (builder, "image_vm");
    image_cit = gtk_builder_get_object (builder, "image_cit");
    image_mirsoft = gtk_builder_get_object (builder, "image_mirsoft");
    image_sh = gtk_builder_get_object (builder, "image_sh");
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_vm), g_vmareNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_cit), g_citNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_mirsoft), g_mirNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_sh), g_shNor);
	//shutdown button and setting button
	GObject *image_shutdown;
	GObject *image_setting;
	image_shutdown = gtk_builder_get_object (builder, "image_shutdown");
    image_setting = gtk_builder_get_object (builder, "image_setting");
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_shutdown), g_shutdownNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_setting), g_setNor);
}

static void signal_ctrlevent(GtkBuilder *builder)
{
	//signal event
	GObject *image_shutdown;
	GObject *image_setting;
	image_shutdown = gtk_builder_get_object(builder, "image_shutdown");
    image_setting = gtk_builder_get_object(builder, "image_setting");
	GObject *btn_set;
	GObject *btn_exit;
	btn_set = gtk_builder_get_object(builder, "btn_setting");
	btn_exit = gtk_builder_get_object(builder, "btn_shutdown");
	g_signal_connect(G_OBJECT(btn_set), "pressed", G_CALLBACK(on_btn_setting_pressed), (GtkWidget *)image_setting);
    g_signal_connect(G_OBJECT(btn_set), "leave", G_CALLBACK(on_btn_setting_leave), (GtkWidget *)image_setting);
	g_signal_connect(G_OBJECT(btn_set), "enter", G_CALLBACK(on_btn_setting_enter), (GtkWidget *)image_setting);
	g_signal_connect(G_OBJECT(btn_exit), "pressed", G_CALLBACK(on_btn_exit_pressed), (GtkWidget *)image_shutdown);
    g_signal_connect(G_OBJECT(btn_exit), "leave", G_CALLBACK(on_btn_exit_leave), (GtkWidget *)image_shutdown);
	g_signal_connect(G_OBJECT(btn_exit), "enter", G_CALLBACK(on_btn_exit_enter), (GtkWidget *)image_shutdown);
	GObject *image_vm;
	GObject *image_cit;
	GObject *image_mirsoft;
	GObject *image_sh;
    image_vm = gtk_builder_get_object (builder, "image_vm");
    image_cit = gtk_builder_get_object (builder, "image_cit");
    image_mirsoft = gtk_builder_get_object (builder, "image_mirsoft");
    image_sh = gtk_builder_get_object (builder, "image_sh");
	GObject *btn_vm;
	GObject *btn_cit;
	GObject *btn_mir;
	GObject *btn_sh;
	btn_vm = gtk_builder_get_object(builder, "btn_vm");
	btn_cit = gtk_builder_get_object(builder, "btn_cit");
	btn_mir = gtk_builder_get_object(builder, "btn_mir");
	btn_sh = gtk_builder_get_object(builder, "btn_sh");
	g_signal_connect(G_OBJECT(btn_vm), "pressed", G_CALLBACK(on_btn_vm_pressed), (GtkWidget *)image_vm);
    g_signal_connect(G_OBJECT(btn_vm), "leave", G_CALLBACK(on_btn_vm_leave), (GtkWidget *)image_vm);
	g_signal_connect(G_OBJECT(btn_vm), "enter", G_CALLBACK(on_btn_vm_enter), (GtkWidget *)image_vm);
	g_signal_connect(G_OBJECT(btn_cit), "pressed", G_CALLBACK(on_btn_cit_pressed), (GtkWidget *)image_cit);
    g_signal_connect(G_OBJECT(btn_cit), "leave", G_CALLBACK(on_btn_cit_leave), (GtkWidget *)image_cit);
	g_signal_connect(G_OBJECT(btn_cit), "enter", G_CALLBACK(on_btn_cit_enter), (GtkWidget *)image_cit);
	g_signal_connect(G_OBJECT(btn_mir), "pressed", G_CALLBACK(on_btn_mir_pressed), (GtkWidget *)image_mirsoft);
    g_signal_connect(G_OBJECT(btn_mir), "leave", G_CALLBACK(on_btn_mir_leave), (GtkWidget *)image_mirsoft);
	g_signal_connect(G_OBJECT(btn_mir), "enter", G_CALLBACK(on_btn_mir_enter), (GtkWidget *)image_mirsoft);
	g_signal_connect(G_OBJECT(btn_sh), "pressed", G_CALLBACK(on_btn_sh_pressed), (GtkWidget *)image_sh);
    g_signal_connect(G_OBJECT(btn_sh), "leave", G_CALLBACK(on_btn_sh_leave), (GtkWidget *)image_sh);
	g_signal_connect(G_OBJECT(btn_sh), "enter", G_CALLBACK(on_btn_sh_enter), (GtkWidget *)image_sh);
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
                                  g_filename_to_utf8(g_home_css, strlen(g_home_css), &bytes_read, &bytes_written, &error),
                                  NULL);
    //gtk_css_provider_load_from_path (provider, home, NULL);
    g_object_unref (provider);
}

//draw background
gboolean on_draw_event (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    //g_print("on_draw_event start.\n");
    //LogInfo("debug: on_draw_event start.\n");
    GdkWindow *window;
    cairo_t *cr = NULL;
    window = gtk_widget_get_window(widget);
    cr = gdk_cairo_create(window);
    cairo_set_source_surface (cr, surface_topwindow, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}

static void show_netInfo_window()
{
	printf("Debug : show_netInfo_window(). \n");
	SYNetInfoWin();
}


static void init_ctrlbtn_pos(GtkBuilder *builder, GtkLayout * widget, int layout4_height)
{
	//cal subctrl button
	GdkScreen* screen;
    screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	int picset_width = 0;
	int picset_height = 0;
	int picexit_width = 0;
	int picexit_height = 0;
	int picnet_width = 0;
	int picnet_height = 0;
	GObject *button1;
	GObject *button2;
	GObject *btn_netstatus;
	button1 = gtk_builder_get_object (builder, "btn_setting");
	button2 = gtk_builder_get_object (builder, "btn_shutdown");
	btn_netstatus = gtk_builder_get_object (builder, "btn_netstatus");
	int nspace_width = 40;
	int ndelay = 30;
	if (scr_width == 1024 && scr_height == 768)
	{
		gdk_pixbuf_get_file_info("images2/1024x768/set_nor.png", &picset_width, &picset_height);
		gdk_pixbuf_get_file_info("images2/1024x768/exit_nor.png", &picexit_width, &picexit_height);
		gdk_pixbuf_get_file_info("images2/1024x768/netstatus_up.png", &picnet_width, &picnet_height);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		gdk_pixbuf_get_file_info(IMAGE_SETTING_NOR, &picset_width, &picset_height);
		gdk_pixbuf_get_file_info(IMAGE_BTN_SHUTDWON_NOR, &picexit_width, &picexit_height);
		gdk_pixbuf_get_file_info("images2/netstatus_up.png", &picnet_width, &picnet_height);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
	{
		gdk_pixbuf_get_file_info("images2/1440x900/set_nor.png", &picset_width, &picset_height);
		gdk_pixbuf_get_file_info("images2/1440x900/exit_nor.png", &picexit_width, &picexit_height);
		gdk_pixbuf_get_file_info("images2/1440x900/netstatus_up.png", &picnet_width, &picnet_height);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		gdk_pixbuf_get_file_info("images2/1280x720/set_nor.png", &picset_width, &picset_height);
		gdk_pixbuf_get_file_info("images2/1280x720/exit_nor.png", &picexit_width, &picexit_height);
		gdk_pixbuf_get_file_info("images2/1280x720/netstatus_up.png", &picnet_width, &picnet_height);
		//do code
	}else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		gdk_pixbuf_get_file_info("images2/1366x768/set_nor.png", &picset_width, &picset_height);
		gdk_pixbuf_get_file_info("images2/1366x768/exit_nor.png", &picexit_width, &picexit_height);
		gdk_pixbuf_get_file_info("images2/1366x768/netstatus_up.png", &picnet_width, &picnet_height);
		//do code
	}
	setctrl_size((GtkWidget *)button2, picexit_width, picexit_height);
	setctrl_size((GtkWidget *)button1, picset_width, picset_height);
	setctrl_size((GtkWidget *)btn_netstatus, picnet_width, picnet_height);
	//exit button
	int x = scr_width - nspace_width - picexit_width;
	int y = 0;
	if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	    y = (layout4_height - picexit_height)/2 + 50;
	else if ((scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768) || (scr_width == 1366 && scr_height == 768))
		y = (layout4_height - picexit_height)/2 + 40;
	else
		y = (layout4_height - picexit_height)/2 + 20;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(button2), x, y);
	//set button
	x = scr_width - nspace_width - picexit_width - ndelay - picset_width;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(button1), x, y);
	//net button
	x = scr_width - nspace_width - picexit_width - ndelay - picset_width - ndelay - picnet_width;
	gtk_layout_move((GtkLayout *)widget, GTK_WIDGET(btn_netstatus), x, y);
	GtkWidget *menu_item;
	//GtkWidget *menubar;
	//GtkWidget *vbox;
	//vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
	//gtk_container_add(GTK_WINDOW(g_window), vbox);
	//menubar = gtk_menu_bar_new();
	//gtk_box_pack_start((GtkBox *)vbox, menubar, FALSE, FALSE, 0);
	//menu_item = gtk_menu_item_new_with_label("Network Info");
	//gtk_menu_shell_append(GTK_MENU_BAR(menubar),menu_item);
	GtkWidget * menu = gtk_menu_new();
	//gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item),menu);
	menu_item = gtk_menu_item_new_with_label("详细信息");
	gtk_menu_shell_append(GTK_MENU(menu), menu_item);
	//gtk_widget_show(vbox);
	//gtk_widget_show(menubar);
	gtk_widget_show(menu_item);
	gtk_widget_show(menu);
	g_signal_connect_swapped(G_OBJECT(btn_netstatus),"button_press_event",G_CALLBACK(my_button_handler),G_OBJECT(menu));
	g_signal_connect(GTK_MENU_ITEM(menu_item), "activate", G_CALLBACK(show_netInfo_window),"详细信息");
}

//gboolean window_press_event(GtkWidget *widget, GdkEventButton *event, GdkWindowEdge edge)
//{
//    if (event->button == 1)
//    {
//         if (showsettingwindow == 1)
//         {
//             printf("^^^^^^^^^^^^^^^^^^^^^^.\n");
//			gtk_window_set_keep_above(GTK_WINDOW(g_window), FALSE);
//			gtk_window_set_keep_above(GTK_WINDOW(g_settingwindow), TRUE);
//		 	return FALSE;
//         }
//    }
//    return FALSE;
//}

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


int main(int argc, char *argv[])
{
	gtk_init (&argc, &argv);
	system("sudo chmod +x tmp.sh");
    system("sudo ./tmp.sh");
    system("sudo chmod +x netstatic.sh");
    system("sudo chmod +x dhcp.sh");
    system("sudo chmod +x netget.sh");
    system("sudo xrandr > resol.txt");
    //print version info
    g_mainExit = 0;
	g_interfacetype = GetManufactureType(); //ÖÚ¶¹ÔÆ
    Init_Curlc();
	Checking_log();
	SetLogFlag(1);
	//calculate screen resolution
    GdkScreen* screen;
    screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	int scr_layout3_height = scr_height - scr_layout2_height - scr_layout4_height; //The height of the four circle area

	GtkBuilder *builder;
	GObject *window;
	GError *error = NULL;
	builder = gtk_builder_new();
	int nRet = gtk_builder_add_from_file (builder, "windowts.glade", &error);
	window = gtk_builder_get_object (builder, "window1");
	g_window = window;
	g_mainWindow = window;
	g_builder = builder;
	loadcss();
	create_surfaces();
	loadImage();
	g_citExit = 0;
	//gtk_window_resize(GTK_WINDOW(window), 800, 600);
	gtk_widget_set_app_paintable(GTK_WIDGET(window), TRUE);
	g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK (on_draw_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy",
	                       G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_set_size_request(GTK_WINDOW(window), scr_width, scr_height);
	//gtk_window_set_default_size(GTK_WINDOW(window), scr_width, scr_height);

    GtkAllocation allocat;
	GObject *layout1;
	layout1 = gtk_builder_get_object(builder, "layout1");
	GObject *layout2;
	layout2 = gtk_builder_get_object(builder, "layout2");
	GObject *layout3;
	layout3 = gtk_builder_get_object(builder, "layout3");
	//cal layout1
	//GdkRGBA color1;
	//gdk_rgba_parse(&color1, "#FF7F24");
	//gtk_widget_override_background_color(GTK_WIDGET(layout1), GTK_STATE_FLAG_NORMAL, &color1);
	gtk_widget_set_size_request(GTK_WIDGET(layout1), scr_width, scr_height);
	//cal layout2;
	//GdkRGBA color2;
	//gdk_rgba_parse(&color2, "#5F9EA0");
	//gtk_widget_override_background_color(GTK_WIDGET(layout2), GTK_STATE_FLAG_NORMAL, &color2);
	gtk_widget_set_size_request(GTK_WIDGET(layout2), scr_width, scr_layout2_height);
	//cal layout3;
	//GdkRGBA color3;
	//gdk_rgba_parse(&color3, "#66CDAA");
	//gtk_widget_override_background_color(GTK_WIDGET(layout3), GTK_STATE_FLAG_NORMAL, &color3);
	gtk_widget_set_size_request(GTK_WIDGET(layout3), scr_width, scr_layout3_height);
	//cal layout4
	GObject *layout4;
	//GdkRGBA color;
	//gdk_rgba_parse(&color, "#BBFFFF");
	layout4 = gtk_builder_get_object (builder, "layout4");
	gtk_widget_get_allocation(GTK_WIDGET(layout4), &allocat);
	//gtk_widget_override_background_color(GTK_WIDGET(layout4), GTK_STATE_FLAG_NORMAL, &color);
	gtk_widget_set_size_request(GTK_WIDGET(layout4), scr_width, scr_layout4_height);
	GValue a = G_VALUE_INIT;
	g_value_init (&a, G_TYPE_INT);
	g_value_set_int (&a, scr_height - scr_layout4_height);
	gtk_container_child_set_property(GTK_CONTAINER(layout1), GTK_WIDGET(layout4), "y", &a);
	//printf("widget layout4, x: =%d, y:=%d, width :=%d, height :=%d \n .", allocation_layout4.x, allocation_layout4.y, allocation_layout4.width, allocation_layout4.height);
    init_ctrlbtn_pos(builder, layout4, scr_layout4_height);
	calcircleposition(layout3, builder); //cal  four cir
	setcirctrl_image(builder);
	signal_ctrlevent(builder);
	init_sys_time_ctrl(builder, layout2);
	server_init(); //start msg server thrd
	Net_status_checking(builder, layout4);
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("images2/logo.png"));
	gtk_window_fullscreen(GTK_WINDOW(window));
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE); /* hide the title bar and the boder */
	//g_signal_connect(G_OBJECT(window), "button-press-event", G_CALLBACK(window_press_event), NULL);
	gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);                    
	 gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
	gtk_widget_show_all((GtkWidget *)window);
	if (g_interfacetype == 2)
	{
		cal_circlectrl_hide(GTK_WIDGET(builder));
	}
//	static struct LoginInfo infot = {"", "", "", "", 3389, 0, 0, 0};
//	GetLoginInfo(&infot);
//	if (infot.autologin == 1)
//	{
//		wait_net_setup();
//	}
	//SYMsgDialog2(23, "æ­£åœ¨è¿žæŽ¥ï¼Œ è¯·ç¨åŽ...");
	start_monitor_file_thrd();
	gtk_main();
    g_object_unref(G_OBJECT(builder));
	destroy_surfaces();
	Unit_Curlc();
    system("rm -rf resol.txt");
	g_mainExit = 1;
	CloseLog();
	close_msg_server(); //close msg server thrd
	return 0;
}
