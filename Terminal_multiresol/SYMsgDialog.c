#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include <unistd.h>

static const gchar*  g_symsgcss = "systhrdmg.css";
static GtkBuilder *builder = NULL;
static GObject  *window = NULL;
GtkLabel *labeltext;
static GThread * loginThrd = NULL;
static GThread * loginThrdxx = NULL;

extern void setctrlFont(GtkWidget * widget, int nsize);
extern int ShenCloud_autoLogin();
extern int ShenCloud_classLogin();


/*static pthread_t tid;
static void *thrdfunc(void *arg)
{
     SYMsgDialog();
}*/
void startSyMsgThrd()
{
  /*  if ( pthread_create(&tid, NULL, thrdfunc, NULL) !=0 )
    {
        printf("Create startSyMsgThrd error!\n");
    }*/
}
extern int SYMsgFun(char *p);
extern void SYmsgCaller(CallBackFun fun, char *p);
void SetSymsgContext(int msg);
extern void connectVms();
extern void clean_ctrlpass();
static int g_window_exit = 0;
static gboolean terminate(GThread *thread)
{
    g_thread_join(thread);
	if (window != NULL)
    {
	    gtk_widget_destroy((GtkWidget *)window);
		window = NULL;
	    gtk_main_quit();
    }
	if (g_loginstatus == LOGIN_SUCCESSED)
	{
		if (g_protype.showvmlist == 1)
		{
			if (g_selectProto == 0 && g_nVmCount >= 1)
			{
				g_loginstatus = 0;
				clean_ctrlpass();
			}
		}
		else
		{
			if (g_selectProto == 0 && g_nVmCount > 1)
		    {
		    	g_loginstatus = 0;
				clean_ctrlpass();
		    }
		}
	}
	if (g_loginstatus == LOGIN_SIGNELVM)
	{
	     if (g_protype.wuhudx == 1)
	     {
			 if (g_selectProto == 0 && g_nVmCount == 1)
			 {
			 	g_loginstatus = 0;
		 		clean_ctrlpass();
			 }
	     }
	}
    return FALSE;
}

static void * MsgThrd(GtkLabel *data){
  char sztmp[MAX_BUFF_SIZE] = {0};
  g_window_exit = 0;
  for(;;){
  	if (g_loginstatus == LOGIN_SUCCESSED || g_loginstatus == LOGIN_RETURN)
		break;
	if (g_window_exit == 1)
		break;
	//printf("SYMsgDialog MsgThrd g_loginstatus = %d .\n", g_loginstatus);
    switch (g_loginstatus) {
      case  LOGIN_STATUS_CONNECTING:
            strcpy(sztmp, "正在连接，请稍后 ... ");
            break;
      case LOGIN_STATUS_GETVMS:
            strcpy(sztmp, "连接成功，获取虚拟机信息！");
            break;
      case LOGIN_STATUS_FAILED:
            strcpy(sztmp, "登录失败！");
            break;
      case LOGIN_STATUS_GETVMS_FAILD:
            strcpy(sztmp, "获取虚拟机信息失败！");
            break;
      case LOGIN_STATUS_USER_PASS_ERROR:
            strcpy(sztmp, "用户名或密码错误！");
            break;
      case LOGIN_STATUS_CONNECT_FAILED_AIR:
            strcpy(sztmp, "连接失败，请确认信息！");
            break;
      case LOGIN_STATUS_IP_ERROR:
            strcpy(sztmp, "ip 地址错误！");
            break;
      case LOGIN_STATUS_IP_NONULL:
            strcpy(sztmp, "ip 地址不能为空！");
            break;
      case LOGIN_STATUS_PORT_NONULL:
            strcpy(sztmp, "端口不能为空！");
            break;
      case LOGIN_STATUS_USERNAME_NONULL:
            strcpy(sztmp, "用户名不能为空！");
            break;
      case LOGIN_STATUS_PASS_NONULL:
            strcpy(sztmp, "密码不能为空！");
            break;
      case SY_OVIRT_GETVMSTICKET_FAILED:
            strcpy(sztmp, "获取连接虚拟机密码失败！");
            break;
	  case LOGIN_USER_NORIGHT:
	  		strcpy(sztmp, "当前用户没有权限！");
	  		break;
    }
	if (g_loginstatus != 0)
	{
    	gtk_label_set_text(GTK_LABEL(data), sztmp);
	}
	g_loginstatus = 0;
    usleep(1000);
  }
  /* Make sure this thread is joined properly */
  gdk_threads_add_idle((GSourceFunc)terminate, g_thread_self());
  return NULL;
}

