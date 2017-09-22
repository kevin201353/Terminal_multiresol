#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "global.h"

static const char *g_home_css2 = "modifypass.css";
static int stwidth = 0;
static int stheight = 0;
static GtkBuilder *builder;
GObject * layout_modifypass;
GtkWidget * label_oldpass = NULL;
GtkWidget * label_newpass = NULL;
GtkWidget * btn_savemodifypass = NULL;
static  char g_url[512] = {0};
static struct ServerInfo  serverInfo;
static struct LoginInfo   mloginfo;
static float g_scr_old_width = 1920.0;
static float g_scr_old_height = 1080.0;


void get_widget_xy(GtkContainer* container, GtkWidget *widget, int *x, int *y)
{
	if (NULL == container || NULL == widget)
		return;
	char szInfo[100] = {0};
	GValue a = G_VALUE_INIT;
	g_value_init (&a, G_TYPE_INT);
	gtk_container_child_get_property(GTK_CONTAINER(container), GTK_WIDGET(widget), "x", &a);
	*x = g_value_get_int(&a);
	sprintf(szInfo, "get widget xxxx : x = %d", *x);
	LogInfo(szInfo);
	GValue b = G_VALUE_INIT;
	g_value_init (&b, G_TYPE_INT);
	gtk_container_child_get_property(GTK_CONTAINER(container), GTK_WIDGET(widget), "y", &b);
	*y = g_value_get_int(&b);
	sprintf(szInfo, "get widget xxxx : y = %d", *y);
	LogInfo(szInfo);
}

void my_container_resize2(GtkWindow* window, GtkContainer* container, float factorX, float factorY)
{
	if (NULL == container)
		return;
	
	char szInfo[100] = {0};
	int width = 0;
	int height = 0;
	int x =0;
	int y =0;
	get_widget_xy((GtkContainer*)window, container, &x, &y);
	int fact_x = (int)(x*factorX);
    int fact_y = (int)(y*factorY);
	gtk_layout_move((GtkContainer*)window, container, fact_x, fact_y);
	gtk_widget_get_size_request((GtkWidget *)container, &width, &height);
	sprintf(szInfo, "my_container_resize2, width : %d , height :%d ", width, height);
	LogInfo(szInfo);
	int fact_width = (int)(width*factorX);
	int fact_height = (int)(height*factorY);
	sprintf(szInfo, "my_container_resize2, fact_width : %d , fact_height :%d ", width, height);
	LogInfo(szInfo);
	gtk_widget_set_size_request((GtkWidget *)container, fact_width, fact_height);
}

void my_container_resize(GtkContainer* container, float factorX, float factorY)
{
	if (NULL == container)
		return;
	
	char szInfo[100] = {0};
	int width = 0;
	int height = 0;
	gtk_widget_get_size_request((GtkWidget *)container, &width, &height);
	sprintf(szInfo, "my_container_resize, width : %d , height :%d ", width, height);
	LogInfo(szInfo);
	int fact_width = (int)(width*factorX);
	int fact_height = (int)(height*factorY);
	sprintf(szInfo, "my_container_resize, fact_width : %d , fact_height :%d ", width, height);
	LogInfo(szInfo);
	gtk_widget_set_size_request((GtkWidget *)container, fact_width, fact_height);
}

void my_resizemain(GtkWidget *widget, float factorX, float factorY)
{
	char szInfo[100] = {0};
	int width = 0;
	int height = 0;
	gtk_widget_get_size_request(widget, &width, &height);
	sprintf(szInfo, "my_resizemain widget, width : %d , height :%d ", width, height);
	LogInfo(szInfo);
    int fact_width = (int)(width*factorX);
    int fact_height = (int)(height*factorY);
	sprintf(szInfo, "my_resizemain new fact_width: %d, fact_height: %d", fact_width, fact_height);
	LogInfo(szInfo);
	gtk_widget_set_size_request(widget, fact_width, fact_height);
}
void my_resize(GtkContainer* container, GtkWidget *widget, float factorX, float factorY)
{
	if (NULL == container || NULL == widget)
		return;
	char szInfo[100] = {0};
	int width = 0;
	int height = 0;
	gtk_widget_get_size_request(widget, &width, &height);
	sprintf(szInfo, "my_resize widget, width : %d , height :%d ", width, height);
	LogInfo(szInfo);
	int x = 0;
	int y = 0;
	get_widget_xy(container, widget, &x, &y);
    sprintf(szInfo, "my_resize widget xy, factorX: %f, factorY: %f, x : %d , y :%d ", factorX, factorY, x, y);
	LogInfo(szInfo);
	int fact_x = (int)(x*factorX);
    int fact_y = (int)(y*factorY);
    int fact_width = (int)(width*factorX);
    int fact_height = (int)(height*factorY);
	sprintf(szInfo, "my_resize new fact_x : %d, fact_y :%d, fact_width: %d, fact_height: %d", fact_x, fact_y, fact_width, fact_height);
	LogInfo(szInfo);
	gtk_layout_move((GtkLayout *)container, widget, fact_x, fact_y);
	gtk_widget_set_size_request(widget, fact_width, fact_height);
}

