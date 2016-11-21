#include <gtk/gtk.h>
#include "msg.h"

void msg_respose(struct ReportMsg msg)
{
	switch(msg.action)
	{
		case MSG_WINDOW_CLOSE:
			g_thrdMsgExit = 1;
			//LogInfo("msg_respose : exit wait window recv msg.\n");
			printf("msg_respose : exit wait window recv msg.\n");
			close_msg_sender();
			gtk_main_quit();
			break;
		default:
			break;
	}
}

int main(int argc,char **argv)
{
	GtkWidget *win;
	GtkWidget *image;
	int pic_width = 0;
	int pic_height = 0;
	gtk_init(&argc,&argv);
	GdkScreen* screen;
    screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	win=gtk_window_new(GTK_WINDOW_POPUP);
	gtk_window_set_decorated(GTK_WINDOW(win), FALSE);
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
	image = gtk_image_new_from_file("images2/loading.gif");
	gdk_pixbuf_get_file_info("images2/loading.gif", &pic_width, &pic_height);
	gtk_widget_set_size_request(GTK_WIDGET(win), pic_width, pic_height);
	gtk_container_add(GTK_CONTAINER(win), image);
	gtk_window_set_modal (GTK_WINDOW (win), TRUE);	
	GdkRGBA color1;
//	GdkRGBA color2;
//	color2.red = 51;
//	color2.green = 44;
//	color2.blue = 43;
//	color2.alpha = 0.4;
//    gdk_rgba_parse(&color1, /*gdk_rgba_to_string(&color2)*/"#EEF3FA");
//    gtk_widget_override_background_color(GTK_WIDGET(win), GTK_STATE_FLAG_NORMAL, &color1);
	int x, y = 0;
	x = scr_width/2 - pic_width/2;
	//y = scr_height/2 - win_height/2;
	y = scr_height - 20 - pic_height;
	gtk_window_move(GTK_WINDOW(win), x, y);
	gtk_widget_show_all(win);
	start_msg_sender();
	gtk_main();
	g_thrdMsgExit = 0;
	return 0;
}