static gboolean terminate_login(GThread *thread)
{
	 g_thread_join(thread);
	 return FALSE;
}

static void * LoginThrdxx()
{
	if (ShenCloud_login() < 0)
	{
		g_loginsuccess = -1;
	}
	gdk_threads_add_idle((GSourceFunc)terminate_login, g_thread_self());
	return NULL;
}

static gboolean close_button_clicked(GtkButton *button,  gpointer user_data)
{
	g_window_exit = 1;
	if (NULL != loginThrd)
	{
		g_thread_join(loginThrd);
		loginThrd = NULL;
	}
	/*
	if (NULL != loginThrdxx)
	{
		g_thread_join(loginThrdxx);
		loginThrdxx = NULL;
	}*/
	
    if (window != NULL)
    {
    	g_window_exit = 1;
	    gtk_widget_destroy((GtkWidget *)window);
		window = NULL;
		showSyMsgDlg11 = 0;
	    gtk_main_quit();
    }
}

static gboolean OK_button_clicked(GtkButton *button,  gpointer user_data)
{
    if ((int)user_data == 20)
    {
        system("./manual_resol.sh");
        LogInfo("changed resolution, OK_button_clicked, software reboot.\n");
		char cmd[MAX_BUFF_SIZE] = {0};
		sprintf(cmd, "killall - 9 shencloud-ui \n exec ./shencloud-ui & ");
		system(cmd);
	}
	else
	{
#ifdef ARM
		system("sudo echo 1 > /sys/class/graphics/fb0/blank");
#endif
		system("sudo poweroff");
		if (window != NULL)
	    {
	    	g_window_exit = 1;
		    //gtk_widget_destroy((GtkWidget *)window);
			window = NULL;
			showSyMsgDlg11 = 0;
		    gtk_main_quit();
	    }
	}
}

static gboolean Cancel_button_clicked(GtkButton *button,  gpointer user_data)
{
    if (window != NULL)
    {
    	g_window_exit = 1;
	    gtk_widget_destroy((GtkWidget *)window);
		window = NULL;
		showSyMsgDlg11 = 0;
	    gtk_main_quit();
    	}
}

int SYMsgFun(char *p)
{
	SetSymsgContext(p);
	LogInfo("DFDFDFDFDDFD  SYMsgFun, p = %s.\n", p);
	if (strcmp(p, "登录完成!") == 0 || strcmp(p, "网络配置完成！") == 0)
	{
		if (window != NULL)
		{
		    gtk_widget_destroy((GtkWidget *)window);
			window = NULL;
		    gtk_main_quit();
		}
		LogInfo("DestorySYmsgDlg  exit.\n");
	}
	return 0;
}

void SYmsgCaller(CallBackFun fun, char *p)
{
   fun(p);
}


