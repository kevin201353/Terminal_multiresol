#include <gtk/gtk.h>
#include "global.h"
#include <string.h>

static const gchar*  g_settingcss = "setting.css";
#define IMAGE_CHECKBUTTON_NOR    "images2/checkbtnset_nor.png"
#define IMAGE_CHECKBUTTON_PRESS  "images2/checkbtnset_press.png"

enum {
    COLUMN = 0,
    NUM_COLS
};
struct ServerInfo g_serverinfo = {"192.168.0.220", 443, "admin@internal", "shencloud", 1, 0, "1920x1080"};
GObject * g_layout1;  //服务器Layout
GObject * g_layout2;  //网络
GObject * g_layout_about;

static GObject * g_window;
GdkPixbuf *g_checkNorimage;
GdkPixbuf *g_checkPressimage;
static GtkBuilder *g_builder2 = NULL;
static unsigned short g_checkrepass = 0;
static unsigned short g_checkmanresol = 0;
extern void SY_NetworkDisableCtrl();
extern unsigned short saveServerInfo();
static int showsettingwindow = 0;
extern int check_ipv4_valid(char *s);
extern void setctrlFont(GtkWidget * widget, int nsize);
GdkPixbuf *g_imagelogo;
GdkPixbuf *g_imageclose;
extern char szResol[MAX_RESOL_SIZE][100];
extern GtkLayout * GetAboutLayout();
extern void setAbout_win_Size(int width, int height);
extern void SY_AboutWindow();



gboolean on_window_state_event (GtkWidget *widget, GdkEventWindowState  *event, gpointer  user_data)
{
    if ((event->changed_mask == GDK_WINDOW_STATE_ICONIFIED &&
        event->new_window_state == GDK_WINDOW_STATE_ICONIFIED) ||
        (event->changed_mask == GDK_WINDOW_STATE_MAXIMIZED &&
            event->new_window_state == GDK_WINDOW_STATE_MAXIMIZED))
      {
          LogInfo("Debug: setting on_window_state_event window minizied and maximized.\n");
          return TRUE;
      }
    return FALSE;
}

static gboolean close_button_clicked(GtkButton *button,  gpointer user_data)
{
    gtk_widget_destroy((GtkWidget *)g_window);
    gtk_main_quit();
    showsettingwindow = 0;
    //SetLoginWindowFocus();
}

static gboolean save_button_clicked(GtkButton *button,  gpointer user_data)
{
    //设置分辨率
    saveServerInfo();
    char szTmp[MAX_BUFF_SIZE] = {0};
    if (g_checkrepass == 1)
    {
        //根据终端屏幕分辩率自动调整
        LogInfo("Debug: setting.c main exit, selected resolution, screen_width: %d, screen_height: %d. \n", g_screen_width, g_screen_height);
        sprintf(szTmp, "sudo xrandr -s  %dx%d", g_screen_width, g_screen_height);
        system(szTmp);
        LogInfo("Debug: setting.c main exit, selected resolution, cmd: %s .\n", szTmp);
        char szCmd[MAX_BUFF_SIZE] = {0};
        sprintf(szCmd, "sudo echo  %s > manual_resol.sh", szTmp);  //暂时这样处理，开机启动显示分辨率
        system(szCmd);
		memset(szCmd, 0, sizeof(char) * MAX_BUFF_SIZE);
		sprintf(szCmd, "%dx%d", g_screen_width, g_screen_height);
		LogInfo("changed resolution g_serverinfo.szResol = %s, szCmd = %s.\n", g_serverinfo.szResol, szCmd);
		if (strcmp(g_serverinfo.szResol, szCmd) != 0)
    	{
          char sztmp[MAX_BUFF_SIZE] = {0};
          sprintf(sztmp, "确定将分辨率改为 %s ？", (char *)szCmd);
        	SYMsgDialog(20, sztmp);
    	}
    }
    if (g_checkmanresol == 1)
    {
        //手动设置分辨率调整
        GdkScreen* screen;
        screen = gdk_screen_get_default();
        int width = gdk_screen_get_width(screen);
        int height = gdk_screen_get_height(screen);
        char szold_screen[MAX_BUFF_SIZE] = {0};
        sprintf(szold_screen, "%dx%d", width, height);
        GObject *comboboxtext_resol;
        comboboxtext_resol = gtk_builder_get_object (g_builder2, "comboboxtext_resol");
        gchar *active = gtk_combo_box_text_get_active_text((GtkComboBoxText *)comboboxtext_resol);
        LogInfo("setting.c main exit, old resolution: %s, selected resolution: %s\n", szold_screen, (char *)active);
        strcpy(g_serverinfo.szResol, (char *)active);
        if (strcmp(szold_screen, (char *)active) != 0)
        {

            LogInfo("Debug: setting.c screen resolution changed, start reset resolution.\n");
            strcpy(szTmp, "sudo xrandr -s ");
            strcat(szTmp, (char *)active);
            char szCmd[MAX_BUFF_SIZE] = {0};
            sprintf(szCmd, "sudo echo  %s > manual_resol.sh", szTmp); //暂时这样处理, 开机启动显示分辨率
            system(szCmd);
            memset(szCmd, 0, MAX_BUFF_SIZE);
            sprintf(szCmd, "确定将分辨率改为 %s ？", (char *)active);
			SYMsgDialog(20, szCmd);
        }
    }
    return TRUE;
}