static int initUrl()
{
	if ( GetServerInfo2(&serverInfo) < 0 )
	{
	 	return -1;
	}
	char sztmp[20] = {0};
	sprintf(sztmp, "%d", serverInfo.nport);
	strcpy(g_url, "http://");
	strcat(g_url, serverInfo.szIP);
	strcat(g_url, "/");
	return 0;
}


static gboolean btn_savemodifypass_clicked(GtkButton *button,  gpointer user_data)
{
	GObject * entry_oldpass = gtk_builder_get_object(builder, "entry_oldpass");
	GObject * entry_newpass = gtk_builder_get_object(builder, "entry_newpass");
	char sz_oldpass[20] = {0};
	char sz_newpass[20] = {0};
	strcpy(sz_oldpass, gtk_entry_get_text(GTK_ENTRY(entry_oldpass)));
	strcpy(sz_newpass, gtk_entry_get_text(GTK_ENTRY(entry_newpass)));
	char szInfo[100] = {0};
	sprintf(szInfo, "oldpass : %s , newpass :%s ", sz_oldpass, sz_newpass);
	LogInfo(szInfo);
	/*
	if (Ovirt_ModifyPass(g_url, mloginfo.user, sz_oldpass, sz_newpass) < 0)
	{
		LogInfo("modifypass modify pass failed.");
		return FALSE;
	}*/
	close_setting_window();
	return TRUE;
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
                                  g_filename_to_utf8(g_home_css2, strlen(g_home_css2), &bytes_read, &bytes_written, &error),
                                  NULL);
    //gtk_css_provider_load_from_path (provider, home, NULL);
    g_object_unref (provider);
}

void set_modify_pass_win_size(int width, int height)
{
	stwidth = width;
	stheight = height;
}

GObject * GetModifyPassLayout()
{
     return layout_modifypass;
}

void modify_pass_window()
{
	GObject *window;
	GError *error = NULL;
	builder = gtk_builder_new();
	gtk_builder_add_from_file (builder, "modifypass.glade", &error);
	layout_modifypass = gtk_builder_get_object(builder, "layout_modifypass");
	gtk_widget_set_size_request(GTK_WIDGET(layout_modifypass), stwidth, stheight);
	label_oldpass = gtk_builder_get_object(builder, "label_oldpass");
	label_newpass = gtk_builder_get_object(builder, "label_newpass");
	btn_savemodifypass = gtk_builder_get_object(builder, "btn_savemodifypass");
	if (NULL != label_oldpass)
	{
		gtk_label_set_text(GTK_LABEL(label_oldpass), "旧密码");
	}
	if (NULL != label_newpass)
	{
		gtk_label_set_text(GTK_LABEL(label_newpass), "新密码");
	}

	if (NULL != btn_savemodifypass)
	{
		gtk_button_set_label(GTK_BUTTON(btn_savemodifypass), "保存");
	}
	loadcss();
	g_signal_connect(GTK_BUTTON(btn_savemodifypass), "clicked", G_CALLBACK(btn_savemodifypass_clicked), NULL);
	initUrl();
	GetLoginInfo(&mloginfo);
	char szInfo[100] = {0};
	sprintf(szInfo, "modiypass ovirt_url :%s, login_user :%s", g_url, mloginfo.user);
	LogInfo(szInfo);
	char *p = strstr(mloginfo.user, "admin");
	GObject * entry_oldpass = gtk_builder_get_object(builder, "entry_oldpass");
	GObject * entry_newpass = gtk_builder_get_object(builder, "entry_newpass");
	if (NULL != p)
	{
		gtk_widget_set_sensitive((GtkWidget *)entry_oldpass, FALSE);
		gtk_widget_set_sensitive((GtkWidget *)entry_newpass, FALSE);
	}else
	{
		gtk_widget_set_sensitive((GtkWidget *)entry_oldpass, TRUE);
		gtk_widget_set_sensitive((GtkWidget *)entry_newpass, TRUE);
	}

	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	float factor_x = (float)scr_width/g_scr_old_width;
	float factor_y = (float)scr_height/g_scr_old_height;
	my_container_resize((GtkContainer *)layout_modifypass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_modifypass, (GtkWidget *)label_oldpass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_modifypass, (GtkWidget *)label_newpass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_modifypass, (GtkWidget *)entry_oldpass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_modifypass, (GtkWidget *)entry_newpass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_modifypass, (GtkWidget *)btn_savemodifypass, factor_x, factor_y);
}