void SetSymsgContext(int msg)
{
	g_loginstatus = msg;
#if 0
//171122
	 char sztmp[MAX_BUFF_SIZE] = {0};
	 switch (g_loginstatus) {
      case  LOGIN_STATUS_CONNECTING:
            strcpy(sztmp, "正在连接，请稍后 ... ");
            break;
      case LOGIN_STATUS_GETVMS:
            strcpy(sztmp, "连接成功，获取虚拟机信息！");
            break;
      case LOGIN_STATUS_FAILED:
            strcpy(sztmp, "登录失败！");
            break;
      case LOGIN_STATUS_GETVMS_FAILD:
            strcpy(sztmp, "获取虚拟机信息失败！");
            break;
      case LOGIN_STATUS_USER_PASS_ERROR:
            strcpy(sztmp, "用户名或密码错误！");
            break;
      case LOGIN_STATUS_CONNECT_FAILED_AIR:
            strcpy(sztmp, "连接失败，请确认信息！");
            break;
      case LOGIN_STATUS_IP_ERROR:
            strcpy(sztmp, "ip 地址错误！");
            break;
      case LOGIN_STATUS_IP_NONULL:
            strcpy(sztmp, "ip 地址不能为空！");
            break;
      case LOGIN_STATUS_PORT_NONULL:
            strcpy(sztmp, "端口不能为空！");
            break;
      case LOGIN_STATUS_USERNAME_NONULL:
            strcpy(sztmp, "用户名不能为空！");
            break;
      case LOGIN_STATUS_PASS_NONULL:
            strcpy(sztmp, "密码不能为空！");
            break;
      case SY_OVIRT_GETVMSTICKET_FAILED:
            strcpy(sztmp, "获取连接虚拟机密码失败！");
            break;
	  case LOGIN_USER_NORIGHT:
	  		strcpy(sztmp, "当前用户没有权限！");
	  		break;
    }
	SYMsgDialog(7, sztmp);
#endif 
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
    screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	gtk_css_provider_load_from_path (provider, g_filename_to_utf8(g_symsgcss, strlen(g_symsgcss), &bytes_read, &bytes_written, &error), NULL);
    //gtk_css_provider_load_from_path (provider, home, NULL);
    g_object_unref (provider);
}