void  on_button_11_clicked(GtkButton *button, gpointer   user_data)
{
    printf("on button 11 clicked.\n");
}

//检测分辨率是否存在
//注意该函数在setResol之后调用
int IsRelExist(char *resol, int *index)
{
  int i = 0;
  for (; i< g_resolCount; i++)
  {
      if (strcmp(szResol[i], resol) == 0)
      {
          LogInfo("Debug: setting.c IsRelExist  ##### 11,  index = %d .\n", i);
          *index = i;
          return 1;
      }
  }
  return 0;
}

//初始化手动调整分辨率
int setResol()
{
    GObject *comboresol;
    comboresol = gtk_builder_get_object (g_builder2, "comboboxtext_resol");
    GetResol();
    int i = 0;
    LogInfo("Debug: setting.c setResol  g_resolCount = %d .\n", g_resolCount);
    for (; i< g_resolCount; i++)
    {
        LogInfo("Debug: setting.c setResol  manual resolution = %s .\n", szResol[i]);
        if (strcmp(szResol[i], "") > 0)
        {
            gtk_combo_box_text_insert_text((GtkComboBoxText *)comboresol, i, szResol[i]);
        }
    }
    gtk_widget_set_sensitive(GTK_WIDGET(comboresol), g_checkmanresol);
    //从配置文件中获取已设置的分辨率
    int index = 0;
    LogInfo("Debug: setting.c setResol  g_serverinfo.szResol = %s .\n", g_serverinfo.szResol);
	//get current screen
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	char szTmp[MAX_BUFF_SIZE] = {0};
	sprintf(szTmp, "%dx%d", scr_width, scr_height);
    if (IsRelExist(szTmp, &index) == 1)
    {
        gtk_combo_box_set_active((GtkComboBox *)comboresol, index);
    }else
    {
        gtk_combo_box_set_active((GtkComboBox *)comboresol, 0);
    }
    LogInfo("Debug: setting.c setResol  IsRelExist  index = %d .\n", index);
    return 0;
}

int initctrl()
{
    sprintf(g_resol[0], "%s", "1920x1080");
    sprintf(g_resol[1], "%s", "1440x900");
    sprintf(g_resol[2], "%s", "1024x768");
    sprintf(g_resol[3], "%s", "1600x900");
    g_resolCount = 4;
    GetServerInfo2(&g_serverinfo);
    LogInfo("Debug: setting.c initctrl get serverinfo,  ip: %s .\n", g_serverinfo.szIP);
    LogInfo("Debug: setting.c initctrl get serverinfo,  user: %s .\n", g_serverinfo.szUser);
    LogInfo("Debug: setting.c initctrl get serverinfo,  pass: %s .\n", g_serverinfo.szPass);
    LogInfo("Debug: setting.c initctrl get serverinfo,  port: %d .\n", g_serverinfo.nport);
    LogInfo("Debug: setting.c initctrl get serverinfo,  resolution: %d .\n", g_serverinfo.resol);
    GObject *object;
    object = gtk_builder_get_object (g_builder2, "entry_serverip");
    gtk_entry_set_text((GtkEntry *)object, g_serverinfo.szIP);
    object = gtk_builder_get_object (g_builder2, "entry_port");
    char szTmp[MAX_BUFF_SIZE] = {0};
    sprintf(szTmp, "%d", g_serverinfo.nport);
    gtk_entry_set_text((GtkEntry *)object, szTmp);
    object = gtk_builder_get_object (g_builder2, "entryset_user");
    gtk_entry_set_text((GtkEntry *)object, g_serverinfo.szUser);
    object = gtk_builder_get_object (g_builder2, "entryset_pass");
    gtk_entry_set_text((GtkEntry *)object, g_serverinfo.szPass);
    g_checkrepass = g_serverinfo.resol;
    g_checkmanresol = g_serverinfo.manresol;
    LogInfo("Debug: setting.c initctrl g_checkrepass 555555,  resolution: %d .\n", g_checkrepass);
    LogInfo("Debug: setting.c initctrl g_checkmanresol 555555,  resolution: %d .\n", g_checkmanresol);
    GObject *image_resolution;
    image_resolution = gtk_builder_get_object (g_builder2, "image_resolution");
    if (g_checkrepass == 1)
    {
       LogInfo("Debug:  setting.c init g_checkrepass is 1.\n");
       gtk_image_set_from_pixbuf(GTK_IMAGE(image_resolution), g_checkPressimage);
       GObject *object;
       object = gtk_builder_get_object (g_builder2, "image_manualresol");
       gtk_image_set_from_pixbuf(GTK_IMAGE(object), g_checkNorimage);
    }else
    {
       LogInfo("Debug:  setting.c init g_checkrepass is 0.\n");
       gtk_image_set_from_pixbuf(GTK_IMAGE(image_resolution), g_checkNorimage);
    }
    image_resolution = gtk_builder_get_object (g_builder2, "image_manualresol");
    if (g_checkmanresol == 1)
    {
       LogInfo("Debug:  setting.c init g_checkmanresol is 1.\n");
       gtk_image_set_from_pixbuf(GTK_IMAGE(image_resolution), g_checkPressimage);
       GObject *object;
       object = gtk_builder_get_object (g_builder2, "image_resolution");
       gtk_image_set_from_pixbuf(GTK_IMAGE(object), g_checkNorimage);
    }else
    {
       LogInfo("Debug:  setting.c init g_checkmanresol is 0.\n");
       gtk_image_set_from_pixbuf(GTK_IMAGE(image_resolution), g_checkNorimage);
    }
}

