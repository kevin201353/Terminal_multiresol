#include "global.h"
#include "interface.h"
#include <unistd.h>

#define IMAGE_BTN_LOGIN_NOR      "images2/btnlogin_nor.png"
#define IMAGE_BTN_LOGIN_PRES     "images2/btnlogin_press.png"
#define IMAGE_CIT_BACKGROUD      "images2/cit_login.png"  //登录界面背景图路径
#define IMAGE_BTN_CITPREV_NOR    "images2/loginout_nor.png"
#define IMAGE_BTN_CITPREV_PRES   "images2/loginout_press.png"
#define IMAGE_LOGO_CIT           "images2/cit_logo.png"
#define IMAGE_BTN_SHUTDWON_NOR   "images2/exit_nor.png"
#define IMAGE_BTN_SHUTDWON_PRES  "images2/exit_press.png"


static char *g_cit_css = "citlogin.css";
static GObject *g_window = NULL;
static GdkPixbuf *g_shutdownPress;
static GdkPixbuf *g_shutdownNor;
GdkPixbuf *g_loginPress;
GdkPixbuf *g_loginNor;
GdkPixbuf *g_prevPress;
GdkPixbuf *g_prevNor;
GdkPixbuf *g_logoCit;
static cairo_surface_t *surface ;
static GtkBuilder *g_builder;
extern void MsgDailog(char * sMsg);
static char g_szURL[MAX_BUFF_SIZE] = {0};
static int showlogincitwindow = 0;
//CallBackFun
extern void SYmsgCaller(CallBackFun fun, char *p);
extern int SYMsgFun(char *p);
//
/*static pthread_t tid;
static void *thrd_citfunc(void *arg)
{
    cit_login(g_szURL);
}*/
void cit_login2();
extern GdkPixbuf *create_pixbuf(const gchar * filename);
static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    switch(event->keyval)
    {
        case GDK_KEY_Return:
            {
               cit_login2();
            }
            break;
    }
    return FALSE;
}

int cit_login()
{
    if(vfork() == 0)
    {
//       if (Run_Citrix(g_szURL) < 0)
//       {
//          //MsgDailog("登录失败！");
//          SYMsgDialog2(7, "登录失败！");
//          return -1;
//       }
    }else
    {
       printf("This is the parent process, dddd.\n");
    }
    return 0;
}

//为登录主窗口绘制背景图
static gboolean on_expose_loginevent (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    GdkWindow *window;
    cairo_t *cr;
    window = gtk_widget_get_window(widget); //gtk3中GtkWidget中已经没有window成员了
    cr = gdk_cairo_create(window);
    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}

static gboolean gtk_main_quit_login(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
      gtk_main_quit();
      showlogincitwindow = 0;
      return TRUE;
}

static void  on_btn_prev_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevPress);
   showlogincitwindow = 0;
   gtk_widget_destroy((GtkWidget *)g_window);
   gtk_main_quit();
   g_citExit = 0;
}

static void  on_btn_prev_leave(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevNor);
}

static void  on_btn_prev_enter(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevPress);
}


static void  on_btn_shutdown_pressed(GtkButton *button,  gpointer   user_data)
{
    gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_shutdownPress);
	//MsgShutDownDailog("您确定要关闭系统吗？");
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


static void  on_btn_login_pressed(GtkButton *button,  gpointer   user_data)
{
     gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginPress);
	 cit_login2();
}

static void  on_btn_login_enter(GtkButton *button,  gpointer   user_data)
{
     gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginPress);
}

static void  on_btn_login_leave(GtkButton *button,  gpointer   user_data)
{
     gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginNor);
}

void cit_login2()
{
    GObject *http;
    if (g_builder == NULL)
    {
        LogInfo("debug: cit_login  g_builder == NULL.\n");
        g_citExit = 0;
        return;
    }
    http = gtk_builder_get_object (g_builder, "entry_http");
    char * ip = gtk_entry_get_text(GTK_ENTRY(http));
    if (strcmp(ip, "") == 0)
    {
        //MsgDailog("URL不能为空！");
        g_citExit = 0;
        SYMsgDialog2(7, "URL不能为空！");
        return;
    }
    LogInfo("debug: cit_login http: %s.\n", ip);
    strcpy(g_szURL, ip);
    /*if ( pthread_create(&tid, NULL, thrd_citfunc, NULL) !=0 ) {
        printf("Create cit thread error!\n");
    }*/
    gtk_widget_destroy((GtkWidget *)g_window);
    gtk_main_quit();
    cit_login();
    g_citExit = 0;
}