//nfalg 0:sh   1:mirsoft  2: cit  3:vm  4
static void init_ctrl_size(GtkBuilder *builder)
{
	GdkScreen* screen;
    screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	int win_width = 0;
	int win_height = 0;
//	if ((scr_width == 1024 && scr_height == 768) || (scr_width == 1440 && scr_height == 900) ||
//		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050))
//	{
//		win_width = 300;
//		win_height = 100;
//	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
//	{
//		win_width = 470;
//		win_height = 170;
//	}else if ( (scr_width == 1280 && scr_height == 720) || (scr_width == 1366 && scr_height == 768) ||
//		(scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768) || (scr_width == 1280 && scr_height == 768) ||
//		(scr_width == 1280 && scr_height == 1024))
//	{
//		win_width = 300;
//		win_height = 100;
//	}
	GObject *layout_thrdtitle = gtk_builder_get_object(builder, "layout_thrdtitle");
	GObject *layout_thrdtext = gtk_builder_get_object(builder, "layout_thrdtext");
	GObject *fixed_thrdmsg = gtk_builder_get_object(builder, "fixed_thrdmsg");
	GObject *btn_OK = gtk_builder_get_object(builder, "btn_OK");
	GObject *btn_Cancel = gtk_builder_get_object(builder, "btn_Cancel");
	GObject *label_thrdtext = gtk_builder_get_object(builder, "label_thrdtext");
	int btn_OK_width = 0;
	int btn_OK_height = 0;
	int btn_Cancel_width = 0;
	int btn_Cancel_height = 0;
	int label_text_width = 0;
	int label_text_height = 0;
	int layout_thrdtitle_height = 0;
	int layout_thrdtext_height = 0;
	int nsize = 0;
	if (scr_width == 1024 && scr_height == 768)
	{
		win_width = 300;
		win_height = 100;
		btn_OK_width = 30;
		btn_OK_height = 5;
		btn_Cancel_width = 30;
		btn_Cancel_height = 5;
		label_text_width = 220;
		label_text_height = 60;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 90;
		nsize = 8;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		win_width = 470;
		win_height = 160;
		btn_Cancel_width = 60;
		btn_Cancel_height = 20;
		btn_OK_width = 60;
		btn_OK_height = 20;
		label_text_width = 350;
		label_text_height = 80;
		layout_thrdtitle_height = 30;
		layout_thrdtext_height = 90;
		nsize = 12;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   ||
	  (scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050) )
	{
//		win_width = 470;
//		win_height = 160;
//		btn_Cancel_width = 60;
//		btn_Cancel_height = 20;
//		btn_OK_width = 60;
//		btn_OK_height = 20;
//		label_text_width = 350;
//		label_text_height = 80;
//		layout_thrdtitle_height = 30;
//		layout_thrdtext_height = 90;
//		nsize = 10;
		win_width = 300;
		win_height = 100;
		btn_OK_width = 30;
		btn_OK_height = 5;
		btn_Cancel_width = 30;
		btn_Cancel_height = 5;
		label_text_width = 220;
		label_text_height = 60;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 90;
		nsize = 9;
		
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	    (scr_width == 1280 && scr_height == 1024))
	{
		win_width = 300;
		win_height = 100;
		btn_OK_width = 30;
		btn_OK_height = 5;
		btn_Cancel_width = 30;
		btn_Cancel_height = 5;
		label_text_width = 220;
		label_text_height = 60;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 90;
		nsize = 9;
	}else if ( (scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) ||
		(scr_width == 1360 && scr_height == 768))
	{
		win_width = 300;
		win_height = 100;
		btn_OK_width = 30;
		btn_OK_height = 5;
		btn_Cancel_width = 30;
		btn_Cancel_height = 5;
		label_text_width = 220;
		label_text_height = 60;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 90;
		nsize = 9;
	}
	gtk_widget_set_size_request(GTK_WINDOW(window), win_width, win_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout_thrdtitle), win_width, layout_thrdtitle_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout_thrdtext), win_width, layout_thrdtext_height);
	gtk_widget_set_size_request(GTK_WIDGET(fixed_thrdmsg), win_width, win_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_thrdtext), label_text_width, label_text_height);
	//
	//
	gtk_widget_set_size_request(GTK_WIDGET(btn_OK), btn_OK_width, btn_OK_height);
	gtk_widget_set_size_request(GTK_WIDGET(btn_Cancel), btn_Cancel_width, btn_Cancel_height);
	setctrlFont(GTK_WIDGET(layout_thrdtext), nsize);
	setctrlFont(GTK_WIDGET(btn_OK), nsize);
	setctrlFont(GTK_WIDGET(btn_Cancel), nsize);
	GObject *image_syclose = gtk_builder_get_object(builder, "image_syclose");
	GObject *image_sylogo = gtk_builder_get_object(builder, "image_sylogo");
	GObject *btnsymsg_close = gtk_builder_get_object(builder, "btnsymsg_close");
	int space_left = 2;
	int space_right = 10;
	//get button pic width and height
	int x, y = 0;
	int pic_close_width = 0;
	int pic_close_height = 0;
	int pic_logo_width = 0;
	int pic_logo_height = 0;
	if ((scr_width == 1024 && scr_height == 768) || (scr_width == 1440 && scr_height == 900) ||
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050))
	{
		gdk_pixbuf_get_file_info("images2/1024x768/close_set_22.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1024x768/logo22.png", &pic_logo_width, &pic_logo_height);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		gdk_pixbuf_get_file_info("images2/close_set.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/logo.png", &pic_logo_width, &pic_logo_height);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 1024) ||
	    (scr_width == 1280 && scr_height == 768) || (scr_width == 1366 && scr_height == 768) ||
	    ( scr_width == 1368 && scr_height == 768 ) || (scr_width == 1360 && scr_height == 768))
	{
		gdk_pixbuf_get_file_info("images2/1024x768/close_set_22.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1024x768/logo22.png", &pic_logo_width, &pic_logo_height);
	}
	gtk_widget_set_size_request(GTK_WIDGET(image_syclose), pic_close_width, pic_close_height);
	gtk_widget_set_size_request(GTK_WIDGET(image_sylogo), pic_logo_width, pic_logo_height);
	gtk_widget_set_size_request(GTK_WIDGET(btnsymsg_close), pic_close_width, pic_close_height);
	x = space_left;
	y = 1;
	gtk_layout_move((GtkLayout *)layout_thrdtitle, GTK_WIDGET(image_sylogo), x, y);
	x = win_width - pic_close_width - space_right;
	y = 1;
	gtk_layout_move((GtkLayout *)layout_thrdtitle, GTK_WIDGET(image_syclose), x, y);
	//ok, cancel btn position
	x = win_width/2 - 10 - btn_OK_width;
	y = win_height - 2 - btn_OK_height;
	gtk_fixed_move((GtkFixed *)fixed_thrdmsg, GTK_WIDGET(btn_OK), x, y);
	x = win_width/2 + 10;
	y = win_height - 2 - btn_Cancel_height;
	gtk_fixed_move((GtkFixed *)fixed_thrdmsg, GTK_WIDGET(btn_Cancel), x, y);
	//label_thrdtext
	x = win_width/2  - label_text_width/2;
	y = 2;
	gtk_layout_move((GtkLayout *)layout_thrdtext, GTK_WIDGET(label_thrdtext), x, y);
	//close
	x = win_width - 10 - pic_close_width;
	y = (layout_thrdtitle_height - pic_close_height)/2;
	gtk_layout_move((GtkLayout *)layout_thrdtitle, GTK_WIDGET(btnsymsg_close), x, y);
	//image logo
	x = 1;
	y = (layout_thrdtitle_height - pic_logo_height)/2;
	gtk_layout_move((GtkLayout *)layout_thrdtitle, GTK_WIDGET(image_sylogo), x, y);