unsigned short saveServerInfo()
{
    GObject *object;
    object = gtk_builder_get_object (g_builder2, "entry_serverip");
    char * szserverIP = (char *)gtk_entry_get_text((GtkEntry *)object);
    LogInfo("debug: serverIP: %s.\n" ,szserverIP);
    memset(g_serverinfo.szIP, 0, MAX_BUFF_SIZE);
    memcpy(g_serverinfo.szIP, szserverIP, strlen(szserverIP));

    object = gtk_builder_get_object (g_builder2, "entry_port");
    char * szPort = (char *)gtk_entry_get_text((GtkEntry *)object);
    LogInfo("debug: serverIP: %s.\n" ,szPort);
    g_serverinfo.nport = (unsigned int)atoi(szPort);

    object = gtk_builder_get_object (g_builder2, "entryset_user");
    char * szUsername = (char *)gtk_entry_get_text((GtkEntry *)object);
    LogInfo("debug: username: %s.\n" ,szUsername);
    memset(g_serverinfo.szUser, 0, MAX_BUFF_SIZE);
    memcpy(g_serverinfo.szUser, szUsername, strlen(szUsername));

    object = gtk_builder_get_object (g_builder2, "entryset_pass");
    char * szPassword = (char *)gtk_entry_get_text((GtkEntry *)object);
    LogInfo("debug: password: %s.\n" ,szPassword);
    memset(g_serverinfo.szPass, 0, MAX_BUFF_SIZE);
    memcpy(g_serverinfo.szPass, szPassword, strlen(szPassword));
    g_serverinfo.resol = g_checkrepass;
    g_serverinfo.manresol = g_checkmanresol;
    if (!check_ipv4_valid(g_serverinfo.szIP))
    {
        SYMsgDialog(7, "IP 格式不正确, 请重新输入！");
        return -1;
    }
	LogInfo("debug: wirte setting checkmanresol=%d, checkclientresol=%d.\n" ,g_checkrepass, g_checkmanresol);
    SaveServerInfo(g_serverinfo);
    return 0;
}

static gboolean gtk_main_quit_setting(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    gtk_main_quit();
    //SetLoginWindowFocus();
    return TRUE;
}

static gboolean checkbutton_resolution_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        g_checkrepass = !g_checkrepass;
        LogInfo("Debug: checkbutton resolution gtkimage check: %d.\n", g_checkrepass);
        if (g_checkrepass == 1)
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_checkPressimage);
            GObject *object;
            object = gtk_builder_get_object (g_builder2, "image_manualresol");
            gtk_image_set_from_pixbuf(GTK_IMAGE(object), g_checkNorimage);
            g_checkmanresol = 0;
            GObject *comboresol;
            comboresol = gtk_builder_get_object (g_builder2, "comboboxtext_resol");
            gtk_widget_set_sensitive(GTK_WIDGET(comboresol), 0);
        }
        else
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_checkNorimage);
        }
    }
    return TRUE;
}

static gboolean checkbutton_manualresol_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        g_checkmanresol = !g_checkmanresol;
        LogInfo("Debug: checkbutton manualresol gtkimage check: %d.\n", g_checkmanresol);
        if (g_checkmanresol == 1)
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_checkPressimage);
            GObject *object;
            object = gtk_builder_get_object (g_builder2, "image_resolution");
            gtk_image_set_from_pixbuf(GTK_IMAGE(object), g_checkNorimage);
            g_checkrepass = 0;
            GObject *comboresol;
            comboresol = gtk_builder_get_object (g_builder2, "comboboxtext_resol");
            gtk_widget_set_sensitive(GTK_WIDGET(comboresol), 1);
        }
        else
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_checkNorimage);
        }
    }
    return TRUE;
}

void on_changed(GtkWidget * widget, gpointer statusbar)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *value;
    if(gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)){
        gtk_tree_model_get(model, &iter, COLUMN, &value, -1);
		    LogInfo("Debug: on changed selected value :%s.\n", (gchar *)value);
		if (strcmp((char *)(gchar *)value, "基本设置") == 0)
		{
  			gtk_widget_set_visible((GtkWidget *)g_layout1, 1);
  			gtk_widget_set_visible((GtkWidget *)g_layout2, 0);
			gtk_widget_set_visible((GtkWidget *)g_layout_about, 0);
        	initctrl();
		}
		if (strcmp((char *)(gchar *)value, "网络") == 0)
		{
        	saveServerInfo();
  			gtk_widget_set_visible((GtkWidget *)g_layout2, 1);
  			gtk_widget_set_visible((GtkWidget *)g_layout1, 0);
			gtk_widget_set_visible((GtkWidget *)g_layout_about, 0);
		}
		if (strcmp((char *)(gchar *)value, "关于") == 0)
		{
        	saveServerInfo();
			LogInfo("on_changed  selected about window.\n ");
  			gtk_widget_set_visible((GtkWidget *)g_layout2, 0);
  			gtk_widget_set_visible((GtkWidget *)g_layout1, 0);
			gtk_widget_set_visible((GtkWidget *)g_layout_about, 1);
		}
        g_free(value);
    }
}