static void  on_btn_login_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginNor);
   cit_login2();
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
    gtk_css_provider_load_from_path (provider,
                                 g_filename_to_utf8(g_cit_css, strlen(g_cit_css), &bytes_read, &bytes_written, &error),
                                 NULL);
    //gtk_css_provider_load_from_path (provider, home, NULL);
    g_object_unref (provider);
}

static void set_ctrl_image(GtkBuilder *builder)
{
	GObject *image_loginout;
	GObject *image_shutdown;
	GObject *image_login;
	image_loginout = gtk_builder_get_object (builder, "image_loginout");
	image_shutdown = gtk_builder_get_object (builder, "image_shutdown");
	image_login = gtk_builder_get_object (builder, "image_login");
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_loginout), g_prevNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_shutdown), g_shutdownNor);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_login), g_loginNor);
}
static init_ctrl_pos(GtkBuilder *builder)
{
	GObject *layout1;
	GObject *layout2;
	GObject *layout3;
	GObject *btn_citPrev;
	GObject *label_http;
	GObject *entry_http;
	GObject *btn_citlogin;
	GObject *btn_shutdown;
	GObject *image_logo;
	layout1 = gtk_builder_get_object (builder, "layout1");
	layout2 = gtk_builder_get_object (builder, "layout2");
	layout3 = gtk_builder_get_object (builder, "layout3");
	btn_citPrev = gtk_builder_get_object (builder, "btn_citPrev");
	label_http = gtk_builder_get_object (builder, "label_http");
	entry_http = gtk_builder_get_object (builder, "entry_http");
	btn_citlogin = gtk_builder_get_object (builder, "btn_citlogin");
	btn_shutdown = gtk_builder_get_object (builder, "btn_shutdown");
	image_logo = gtk_builder_get_object (builder, "image_logo");
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	int layout1_height = 0;
	int layout2_height = 0;
	int layout3_height = 0;
	int pic_loginout_width = 0;
	int pic_loginout_height = 0;
	int pic_shutdown_width = 0;
	int pic_shutdown_height = 0;
	int label_http_width = 0;
	int label_http_height = 0;
	int entry_http_width = 0;
	int entry_http_height = 0;
	int pic_login_width = 0;
	int pic_login_height = 0;
	int pic_logo_width = 0;
	int pic_logo_height = 0;
	if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
	    layout1_height = 200;
		layout3_height = 300;
		layout2_height = scr_height - layout1_height - layout3_height;
		label_http_width = 60;
		label_http_height = 30;
		entry_http_width = 150;
		entry_http_height = 30;
		gdk_pixbuf_get_file_info("images2/loginout_nor.png", &pic_loginout_width, &pic_loginout_height);
		gdk_pixbuf_get_file_info("images2/exit_nor.png", &pic_shutdown_width, &pic_shutdown_height);
		gdk_pixbuf_get_file_info("images2/btnlogin_nor.png", &pic_login_width, &pic_login_height);
		gdk_pixbuf_get_file_info("images2/cit_logo.png", &pic_logo_width, &pic_logo_height);
		setctrlFont(GTK_WIDGET(label_http), 14);
		setctrlFont(GTK_WIDGET(entry_http), 14);
	}else if (scr_width == 1024 && scr_height == 768)
	{
		layout1_height = 100;
		layout3_height = 200;
		label_http_width = 60;
		label_http_height = 20;
		entry_http_width = 150;
		entry_http_height = 20;
		layout2_height = scr_height - layout1_height - layout3_height;
		gdk_pixbuf_get_file_info("images2/1024x768/loginout_nor.png", &pic_loginout_width, &pic_loginout_height);
		gdk_pixbuf_get_file_info("images2/1024x768/exit_nor.png", &pic_shutdown_width, &pic_shutdown_height);
		gdk_pixbuf_get_file_info("images2/1024x768/btnlogin_nor.png", &pic_login_width, &pic_login_height);
		gdk_pixbuf_get_file_info("images2/1024x768/cit_logo.png", &pic_logo_width, &pic_logo_height);
		setctrlFont(GTK_WIDGET(label_http), 9);
		setctrlFont(GTK_WIDGET(entry_http), 9);
		
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080))
	{
		layout1_height = 100;
		layout3_height = 200;
		label_http_width = 60;
		label_http_height = 30;
		entry_http_width = 150;
		entry_http_height = 30;
		layout2_height = scr_height - layout1_height - layout3_height;
		gdk_pixbuf_get_file_info("images2/1440x900/loginout_nor.png", &pic_loginout_width, &pic_loginout_height);
		gdk_pixbuf_get_file_info("images2/1440x900/exit_nor.png", &pic_shutdown_width, &pic_shutdown_height);
		gdk_pixbuf_get_file_info("images2/1440x900/btnlogin_nor.png", &pic_login_width, &pic_login_height);
		gdk_pixbuf_get_file_info("images2/1440x900/cit_logo.png", &pic_logo_width, &pic_logo_height);
		setctrlFont(GTK_WIDGET(label_http), 10);
		setctrlFont(GTK_WIDGET(entry_http), 10);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		layout1_height = 100;
		layout3_height = 200;
		label_http_width = 60;
		label_http_height = 20;
		entry_http_width = 150;
		entry_http_height = 20;
		layout2_height = scr_height - layout1_height - layout3_height;
		gdk_pixbuf_get_file_info("images2/1280x720/loginout_nor.png", &pic_loginout_width, &pic_loginout_height);
		gdk_pixbuf_get_file_info("images2/1280x720/exit_nor.png", &pic_shutdown_width, &pic_shutdown_height);
		gdk_pixbuf_get_file_info("images2/1280x720/btnlogin_nor.png", &pic_login_width, &pic_login_height);
		gdk_pixbuf_get_file_info("images2/1280x720/cit_logo.png", &pic_logo_width, &pic_logo_height);
		setctrlFont(GTK_WIDGET(label_http), 9);
		setctrlFont(GTK_WIDGET(entry_http), 9);
	}else if ((scr_width == 1366 && scr_height == 768) || (scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		layout1_height = 100;
		layout3_height = 200;
		label_http_width = 60;
		label_http_height = 20;
		entry_http_width = 150;
		entry_http_height = 20;
		layout2_height = scr_height - layout1_height - layout3_height;
		gdk_pixbuf_get_file_info("images2/1366x768/loginout_nor.png", &pic_loginout_width, &pic_loginout_height);
		gdk_pixbuf_get_file_info("images2/1366x768/exit_nor.png", &pic_shutdown_width, &pic_shutdown_height);
		gdk_pixbuf_get_file_info("images2/1366x768/btnlogin_nor.png", &pic_login_width, &pic_login_height);
		gdk_pixbuf_get_file_info("images2/1366x768/cit_logo.png", &pic_logo_width, &pic_logo_height);
		setctrlFont(GTK_WIDGET(label_http), 9);
		setctrlFont(GTK_WIDGET(entry_http), 9);
	}
	gtk_widget_set_size_request(GTK_WINDOW(layout1), scr_width, layout1_height);
	gtk_widget_set_size_request(GTK_WINDOW(layout2), scr_width, layout2_height);
	gtk_widget_set_size_request(GTK_WINDOW(layout3), scr_width, layout3_height);
	gtk_widget_set_size_request(GTK_WINDOW(btn_citPrev), pic_loginout_width, pic_loginout_height);
	gtk_widget_set_size_request(GTK_WINDOW(btn_shutdown), pic_shutdown_width, pic_shutdown_height);
	gtk_widget_set_size_request(GTK_WINDOW(label_http), label_http_width, label_http_height);
	gtk_widget_set_size_request(GTK_WINDOW(entry_http), entry_http_width, entry_http_height);
	gtk_widget_set_size_request(GTK_WINDOW(btn_citlogin), pic_login_width, pic_login_height);
	gtk_widget_set_size_request(GTK_WINDOW(image_logo), pic_logo_width, pic_logo_height);
	//set loginout pos
	int x, y = 0;
	int delay_width = 40;
	int space_width = 10;
	x = scr_width - delay_width - pic_loginout_width;
	if (scr_width == 1920 && scr_height == 1080)
		y = (layout1_height - pic_loginout_height)/2 - 30;
	else 
		y = (layout1_height - pic_loginout_height)/2;
	gtk_layout_move((GtkLayout *)layout1, GTK_WIDGET(btn_citPrev), x, y);
	x = scr_width - delay_width - pic_shutdown_width;
	y = layout3_height - delay_width - pic_shutdown_height;
	gtk_layout_move((GtkLayout *)layout3, GTK_WIDGET(btn_shutdown), x, y);
	//set http pos
	int tmp_x = scr_width/2 - (10 + label_http_width + entry_http_width)/2;
	x = scr_width/2 - (space_width + label_http_width + entry_http_width)/2 - space_width*3;
	if (scr_width == 1024 && scr_height == 768)
		y = layout2_height/2 + 10;
    else
		y = layout2_height/2;
	gtk_layout_move((GtkLayout *)layout2, GTK_WIDGET(label_http), x, y);
	x = tmp_x + space_width + label_http_width - space_width*3;
	gtk_layout_move((GtkLayout *)layout2, GTK_WIDGET(entry_http), x, y);
	//set logo
	if (scr_width == 1024 && scr_height == 768)
		x = scr_width/2 - pic_logo_width/2 - 35;
	else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
		x = scr_width/2 - pic_logo_width/2 - 28;
	else
		x = scr_width/2 - pic_logo_width/2;
	y = y - delay_width - pic_logo_height;
	gtk_layout_move((GtkLayout *)layout2, GTK_WIDGET(image_logo), x, y);
	//set login btn
	if (scr_width == 1024 && scr_height == 768)
		y = layout2_height/2 + 10;
	else
		y = layout2_height/2;
	x = tmp_x + (space_width + label_http_width + entry_http_width)/2  - pic_login_width/2 + space_width*2;
	y = y + entry_http_height + space_width * 2;
	gtk_layout_move((GtkLayout *)layout2, GTK_WIDGET(btn_citlogin), x, y);
}