//	//move main dialog position
//	x = scr_width/2 - win_width/2;
//	y = scr_height/2 - win_height/2;
//	gtk_window_move(GTK_WINDOW(window), x, y);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);

}

static void init_ctrl_image(GtkBuilder *builder)
{
	GdkPixbuf *g_pixlogo;
	GdkPixbuf *g_pixclose;
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	if ((scr_width == 1024 && scr_height == 768) || (scr_width == 1440 && scr_height == 900) ||
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050))
	{
		g_pixlogo = gdk_pixbuf_new_from_file("images2/1024x768/logo22.png", NULL);
		g_pixclose = gdk_pixbuf_new_from_file("images2/1024x768/close_set_22.png", NULL);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		g_pixlogo = gdk_pixbuf_new_from_file("images2/logo22.png", NULL);
		g_pixclose = gdk_pixbuf_new_from_file("images2/close_set_22.png", NULL);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		g_pixlogo = gdk_pixbuf_new_from_file("images2/1024x768/logo22.png", NULL);
		g_pixclose = gdk_pixbuf_new_from_file("images2/1024x768/close_set_22.png", NULL);
	}else if ((scr_width == 1366 && scr_height == 768) || (scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		g_pixlogo = gdk_pixbuf_new_from_file("images2/1024x768/logo22.png", NULL);
		g_pixclose = gdk_pixbuf_new_from_file("images2/1024x768/close_set_22.png", NULL);
	}
	GObject *image_sylogo = gtk_builder_get_object (builder, "image_sylogo");
	GObject *image_syclose = gtk_builder_get_object(builder, "image_syclose");
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_sylogo), g_pixlogo);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_syclose), g_pixclose);
	if (g_interfacetype == 2)
	{
		gtk_widget_hide(GTK_WIDGET(image_sylogo));
	}
}