static GtkTreeModel *create_and_fill_model(void)
{
    GtkTreeStore *treestore;
    GtkTreeIter toplevel, child;
    treestore = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING);
    gtk_tree_store_append(treestore,&toplevel,NULL);
    gtk_tree_store_set(treestore,&toplevel, COLUMN,"基本设置",-1);
    gtk_tree_store_append(treestore,&toplevel,NULL);
    gtk_tree_store_set(treestore,&toplevel, COLUMN,"网络",-1);
    gtk_tree_store_append(treestore,&toplevel,NULL);
    gtk_tree_store_set(treestore,&toplevel, COLUMN,"关于",-1);
  /*  gtk_tree_store_append(treestore,&child,&toplevel);
    gtk_tree_store_set(treestore,&child, COLUMN,"PHP",-1);
    gtk_tree_store_append(treestore,&child,&toplevel);
    gtk_tree_store_set(treestore,&child, COLUMN,"Perl",-1);*/
    return GTK_TREE_MODEL(treestore);
}

static GtkWidget *create_view_and_model(void)
{
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    GtkWidget *view;
    GtkTreeModel *model;
    view = gtk_tree_view_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col,"");
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(view), FALSE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer,TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,"text", COLUMN);
    model = create_and_fill_model();
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    g_object_unref(model);
    return view;
}

static void print_hello (GtkWidget *widget,
             gpointer   data)
{
    g_print ("Hello World\n");
}

/*
  @Description: 从一个图片中获取信息得到pixbuf
  @param:       gchar filename
*/
GdkPixbuf *create_pixbuf(const gchar * filename)
{
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    /*
     * 函数gdk_pixbuf_new_from_file() 从一个图片文件中加载图象数据，从而生成一个新的 pixbuf，
     * 至于文件中包含图象的格式，是由系统自动检测的。如果该函数返回值是NULL 的话，程序就会出现错误。
    */
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    if(!pixbuf) {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }
    return pixbuf;
}

gboolean window_drag(GtkWidget *widget, GdkEventButton *event, GdkWindowEdge edge)
{
    if (event->button == 1)
    {
        gtk_window_begin_move_drag(GTK_WINDOW(gtk_widget_get_toplevel(widget)), event->button, event->x_root, event->y_root, event->time);
        gtk_widget_queue_draw(widget);
    }
    return FALSE;
}

/*gboolean window_move(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gint x, y;
    gchar buf[10] = {0};
    x = event->configure.x;
    y = event->configure.y;
    if (x <= 0)//如果窗口超出桌面最左边框
    {
        x = 0;
        gtk_window_move(GTK_WINDOW(widget), x, y);
    }
    sprintf(buf, "%d,%d", x, y);
    gtk_window_set_title(GTK_WINDOW(widget), buf);
    return TRUE;
}*/

static void loadcss()
{
	//绘制控件
	/*----- CSS ----------- */
	GtkCssProvider *provider;
	GdkDisplay *display;
	GdkScreen *screen;
	/*-----------------------*/
	/* ----------------- CSS ----------------------------------------------------------------------------------------------*/
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
	gtk_css_provider_load_from_path (provider,
		                        g_filename_to_utf8(g_settingcss, strlen(g_settingcss), &bytes_read, &bytes_written, &error),
		                        NULL);
	//gtk_css_provider_load_from_path (provider, home, NULL);
	g_object_unref (provider);
	/* --------------------------------------------------------------------------------------------------------------------*/
}

