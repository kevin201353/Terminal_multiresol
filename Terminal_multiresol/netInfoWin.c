#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include <unistd.h>
#include <string.h>

static const gchar*  g_symsgcss = "systhrdmg.css";
static GtkBuilder *builder;
static GObject  *window = NULL;
static int showSyMsgDlg = 0;
GtkLabel *labeltext;

extern void setctrlFont(GtkWidget * widget, int nsize);
static int g_window_exit = 0;

static gboolean close_button_clicked(GtkButton *button,  gpointer user_data)
{
    if (window != NULL)
    {
    		g_window_exit = 1;
	    gtk_widget_destroy((GtkWidget *)window);
		window = NULL;
		showSyMsgDlg = 0;
	    gtk_main_quit();
    	}
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
	if ((scr_width == 1024 && scr_height == 768) || (scr_width == 1440 && scr_height == 900) ||
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
	{
		win_width = 300;
		win_height = 300;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200) )
	{
		win_width = 470;
		win_height = 470;
	}else if ( (scr_width == 1280 && scr_height == 720) || (scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) || (scr_width == 1360 && scr_height == 768) || (scr_width == 1280 && scr_height == 768) ||
		(scr_width == 1280 && scr_height == 1024))
	{
		win_width = 300;
		win_height = 300;
	}
	GObject *layout_thrdtitle = gtk_builder_get_object(builder, "layout_thrdtitle");
	GObject *layout_thrdtext = gtk_builder_get_object(builder, "layout_thrdtext");
	GObject *fixed_thrdmsg = gtk_builder_get_object(builder, "fixed_thrdmsg");
	GObject *label_thrdtext = gtk_builder_get_object(builder, "label_thrdtext");
	int label_text_width = 0;
	int label_text_height = 0;
	int layout_thrdtitle_height = 0;
	int layout_thrdtext_height = 0;
	int nsize = 0;
	if (scr_width == 1024 && scr_height == 768)
	{
		win_width = 300;
		win_height = 220;
		label_text_width = 220;
		label_text_height = 60;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 200;
		nsize = 8;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		win_width = 470;
		win_height = 220;
		label_text_width = 350;
		label_text_height = 80;
		layout_thrdtitle_height = 30;
		layout_thrdtext_height = 200;
		nsize = 12;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  ||
	  (scr_width == 1600 && scr_height == 1080) )
	{
		win_width = 470;
		win_height = 220;
		label_text_width = 350;
		label_text_height = 80;
		layout_thrdtitle_height = 30;
		layout_thrdtext_height = 200;
		nsize = 10;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	    (scr_width == 1280 && scr_height == 1024))
	{
		win_width = 300;
		win_height = 220;
		label_text_width = 220;
		label_text_height = 60;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 200;
		nsize = 9;
	}else if ( (scr_width == 1366 && scr_height == 768) ||
		(scr_width == 1368 && scr_height == 768) ||
		(scr_width == 1360 && scr_height == 768))
	{
		win_width = 300;
		win_height = 220;
		label_text_width = 220;
		label_text_height = 60;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 200;
		nsize = 9;
	}
	gtk_widget_set_size_request(GTK_WINDOW(window), win_width, win_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout_thrdtitle), win_width, layout_thrdtitle_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout_thrdtext), win_width, layout_thrdtext_height);
	gtk_widget_set_size_request(GTK_WIDGET(fixed_thrdmsg), win_width, win_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_thrdtext), label_text_width, label_text_height);
	//
	setctrlFont(GTK_WIDGET(layout_thrdtext), nsize);
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
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
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
	//label_thrdtext
	x = win_width/2  - label_text_width/2;
	y = 20;
	gtk_layout_move((GtkLayout *)layout_thrdtext, GTK_WIDGET(label_thrdtext), x, y);
	//close
	x = win_width - 10 - pic_close_width;
	y = (layout_thrdtitle_height - pic_close_height)/2;
	gtk_layout_move((GtkLayout *)layout_thrdtitle, GTK_WIDGET(btnsymsg_close), x, y);
	//image logo
	x = 1;
	y = (layout_thrdtitle_height - pic_logo_height)/2;
	gtk_layout_move((GtkLayout *)layout_thrdtitle, GTK_WIDGET(image_sylogo), x, y);
	//move main dialog position
	x = scr_width/2 - win_width/2;
	y = scr_height/2 - win_height/2;
	gtk_window_move(GTK_WINDOW(window), x, y);
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
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )  || (scr_width == 1600 && scr_height == 1080))
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
}