void disableclose(int val)
{
	GObject * btnsymsg_close;
    btnsymsg_close = gtk_builder_get_object(builder, "btnsymsg_close");
	gtk_widget_set_visible(GTK_WIDGET(btnsymsg_close), val);
	gtk_widget_set_sensitive(GTK_WIDGET(btnsymsg_close), val);
}
void SYMsgDialog(int nflag, char *msg)
{
    if (showSyMsgDlg11 == 1)
	{
	    printf("Debug: SYMsgDialog showSyMsgDlg==1 return .\n");
    	return;
	}
    showSyMsgDlg11 = 1;
	g_window_exit = 0;
	//printf("Debug: SYMsgDialog enter, showSyMsgDlg11 = %d.\n", showSyMsgDlg11);
    builder = gtk_builder_new();
    GError *error1 = NULL;
    gtk_builder_add_from_file (builder, "SYthrdMsgDlg.glade", &error1);
    window = gtk_builder_get_object(builder, "window_symsg");
    GObject * label_thrdtext;
    label_thrdtext = gtk_builder_get_object(builder, "label_thrdtext");
    labeltext = (GtkLabel *)label_thrdtext;
	loadcss();
	init_ctrl_size(builder);
	init_ctrl_image(builder);
    GObject * btnsymsg_close;
    btnsymsg_close = gtk_builder_get_object(builder, "btnsymsg_close");
    g_signal_connect(GTK_BUTTON(btnsymsg_close), "clicked", G_CALLBACK(close_button_clicked), NULL);
    /*g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(gtk_main_quit), NULL);*/
    //ok-cancel btn
    GObject * btn_OK;
    GObject * btn_Cancel;
    btn_OK = gtk_builder_get_object(builder, "btn_OK");
    btn_Cancel = gtk_builder_get_object(builder, "btn_Cancel");
    gtk_button_set_label(GTK_BUTTON(btn_OK), "是");
    gtk_button_set_label(GTK_BUTTON(btn_Cancel), "否");
	if (nflag == 7 || nflag == 11 || nflag == 4 || nflag == 20 || nflag == 23)
      	gtk_label_set_text(GTK_LABEL(label_thrdtext), msg);
    if (nflag == 11 || nflag == 20)
    {
		gtk_widget_set_visible(GTK_WIDGET(btn_OK), TRUE);
		gtk_widget_set_visible(GTK_WIDGET(btn_Cancel), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(btn_OK), TRUE);
		gtk_widget_set_sensitive(GTK_WIDGET(btn_Cancel), TRUE);
    }else
    {
		gtk_widget_set_visible(GTK_WIDGET(btn_OK), FALSE);
		gtk_widget_set_visible(GTK_WIDGET(btn_Cancel), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(btn_OK), FALSE);
		gtk_widget_set_sensitive(GTK_WIDGET(btn_Cancel), FALSE);
    }
	if (nflag == 0 || nflag == 1 || nflag == 21 || nflag == 23)
    {
		g_loginstatus = 1001;
		loginThrd = NULL;
		loginThrd = g_thread_new("loginthrd",(GThreadFunc)MsgThrd, (GtkLabel *)label_thrdtext);
		switch(nflag)
		{
			case 0:
				{
					if (ShenCloud_login() < 0)
					{
						g_loginsuccess = -1;
					}
					//add
					//loginThrdxx = NULL;
					//loginThrdxx = g_thread_new("shencloud",(GThreadFunc)LoginThrdxx, NULL);
					//end
				}
				break;
		}
    }
    g_signal_connect(GTK_BUTTON(btn_OK), "clicked", G_CALLBACK(OK_button_clicked), nflag);
    g_signal_connect(GTK_BUTTON(btn_Cancel), "clicked", G_CALLBACK(Cancel_button_clicked), NULL);
	gtk_window_set_modal (GTK_WINDOW (window), TRUE);
    gtk_main();
    g_object_unref (G_OBJECT(builder));
	builder = NULL;
	window = NULL;
	LogInfo("SYMsgDialog end ^^^^^^^.\n");
    showSyMsgDlg11 = 0;
}