static void init_ctrl_pos(GtkBuilder *builder)
{
    GObject *entry_serverip;
	GObject *entry_port;
	GObject *entryset_user;
	GObject *entryset_pass;
	GObject *box1;
	GObject *box_menu;
	GObject *box_window;
	GdkScreen* screen;
	int win_width = 0;
	int win_height = 0;
	GObject *layout2;
	GObject *layout3;
	GObject *layout_base;
	int layout2_width = 0;
	int layout2_height = 0;
	int layout3_width = 0;
	int layout3_height = 0;
	int box1_width = 0;
	int box1_height = 0;
	int box_menu_width = 0;
	int box_menu_height = 0;
	int box_window_width = 0;
	int box_window_height = 0;
	int font_size = 12;
	GObject *label_title;
	GObject *label_tip;
	GObject *label_serverip;
	GObject *label_port;
	GObject *label_user;
	GObject *label_pass;
	GObject *label_resolution;  //app resol
	GObject *label_sysresol;
	GObject *separator1;
	GObject *separator2;
	GObject *image_logo;
	GObject *image_resolution;
	GObject *image_close;
	int pic_close_width = 0;
	int pic_close_height = 0;
	int pic_logo_width = 0;
	int pic_logo_height = 0;
	int label_tip_width = 0;
	int label_tip_height = 0;
	int x,y = 0;
	int left = 30;
	int top = 10;
	int delay_height = 10;
	int delay_width = 10;
	int height = 30;
	int width = 100;
	int label_tiltle_width = 100;
	int label_tiltle_height = 20;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	layout2 = gtk_builder_get_object (builder, "layout2");
    layout3 = gtk_builder_get_object (builder, "layout3");
	layout_base = gtk_builder_get_object (builder, "layout_base");
	box1 = gtk_builder_get_object (builder, "box1");
    box_menu = gtk_builder_get_object (builder, "box_menu");
	box_window = gtk_builder_get_object (builder, "box_window");
	label_title = gtk_builder_get_object (builder, "label_title");
	if (scr_width == 1920 && scr_height == 1080)
	{
	 	win_width = 800;
		win_height = 600;
		layout2_width = 800;
		layout2_height = 40;
		layout3_width = 800;
		layout3_height = win_height - layout2_height;
		box1_width = win_width;
		box1_height = layout3_height;
		box_menu_width = 120;
		box_menu_height = box1_height;
		box_window_width = win_width - box_menu_width;
		box_window_height = box1_height;
		font_size = 12;
		setctrlFont(GTK_WIDGET(label_title), 14);
	}
	else if (scr_width == 1024 && scr_height == 768  || scr_width == 1440 && scr_height == 900 || scr_width == 1600 && scr_height == 900)
	{
		win_width = 500;
	 	win_height = 400;
		layout2_width = 500;
		layout2_height = 30;
		layout3_width = 500;
		layout3_height = win_height - layout2_height;
		box1_width = win_width;
		box1_height = layout3_height;
		box_menu_width = 80;
		box_menu_height = box1_height;
		box_window_width = win_width - box_menu_width;
		box_window_height = box1_height;
		font_size = 10;
		setctrlFont(GTK_WIDGET(label_title), 12);
	}
	gtk_widget_set_size_request (GTK_WINDOW(g_window), win_width, win_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout2), layout2_width, layout2_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout3), layout3_width, layout3_height);
	gtk_widget_set_size_request(GTK_WIDGET(box1), box1_width, box1_height);
	gtk_widget_set_size_request(GTK_WIDGET(box_menu), box_menu_width, box_menu_height);
	gtk_widget_set_size_request(GTK_WIDGET(box_window), box_window_width, box_window_height);
	//label
	label_tip = gtk_builder_get_object (builder, "label_tip");
    label_serverip = gtk_builder_get_object (builder, "label_serverip");
	label_port = gtk_builder_get_object (builder, "label_port");
    label_user = gtk_builder_get_object (builder, "label_user");
	label_pass = gtk_builder_get_object (builder, "label_pass");
    label_resolution = gtk_builder_get_object (builder, "label_resolution");
	separator1 = gtk_builder_get_object (builder, "separator1");
	separator2 = gtk_builder_get_object (builder, "separator2");
	setctrlFont(GTK_WIDGET(label_serverip), font_size);
	setctrlFont(GTK_WIDGET(label_port), font_size);
	setctrlFont(GTK_WIDGET(label_user), font_size);
	setctrlFont(GTK_WIDGET(label_pass), font_size);
	setctrlFont(GTK_WIDGET(label_resolution), font_size);
	setctrlFont(GTK_WIDGET(label_sysresol), font_size);
	setctrlFont(GTK_WIDGET(label_tip), font_size);
	//entry
	entry_serverip = gtk_builder_get_object (builder, "entry_serverip");
    entry_port = gtk_builder_get_object (builder, "entry_port");
	entryset_user = gtk_builder_get_object (builder, "entryset_user");
    entryset_pass = gtk_builder_get_object (builder, "entryset_pass");
	setctrlFont(GTK_WIDGET(entry_serverip), font_size);
	setctrlFont(GTK_WIDGET(entry_port), font_size);
	setctrlFont(GTK_WIDGET(entryset_user), font_size);
	setctrlFont(GTK_WIDGET(entryset_pass), font_size);
	//image
	image_logo = gtk_builder_get_object (builder, "image_logo");
	image_resolution = gtk_builder_get_object (builder, "image_resolution");
	image_close = gtk_builder_get_object(builder, "image_close");
	if (scr_width == 1024 && scr_height == 768  || scr_width == 1440 && scr_height == 900 || scr_width == 1600 && scr_height == 900)
	{
		gdk_pixbuf_get_file_info("images2/1024x768/close_set_22.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1024x768/logo22.png", &pic_logo_width, &pic_logo_height);
		gtk_widget_set_size_request(GTK_WIDGET(image_logo), 65, 15);
		gtk_widget_set_size_request(GTK_WIDGET(image_close), 10, 8);
		gtk_widget_set_size_request(GTK_WIDGET(label_serverip), 80, 20);
		label_tip_width = 200;
		label_tip_height = 60;
	}else if (scr_width == 1920 && scr_height == 1080)
	{
		label_tip_width = 400;
		label_tip_height = 100;
		gdk_pixbuf_get_file_info("images2/close_set.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/logo.png", &pic_logo_width, &pic_logo_height);
		gtk_widget_set_size_request(GTK_WIDGET(image_logo), 113, 26);
		gtk_widget_set_size_request(GTK_WIDGET(image_close), 14, 12);
	}
	LogInfo("init_ctrl_pos  pic_close_width = %d, pic_close_height =%d .\n", pic_close_width, pic_close_height);
	LogInfo("init_ctrl_pos  pic_logo_width = %d, pic_logo_height =%d .\n", pic_logo_width, pic_logo_height);
	if (scr_width == 1024 && scr_height == 768  || scr_width == 1440 && scr_height == 900 || scr_width == 1600 && scr_height == 900)
	{
		gtk_widget_set_size_request(GTK_WIDGET(label_tip), label_tip_width, label_tip_height);
		//cal position
		x = layout2_width/2 - label_tiltle_width/2;
		y = (layout2_height - label_tiltle_height)/2;
		int tmp_tilte_y = (layout2_height - label_tiltle_height)/2 - 4;
		gtk_layout_move((GtkLayout *)layout2, GTK_WIDGET(label_title), x, tmp_tilte_y);
		x = layout2_width - pic_close_width - 10;
		y = (layout2_height - pic_close_height)/2;
		gtk_layout_move((GtkLayout *)layout2, GTK_WIDGET(image_close), x, y);
		x = left*2;
		y = top;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(label_tip), x, y);
		y = top + delay_height + height + delay_height;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(separator1), x, y);
		y = top + delay_height + label_tip_height;
		x = left;
		top = top + delay_height + label_tip_height;
		int tmp_serverip = left + 20;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(label_serverip), tmp_serverip, y);
		x = left + width + delay_width;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(entry_serverip), x, y);
		x = left;
		y = top + delay_height + height;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(label_port), x, y);
		x = left + width + delay_width;
		y = top + delay_height + height;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(entry_port), x, y);
		x = left;
		y = top + delay_height + height + delay_height + height;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(label_user), x, y);
		x = left + width + delay_width;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(entryset_user), x, y);
		x = left;
		y = top + delay_height + height + delay_height + height + delay_height + height;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(label_pass), x, y);
		x = left + width + delay_width;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(entryset_pass), x, y);
	}

}