char szinfo[1024] = {0};

static int getnetinfo()
{
	int nRet = system("sudo ./netget.sh");
    if (nRet < 0)
        return -1;

    FILE* fp=fopen("nettmp.txt", "r");
    if (fp == NULL)
       return -1;

    char sztmp2[MAX_BUFF_SIZE] = {0};
    char szTmp[MAX_BUFF_SIZE] = {0};
    fgets(szTmp, MAX_BUFF_SIZE, fp);
    szTmp[strlen(szTmp) - 1] = '\0';
    sprintf(sztmp2, "IP地址    :  %s", szTmp);
	strcat(szinfo, sztmp2);

	strcat(szinfo, "\n");
    memset(szTmp, 0, MAX_BUFF_SIZE);
    fgets(szTmp, MAX_BUFF_SIZE, fp);
    szTmp[strlen(szTmp) - 1] = '\0';
    sprintf(sztmp2, "子网掩码:  %s", szTmp);
	strcat(szinfo, sztmp2);
	strcat(szinfo, "\n");


    memset(szTmp, 0, MAX_BUFF_SIZE);
    fgets(szTmp, MAX_BUFF_SIZE, fp);
    szTmp[strlen(szTmp) - 1] = '\0';
    sprintf(sztmp2, "默认网关:  %s", szTmp);
	strcat(szinfo, sztmp2);
	strcat(szinfo, "\n");

	int i = 0;
	for (; i<3; i++)
	{
		memset(szTmp, 0, MAX_BUFF_SIZE);
		fgets(szTmp, MAX_BUFF_SIZE, fp);
		if (strcmp(szTmp, "") != 0 && strlen(szTmp) > 0)
		{
			szTmp[strlen(szTmp) - 1] = '\0';
		    sprintf(sztmp2, "DNS%d服务器:  %s", i, szTmp);
			strcat(szinfo, sztmp2);
			strcat(szinfo, "\n");
		}
	}
	LogInfo("Debug: netInfoWin output net info = %s .\n", szinfo);
	fclose(fp);
    system("rm -rf nettmp.txt");
    return 0;
}

void SYNetInfoWin()
{
    if (showSyMsgDlg == 1)
    	{
    	    printf("Debug: SYNetInfoWin SYNetInfoWin==1 return .\n");
        return;
    	}
	printf("Debug: SYNetInfoWin enter.\n");
    showSyMsgDlg = 1;
	g_window_exit = 0;
    builder = gtk_builder_new();
    GError *error1 = NULL;
    gtk_builder_add_from_file (builder, "netInfoWin.glade", &error1);
    window = gtk_builder_get_object(builder, "window_netinfo");
    GObject * label_thrdtext;
    label_thrdtext = gtk_builder_get_object(builder, "label_thrdtext");
    labeltext = (GtkLabel *)label_thrdtext;
	loadcss();
	init_ctrl_size(builder);
	init_ctrl_image(builder);
    GObject * btnsymsg_close;
    btnsymsg_close = gtk_builder_get_object(builder, "btnsymsg_close");
    g_signal_connect(GTK_BUTTON(btnsymsg_close), "clicked", G_CALLBACK(close_button_clicked), NULL);
    g_signal_connect (G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL);
	memset(szinfo, 0, sizeof(char) * 1024);
	getnetinfo();
    gtk_label_set_text(GTK_LABEL(label_thrdtext), szinfo);
    gtk_main();
    g_object_unref (G_OBJECT(builder));
    showSyMsgDlg = 0;
}
