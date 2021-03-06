#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include <unistd.h>

static const gchar*  g_symsgcss = "systhrdmg.css";
static GtkBuilder *builder;
static GObject  *window = NULL;
static int showSyMsgDlg = 0;
GtkLabel *labeltext;

extern void setctrlFont(GtkWidget * widget, int nsize);

void closeAutoDialog()
{
	if (window != NULL)
    {
        printf("closeAutoDialog   fdfdfdfdfdf.\n");
	    gtk_widget_destroy((GtkWidget *)window);
		window = NULL;
	    gtk_main_quit();
    	}
}

static gboolean close_button_clicked(GtkButton *button,  gpointer user_data)
{
    if (window != NULL)
    {
	    gtk_widget_destroy((GtkWidget *)window);
		window = NULL;
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
	}else if ((int)user_data == 15)
	{
		system("sudo echo 1 > /sys/class/graphics/fb0/blank");
		system("sudo reboot");
	}
	else
	{
#ifdef ARM
		system("sudo echo 1 > /sys/class/graphics/fb0/blank");
#endif
		system("sudo poweroff");
		if (window != NULL)
	    {
		    gtk_widget_destroy((GtkWidget *)window);
			window = NULL;
		    gtk_main_quit();
	    	}
	}
}

static gboolean Cancel_button_clicked(GtkButton *button,  gpointer user_data)
{
    if (window != NULL)
    {
	    gtk_widget_destroy((GtkWidget *)window);
		window = NULL;
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
	}else if ((scr_width == 1440 && scr_height == 900 ) || 
			(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || 
			(scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050))
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
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	  (scr_width == 1280 && scr_height == 1024) )
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
	}else if ( (scr_width == 1366 && scr_height == 768) || (scr_width == 1368 && scr_height == 768) ||
	   (scr_width == 1360 && scr_height == 768) )
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
	if ((scr_width == 1024 && scr_height == 768) || 
		(scr_width == 1440 && scr_height == 900) || 
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || 
		(scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050))
	{
		gdk_pixbuf_get_file_info("images2/1024x768/close_set_22.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1024x768/logo22.png", &pic_logo_width, &pic_logo_height);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		gdk_pixbuf_get_file_info("images2/close_set.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/logo.png", &pic_logo_width, &pic_logo_height);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	   (scr_width == 1280 && scr_height == 1024) )
	{
		gdk_pixbuf_get_file_info("images2/1024x768/close_set_22.png", &pic_close_width, &pic_close_height);
		gdk_pixbuf_get_file_info("images2/1024x768/logo22.png", &pic_logo_width, &pic_logo_height);
	}else if ((scr_width == 1366 && scr_height == 768) || (scr_width == 1368 && scr_height == 768) || 
	   (scr_width == 1360 && scr_height == 768) )
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
	if ((scr_width == 1024 && scr_height == 768) || 
		(scr_width == 1440 && scr_height == 900) || 
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   ||
		(scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050))
	{
		g_pixlogo = gdk_pixbuf_new_from_file("images2/1024x768/logo22.png", NULL);
		g_pixclose = gdk_pixbuf_new_from_file("images2/1024x768/close_set_22.png", NULL);
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		g_pixlogo = gdk_pixbuf_new_from_file("images2/logo22.png", NULL);
		g_pixclose = gdk_pixbuf_new_from_file("images2/close_set_22.png", NULL);
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) || 
	  (scr_width == 1280 && scr_height == 1024) )
	{
		g_pixlogo = gdk_pixbuf_new_from_file("images2/1024x768/logo22.png", NULL);
		g_pixclose = gdk_pixbuf_new_from_file("images2/1024x768/close_set_22.png", NULL);
	}else if ((scr_width == 1366 && scr_height == 768) || (scr_width == 1368 && scr_height == 768) || 
		(scr_width == 1360 && scr_height == 768))
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

//single tips
void SYMsgDialog2(int nflag, char *msg)
{
    if (showSyMsgDlg == 1)
        return;
    showSyMsgDlg = 1;
    builder = gtk_builder_new();
    GError *error1 = NULL;
    gtk_builder_add_from_file (builder, "SYthrdMsgDlg2.glade", &error1);
    window = gtk_builder_get_object(builder, "window_symsg2");
    GObject * label_thrdtext;
    label_thrdtext = gtk_builder_get_object(builder, "label_thrdtext");
    labeltext = (GtkLabel *)label_thrdtext;
    if (nflag == 7 || nflag == 11 || nflag == 4 || nflag == 20 || nflag == 23 || nflag == 13 || nflag == 15)
        gtk_label_set_text(GTK_LABEL(label_thrdtext), msg);
    else if (nflag == 0)
    {
        gtk_label_set_text(GTK_LABEL(label_thrdtext), "正在连接，请稍后 ... ");
    }
	loadcss();
	init_ctrl_size(builder);
	init_ctrl_image(builder);	
    GObject * btnsymsg_close;
    btnsymsg_close = gtk_builder_get_object(builder, "btnsymsg_close");
    g_signal_connect(GTK_BUTTON(btnsymsg_close), "clicked", G_CALLBACK(close_button_clicked), NULL);
    g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(gtk_main_quit), NULL);
    if (nflag == 0 || nflag == 1)
        ShenCloud_login();
    //ok-cancel btn
    GObject * btn_OK;
    GObject * btn_Cancel;
    btn_OK = gtk_builder_get_object(builder, "btn_OK");
    btn_Cancel = gtk_builder_get_object(builder, "btn_Cancel");
    gtk_button_set_label(GTK_BUTTON(btn_OK), "是");
    gtk_button_set_label(GTK_BUTTON(btn_Cancel), "否");
    if (nflag == 11 || nflag == 20 || nflag == 15)
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
    g_signal_connect(GTK_BUTTON(btn_OK), "clicked", G_CALLBACK(OK_button_clicked), nflag);
    g_signal_connect(GTK_BUTTON(btn_Cancel), "clicked", G_CALLBACK(Cancel_button_clicked), NULL);
	gtk_window_set_modal (GTK_WINDOW (window), TRUE);
    gtk_main();
    g_object_unref (G_OBJECT(builder));
    showSyMsgDlg = 0;
}