static void loadimage(GtkBuilder *builder)
{
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);

	if (scr_width == 1024 && scr_height == 768 || scr_width == 1440 && scr_height == 900 || scr_width == 1600 && scr_height == 900)
	{
		g_imageclose = gdk_pixbuf_new_from_file("images2/1024x768/close_set_22.png", NULL);
	    g_imagelogo = gdk_pixbuf_new_from_file("images2/1024x768/logo22.png", NULL);
	}else if (scr_width == 1920 && scr_height == 1080)
	{
		g_imageclose = gdk_pixbuf_new_from_file("images2/close_set.png", NULL);
	    g_imagelogo = gdk_pixbuf_new_from_file("images2/logo.png", NULL);
	}
	GObject *image_logo;
	GObject *image_close;
	image_logo = gtk_builder_get_object(builder, "image_logo");
	image_close = gtk_builder_get_object(builder, "image_close");
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_imagelogo);
	gtk_image_set_from_pixbuf(GTK_IMAGE(image_close), g_imageclose);
}


void int_tmp(GtkBuilder *builder)
{
    GObject *layout_base = gtk_builder_get_object (builder, "layout_base");
	GObject *eventbox_resolution = gtk_builder_get_object (builder, "eventbox_resolution");
	GObject *separator2 = gtk_builder_get_object (builder, "separator2");
	GObject *label_resolution = gtk_builder_get_object (builder, "label_resolution");
	GObject *btnset_close = gtk_builder_get_object (builder, "btnset_close");
	GObject *layout2 = gtk_builder_get_object (builder, "layout2");
	GObject *box_menu = gtk_builder_get_object (builder, "box_menu");
	GObject *eventbox_manualresol = gtk_builder_get_object (builder, "eventbox_manualresol");
	GObject *comboboxtext_resol = gtk_builder_get_object (builder, "comboboxtext_resol");
	GObject *label_sysresol = gtk_builder_get_object (builder, "label_sysresol");
	GObject *btn_save = gtk_builder_get_object (builder, "btn_save");
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	int layout3_width = 0;
	int layout3_height = 0;
	int layout2_width = 0;
	int layout2_height = 0;
	int pic_close_width = 0;
	int pic_close_height = 0;
	if (scr_width == 1920 && scr_height == 1080)
	{
	 	layout3_width = 800;
		layout3_height = 560;
		layout2_width = 800;
		layout2_height = 40;
		gdk_pixbuf_get_file_info("images2/close_set.png", &pic_close_width, &pic_close_height);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_resolution), 15, 15);
		gtk_widget_set_size_request(GTK_WIDGET(label_resolution), 100, 30);
		gtk_widget_set_size_request(GTK_WIDGET(btnset_close), pic_close_width, pic_close_height);
		setctrlFont(GTK_WIDGET(box_menu), 12);
		setctrlFont(GTK_WIDGET(comboboxtext_resol), 12);
		setctrlFont(GTK_WIDGET(label_sysresol), 12);
		setctrlFont(GTK_WIDGET(label_resolution), 12);
	}
	else if (scr_width == 1024 && scr_height == 768 || scr_width == 1440 && scr_height == 900 ||
		scr_width == 1600 && scr_height == 900)
	{
	    layout2_width = 500;
		layout2_height = 30;
		layout3_width = 500;
	 	layout3_height = 470;
		gdk_pixbuf_get_file_info("images2/1024x768/close_set_22.png", &pic_close_width, &pic_close_height);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_resolution), 8, 8);
		gtk_widget_set_size_request(GTK_WIDGET(eventbox_manualresol), 8, 8);
		gtk_widget_set_size_request(GTK_WIDGET(label_resolution), 100, 20);
		gtk_widget_set_size_request(GTK_WIDGET(label_sysresol), 60, 20);
		gtk_widget_set_size_request(GTK_WIDGET(btnset_close), pic_close_width, pic_close_height);
		int x,y = 0;
		int left = 30;
		int delay_height = 10;
		int delay_width = 10;
		int width_image = 15;
		int height_image = 15;
		int top = layout3_height - 240;
		x = left;
		y = top;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(separator2), x, y);
		x = left * 2;
		y = top + delay_height * 2;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(eventbox_resolution), x, y);
		x = left * 2 + width_image + delay_width;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(label_resolution), x, y);
		//manual_resol
		x = left * 2;
		y = top + delay_height * 2 + height_image + delay_height;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(eventbox_manualresol), x, y);
		x = left *2 + width_image;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(label_sysresol), x, y);
		x = left *2 + width_image + delay_width + 60 + delay_width;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(comboboxtext_resol), x, y);
		//btn_save
		x = 420 - delay_width - 62;
		y = 370 - 29 - delay_height*2;
		gtk_layout_move((GtkLayout *)layout_base, GTK_WIDGET(btn_save), x, y);
		//btn_close
	    x = layout2_width - delay_width - pic_close_width;
		y = (layout2_height - pic_close_height)/2;
		gtk_layout_move((GtkLayout *)layout2, GTK_WIDGET(btnset_close), x, y);
		setctrlFont(GTK_WIDGET(box_menu), 11);
		setctrlFont(GTK_WIDGET(label_sysresol), 9);
		setctrlFont(GTK_WIDGET(comboboxtext_resol), 9);
		setctrlFont(GTK_WIDGET(label_resolution), 9);
	}
}