void SY_logincit_main()
{
    if (showlogincitwindow == 1)
       return;
    showlogincitwindow = 1;
    GtkBuilder *builder;
    GObject *window;

    builder = gtk_builder_new ();
    g_builder = builder;
    GError *errort = NULL;
    gtk_builder_add_from_file (builder, "logincit.glade", &errort);
    g_window = gtk_builder_get_object (builder, "window_cit");

	GdkScreen* screen1;
	screen1 = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen1);
	int scr_height = gdk_screen_get_height(screen1);
	gtk_widget_set_size_request(GTK_WINDOW(g_window), scr_width, scr_height);
	if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		g_loginPress = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_PRES, NULL);
	    g_loginNor = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_NOR, NULL);
	    surface = cairo_image_surface_create_from_png (IMAGE_CIT_BACKGROUD);
	    g_prevPress = gdk_pixbuf_new_from_file(IMAGE_BTN_CITPREV_PRES, NULL);
	    g_prevNor = gdk_pixbuf_new_from_file(IMAGE_BTN_CITPREV_NOR, NULL);	
		g_shutdownPress = gdk_pixbuf_new_from_file(IMAGE_BTN_SHUTDWON_PRES, NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file(IMAGE_BTN_SHUTDWON_NOR, NULL);
	}else if (scr_width == 1024 && scr_height == 768)
	{
	    g_loginPress = gdk_pixbuf_new_from_file("images2/1024x768/btnlogin_press.png", NULL);
	    g_loginNor = gdk_pixbuf_new_from_file("images2/1024x768/btnlogin_nor.png", NULL);
	    surface = cairo_image_surface_create_from_png ("images2/1024x768/cit_login.png");
	    g_prevPress = gdk_pixbuf_new_from_file("images2/1024x768/loginout_press.png", NULL);
	    g_prevNor = gdk_pixbuf_new_from_file("images2/1024x768/loginout_nor.png", NULL);
		g_shutdownPress = gdk_pixbuf_new_from_file("images2/1024x768/exit_press.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1024x768/exit_nor.png", NULL);
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080))
	{
		g_loginPress = gdk_pixbuf_new_from_file("images2/1440x900/btnlogin_press.png", NULL);
	    g_loginNor = gdk_pixbuf_new_from_file("images2/1440x900/btnlogin_nor.png", NULL);
		if ((scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080))
			surface = cairo_image_surface_create_from_png ("images2/1600x900/cit_login.png");
		else if (scr_width == 1440 && scr_height == 900)
	    		surface = cairo_image_surface_create_from_png ("images2/1440x900/cit_login.png");
	    g_prevPress = gdk_pixbuf_new_from_file("images2/1440x900/loginout_press.png", NULL);
	    g_prevNor = gdk_pixbuf_new_from_file("images2/1440x900/loginout_nor.png", NULL);
		g_shutdownPress = gdk_pixbuf_new_from_file("images2/1440x900/exit_press.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1440x900/exit_nor.png", NULL);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || (scr_width == 1280 && scr_height == 1024))
	{
		g_loginPress = gdk_pixbuf_new_from_file("images2/1280x720/btnlogin_press.png", NULL);
	    g_loginNor = gdk_pixbuf_new_from_file("images2/1280x720/btnlogin_nor.png", NULL);
		if (scr_width == 1280 && scr_height == 1024)
			surface = cairo_image_surface_create_from_png ("images2/1280x1024/cit_login.png");
		else 
			surface = cairo_image_surface_create_from_png ("images2/1280x720/cit_login.png");
		g_prevPress = gdk_pixbuf_new_from_file("images2/1280x720/loginout_press.png", NULL);
	    g_prevNor = gdk_pixbuf_new_from_file("images2/1280x720/loginout_nor.png", NULL);
		g_shutdownPress = gdk_pixbuf_new_from_file("images2/1280x720/exit_press.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1280x720/exit_nor.png", NULL);
	}else if ((scr_width == 1366 && scr_height == 768) || (scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768))
	{
		g_loginPress = gdk_pixbuf_new_from_file("images2/1366x768/btnlogin_press.png", NULL);
	    g_loginNor = gdk_pixbuf_new_from_file("images2/1366x768/btnlogin_nor.png", NULL);
		surface = cairo_image_surface_create_from_png ("images2/1366x768/cit_login.png");
		g_prevPress = gdk_pixbuf_new_from_file("images2/1366x768/loginout_press.png", NULL);
	    g_prevNor = gdk_pixbuf_new_from_file("images2/1366x768/loginout_nor.png", NULL);
		g_shutdownPress = gdk_pixbuf_new_from_file("images2/1366x768/exit_press.png", NULL);
	    g_shutdownNor = gdk_pixbuf_new_from_file("images2/1366x768/exit_nor.png", NULL);
	}
	loadcss();
	set_ctrl_image(builder);
	init_ctrl_pos(builder);
    GObject *image_logo;
    image_logo = gtk_builder_get_object (builder, "image_logo");
    g_logoCit = gdk_pixbuf_new_from_file(IMAGE_LOGO_CIT, NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_logoCit);

    GObject *label = gtk_builder_get_object (builder, "label_http");
    gtk_label_set_text(GTK_LABEL(label), "URL");

    gtk_widget_set_app_paintable((GtkWidget * )g_window, TRUE);
    //button = gtk_builder_get_object (builder, "loginbutton");
    g_signal_connect (G_OBJECT(g_window), "draw", G_CALLBACK (on_expose_loginevent), NULL);
    g_signal_connect (G_OBJECT(g_window), "destroy",
                       G_CALLBACK(gtk_main_quit_login), NULL);

	GObject *btn_shutdown;
	GObject *image_shutdown;
	btn_shutdown = gtk_builder_get_object (builder, "btn_shutdown");
	image_shutdown = gtk_builder_get_object (builder, "image_shutdown");
	g_signal_connect(G_OBJECT(btn_shutdown), "pressed", G_CALLBACK(on_btn_shutdown_pressed), (GtkWidget *)image_shutdown);
	g_signal_connect(G_OBJECT(btn_shutdown), "leave", G_CALLBACK(on_btn_shutdown_leave), (GtkWidget *)image_shutdown);
	g_signal_connect(G_OBJECT(btn_shutdown), "enter", G_CALLBACK(on_btn_shutdown_enter), (GtkWidget *)image_shutdown);
	g_citExit = 1;
	//登录
	GObject *image_loginout;
	GObject *login_image;
	image_loginout = gtk_builder_get_object (builder, "image_loginout");
	login_image = gtk_builder_get_object (builder, "image_login");
	GObject *btn_citlogin;
	GObject *btn_citPrev;
	btn_citPrev = gtk_builder_get_object (builder, "btn_citPrev");
	btn_citlogin = gtk_builder_get_object (builder, "btn_citlogin");
	g_signal_connect(G_OBJECT(btn_citPrev), "pressed", G_CALLBACK(on_btn_prev_pressed), (GtkWidget *)image_loginout);
	g_signal_connect(G_OBJECT(btn_citPrev), "leave", G_CALLBACK(on_btn_prev_leave), (GtkWidget *)image_loginout);
	g_signal_connect(G_OBJECT(btn_citPrev), "enter", G_CALLBACK(on_btn_prev_enter), (GtkWidget *)image_loginout);

	g_signal_connect(G_OBJECT(btn_citlogin), "pressed", G_CALLBACK(on_btn_login_pressed), (GtkWidget *)login_image);
	g_signal_connect(G_OBJECT(btn_citlogin), "leave", G_CALLBACK(on_btn_login_leave), (GtkWidget *)login_image);
	g_signal_connect(G_OBJECT(btn_citlogin), "enter", G_CALLBACK(on_btn_login_enter), (GtkWidget *)login_image);
	gtk_window_set_icon(GTK_WINDOW(g_window), create_pixbuf("images2/logo.png"));
	gtk_window_fullscreen(GTK_WINDOW(g_window));
	gtk_window_set_decorated(GTK_WINDOW(g_window), FALSE); /* hide the title bar and the boder */
	g_signal_connect(G_OBJECT(g_window), \
	              "key-press-event", \
	              G_CALLBACK(on_key_press), NULL);

	gtk_main ();
	g_object_unref (G_OBJECT (builder));
	cairo_surface_destroy (surface);
    showlogincitwindow = 0;
}