static void  on_entry_serverip_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
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
	GObject *entry_serverip;
	GObject *entry_port;
	entry_serverip = gtk_builder_get_object (g_builder2, "entry_serverip");
    entry_port = gtk_builder_get_object (g_builder2, "entry_port");
	g_signal_connect(G_OBJECT(entry_serverip), "insert-text", G_CALLBACK(on_entry_serverip_insert_text), NULL);
	g_signal_connect(G_OBJECT(entry_port), "insert-text", G_CALLBACK(on_entry_port_insert_text), NULL);
}


int  SY_Setting_main ()
{
  if (showsettingwindow == 1)
     return;
  showsettingwindow = 1;
  //settingExit = 0;
  GtkBuilder *builder;
  GObject *window;
  GObject *boxmenu;
  GObject *boxwindow;
  GObject *button_11;

  /* Construct a GtkBuilder instance and load our UI description */
  builder = gtk_builder_new ();
  GError *errort = NULL;
  gtk_builder_add_from_file (builder, "setting.glade", &errort);
  window = gtk_builder_get_object (builder, "window1");
  g_window = window;
  boxmenu = gtk_builder_get_object (builder, "box_menu");
  boxwindow = gtk_builder_get_object (builder, "box_window");
  g_checkNorimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_NOR, NULL);
  g_checkPressimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_PRESS, NULL);
  g_checkrepass = 1;
  g_checkmanresol = 0;
  g_builder2 = builder;
  LogInfo("Debug：setting window 33333  first init.\n");
  loadimage(builder);
  SY_NetworkSet_main();
  g_layout2 = GetNetWorkSetLayout();
  init_ctrl_pos(builder);
  int_tmp(builder);
  loadcss();
  int box_win_width = 0;
  int box_win_height = 0;
  gtk_widget_get_size_request(GTK_WIDGET(boxwindow), &box_win_width, &box_win_height);
  setAbout_win_Size(box_win_width, box_win_height);
  SY_AboutWindow();
  g_layout_about = GetAboutLayout();

  GtkWidget *view;
  GtkTreeSelection *selection;
  GtkWidget *vbox1;
  view = create_view_and_model();
  GdkScreen* screen;
  screen = gdk_screen_get_default();
  int scr_width = gdk_screen_get_width(screen);
  int scr_height = gdk_screen_get_height(screen);
  if (scr_width == 1024 && scr_height == 768 ||
  	   scr_width == 1440 && scr_height == 900 ||
  	   scr_width == 1600 && scr_height == 900)
  {
	 setctrlFont(GTK_WIDGET(view), 10);
  }
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
  gtk_box_pack_start(GTK_BOX(boxmenu), view, TRUE, TRUE, 1);
  //树视图默认选中第一个节点。
  GtkTreeIter iter;
  GtkTreeModel* model;
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));
  gtk_tree_model_get_iter_first(model, &iter);
  gtk_tree_selection_select_iter(selection, &iter);
  g_layout1 = gtk_builder_get_object (builder, "layout_base");
  gtk_box_pack_start(GTK_BOX(boxwindow), (GtkWidget *)g_layout2, TRUE, TRUE, 1);
  gtk_box_pack_start(GTK_BOX(boxwindow), (GtkWidget *)g_layout_about, TRUE, TRUE, 1);

  GObject * label_tip;
  GObject * label_serverip;
  GObject * label_port;
  GObject * label_user;
  GObject * label_pass;
  GObject * label_resolution;
  GObject * label_sysresol;
  label_tip = gtk_builder_get_object (builder, "label_tip");
  label_serverip = gtk_builder_get_object (builder, "label_serverip");
  label_port = gtk_builder_get_object (builder, "label_port");
  label_user = gtk_builder_get_object (builder, "label_user");
  label_pass = gtk_builder_get_object (builder, "label_pass");
  label_resolution = gtk_builder_get_object (builder, "label_resolution");
  label_sysresol = gtk_builder_get_object (builder, "label_sysresol");
  gtk_label_set_justify(GTK_LABEL(label_serverip), GTK_JUSTIFY_RIGHT);
  gtk_label_set_justify(GTK_LABEL(label_port), GTK_JUSTIFY_RIGHT);
  gtk_label_set_justify(GTK_LABEL(label_user), GTK_JUSTIFY_RIGHT);
  gtk_label_set_justify(GTK_LABEL(label_pass), GTK_JUSTIFY_RIGHT);

  gtk_label_set_text(GTK_LABEL(label_tip), "在首次使用之前请正确设置服务器地址和端口，\n 如果您不了解这些设置，请联系你的管理员。");
  gtk_label_set_text(GTK_LABEL(label_serverip), "服务器地址");
  gtk_label_set_text(GTK_LABEL(label_port), "端口");
  gtk_label_set_text(GTK_LABEL(label_user), "用户名");
  gtk_label_set_text(GTK_LABEL(label_pass), "密码");
  gtk_label_set_text(GTK_LABEL(label_resolution), "根据客户端分辨率调整");
  gtk_label_set_text(GTK_LABEL(label_sysresol), "手动调整");

  GObject * btn_save;
  btn_save = gtk_builder_get_object (builder, "btn_save");
  gtk_button_set_label(GTK_BUTTON(btn_save), "保存");
  /* Connect signal handlers to the constructed widgets. */
  gtk_window_set_title(window, "配置");
  g_signal_connect(selection, "changed", G_CALLBACK(on_changed), NULL);
  g_signal_connect(window, "window-state-event", G_CALLBACK(on_window_state_event), NULL);
//  g_signal_connect (button_11, "clicked", G_CALLBACK (on_button_11_clicked), NULL);
  GObject *eventbox_resolution;
  eventbox_resolution = gtk_builder_get_object (builder, "eventbox_resolution");
  gtk_widget_set_events((GtkWidget *)eventbox_resolution, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件
  GObject *eventbox_manualresol;
  eventbox_manualresol = gtk_builder_get_object (builder, "eventbox_manualresol");
  gtk_widget_set_events((GtkWidget *)eventbox_manualresol, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件

  GObject *image_resolution;
  image_resolution = gtk_builder_get_object (builder, "image_resolution");
  gtk_image_set_from_pixbuf(GTK_IMAGE(image_resolution), g_checkNorimage);
  g_signal_connect (G_OBJECT (eventbox_resolution), "button_press_event",
                  G_CALLBACK (checkbutton_resolution_press_callback), (GtkWidget *)image_resolution);
  GObject *image_manualresol;
  image_manualresol = gtk_builder_get_object (builder, "image_manualresol");
  gtk_image_set_from_pixbuf(GTK_IMAGE(image_manualresol), g_checkNorimage);
  g_signal_connect (G_OBJECT (eventbox_manualresol), "button_press_event",
                  G_CALLBACK (checkbutton_manualresol_press_callback), (GtkWidget *)image_manualresol);

  gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("images2/logo.png"));
  gtk_widget_add_events(GTK_WIDGET(window), GDK_BUTTON_PRESS_MASK);
  g_signal_connect(G_OBJECT(window), "button-press-event", G_CALLBACK(window_drag), NULL);
  //g_signal_connect(G_OBJECT(window), "configure-event", G_CALLBACK(window_move), NULL);
  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit_setting), NULL);
  g_signal_connect(GTK_BUTTON(btn_save), "clicked", G_CALLBACK(save_button_clicked), NULL);

  //add by zhao
  GObject * label_title;
  GObject * btnset_close;
  label_title = gtk_builder_get_object (builder, "label_title");
  btnset_close = gtk_builder_get_object (builder, "btnset_close");
  gtk_label_set_text(GTK_LABEL(label_title), "配 置");
  g_signal_connect(GTK_BUTTON(btnset_close), "clicked", G_CALLBACK(close_button_clicked), NULL);
  //end
  gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
  gtk_widget_show_all((GtkWidget *)window);
  gtk_widget_set_visible((GtkWidget *)g_layout2, 0);
  gtk_widget_set_visible((GtkWidget *)g_layout_about, 0);
  SY_NetworkDisableCtrl();
  initctrl();
  setResol();
  init_entry_event();
  gtk_main ();
  g_object_unref (G_OBJECT (builder));
  g_builder2 = NULL;
  return 0;
}
