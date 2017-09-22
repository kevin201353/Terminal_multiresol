#include <gtk/gtk.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include <math.h>


static GdkPixbuf * g_pixbuf = NULL;
static pthread_t wait_win_tid;

int  g_exit_waitting;

#define  MAX_TEXT   100
#define  MAX_SIZE   100
struct msg_t{
	long int msg_type;
	char text[MAX_TEXT];
};

static int g_msgid = -1;
static void create_msg_queue()
{
    g_msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (g_msgid == -1)
    {
        fprintf(stderr, "msgget failed with error: %d\n", errno);
        //exit(EXIT_FAILURE);
    }
}

static struct msg_t data;
static void msg_recv(char* dataT)
{
    long int msgtype = 1;
    if ( msgrcv(g_msgid, (void*)&data, MAX_TEXT, msgtype, IPC_NOWAIT) == -1 )
    {
        fprintf(stderr, "msgrcv failed with errno: %d\n", errno);
        //exit(EXIT_FAILURE);
    }
    strcpy(dataT, (char*)data.text);
}

static void msg_queue_del()
{
    if (g_msgid != -1)
    {
        if (msgctl(g_msgid, IPC_RMID, 0) == -1)
        {
            fprintf(stderr, "msgctl(IPC_RMID) failed.\n");
            //exit(EXIT_FAILURE);
        }
    }
}

//void msg_respose(struct ReportMsg msg)
//{
//	switch(msg.action)
//	{
//		case MSG_WINDOW_CLOSE:
//			g_thrdMsgExit = 1;
//			//LogInfo("msg_respose : exit wait window recv msg.\n");
//			printf("msg_respose : exit wait window recv msg.\n");
//			close_msg_sender();
//			gtk_main_quit();
//			break;
//		default:
//			break;
//	}
//}

//static gboolean time_handler()
//{
//	FILE* fp = fopen("wait_signal", "r");
//	if (fp != NULL)
//	{
//		 while(fgets(buf,MAX_BUFF_SIZE, fp) > 0)
//		{
//		    printf("xxxxx time_handler buf = %s.\n", buf);
//			if (strcmp(buf, "2") == 0)
//			{  
//				gtk_main_quit();
//			}
//			sleep(1);
//		 }
//		 
//	}
//	fclose(fp);
//	return TRUE;
//}

static void exec_wait_window()
{
	//FILE* fp = NULL;
	char data[MAX_SIZE] = {0};
	while(1)
	{
//		FILE* fp = fopen("wait_signal", "r");
//		if (fp != NULL)
//		{
//			while(fgets(buf, MAX_BUFF_SIZE, fp) > 0)
//			{
//			    printf("xxxxx time_handler buf = %s.\n", buf);
//				if (strcmp(buf, "2") == 0)
//				{  
//					gtk_main_quit();
//					fclose(fp);
//					return;
//				}
//				sleep(1);
//			 }
//		}
		memset(data, 0, MAX_SIZE);
		msg_recv(data);
		if (strncmp(data, "2", strlen("2")) == 0)
		{
			break;
		}
		sleep(1);
	}
	//fclose(fp);
	gtk_main_quit();
}

static void do_drawing(cairo_t *, GtkWidget *);
static cairo_surface_t *surface;
GtkWidget *darea;
  
struct {
  gushort count;
} glob;

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, 
    gpointer user_data)
{ 
     GdkWindow *window;
    cairo_t *cr1 = NULL;
    window = gtk_widget_get_window(widget);
    cr1 = gdk_cairo_create(window);
    cairo_set_source_surface (cr1, surface, 0, 0);
    cairo_paint (cr1);
    cairo_destroy (cr1);
    do_drawing(cr, widget);
  return FALSE;
}

static void do_drawing(cairo_t *cr, GtkWidget *widget)
{  
  static gdouble const trs[8][8] = {
      { 0.0, 0.15, 0.30, 0.5, 0.65, 0.80, 0.9, 1.0 },
      { 1.0, 0.0,  0.15, 0.30, 0.5, 0.65, 0.8, 0.9 },
      { 0.9, 1.0,  0.0,  0.15, 0.3, 0.5, 0.65, 0.8 },
      { 0.8, 0.9,  1.0,  0.0,  0.15, 0.3, 0.5, 0.65},
      { 0.65, 0.8, 0.9,  1.0,  0.0,  0.15, 0.3, 0.5 },
      { 0.5, 0.65, 0.8, 0.9, 1.0,  0.0,  0.15, 0.3 },
      { 0.3, 0.5, 0.65, 0.8, 0.9, 1.0,  0.0,  0.15 },
      { 0.15, 0.3, 0.5, 0.65, 0.8, 0.9, 1.0,  0.0, }
  };

  GtkWidget *win = gtk_widget_get_toplevel(widget);
  
  gint width, height;
  gtk_window_get_size(GTK_WINDOW(win), &width, &height);

  cairo_translate(cr, width/2, height/2);

  gint i = 0;
  for (i = 0; i < 8; i++) {
      cairo_set_line_width(cr, 3);
      cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
      cairo_set_source_rgba(cr, 0, 0, 0, trs[glob.count%8][i]);

      cairo_move_to(cr, 0.0, -10.0);
      cairo_line_to(cr, 0.0, -40.0);
      cairo_rotate(cr, M_PI/4);
      cairo_stroke(cr);
  }   
}

static gboolean time_handler(GtkWidget *widget)
{
  if (g_exit_waitting == 1)
  	return FALSE;
  glob.count += 1;
  gtk_widget_queue_draw(widget);
  return TRUE;
}

void wait_show(GtkWidget *window, cairo_surface_t *surface1)
{
	g_exit_waitting = 0;
	glob.count = 0;
	surface = surface1;
	darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER (window), darea);
	gtk_widget_set_size_request(GTK_WIDGET(darea), 60, 60);
	GdkScreen* screen;
    screen = gdk_screen_get_default();
    int scr_width = gdk_screen_get_width(screen);
    int scr_height = gdk_screen_get_height(screen);
	int x, y = 0;
	x = scr_width/2 - 60;
	//y = scr_height/2 - win_height/2;
	y = scr_height - 20 - 60;
	gtk_widget_set_app_paintable(GTK_WIDGET(window), TRUE);
	 g_signal_connect(G_OBJECT(darea), "draw", 
      G_CALLBACK(on_draw_event), NULL);
	 g_timeout_add(100, (GSourceFunc) time_handler, (gpointer)window);
	//gtk_widget_move(darea, x, y);
}

//int main(int argc, char *argv[])
//{
//  GtkWidget *window;
//  
//  glob.count = 0;

//  gtk_init(&argc, &argv);

//  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//  
//  int pic_width = 0;
//  int pic_height = 0;
//  surface = cairo_image_surface_create_from_png ("login.png");
//  gdk_pixbuf_get_file_info("login.png", &pic_width, &pic_height);
//  darea = gtk_drawing_area_new();
//  gtk_container_add(GTK_CONTAINER (window), darea);
//  gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
// /*  GdkRGBA color1;
//  gdk_rgba_parse(&color1, "#2C3E60");
//  gtk_widget_override_background_color(GTK_WIDGET(window), GTK_STATE_FLAG_NORMAL, &color1);
//  gtk_widget_override_background_color(GTK_WIDGET(darea), GTK_STATE_FLAG_NORMAL, &color1); */
//  gtk_widget_set_app_paintable(GTK_WIDGET(window), TRUE);
//  g_signal_connect(G_OBJECT(darea), "draw", 
//      G_CALLBACK(on_draw_event), NULL);  
//  g_signal_connect(G_OBJECT(window), "destroy",
//      G_CALLBACK(gtk_main_quit), NULL);

//  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
//  gtk_widget_set_size_request(GTK_WINDOW(window), pic_width, pic_height); 
//  gtk_widget_set_size_request(GTK_WIDGET(darea), 60, 60);
//  gtk_window_set_title(GTK_WINDOW(window), "Waiting demo");

//  g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);
//  gtk_widget_show_all(window);  
//  create_msg_queue();
//	if ( pthread_create(&wait_win_tid, NULL, exec_wait_window, NULL) !=0 )
//	{
//		printf("Create start_wait_window thrd error!\n");
//	}
//  gtk_main();
//  cairo_surface_destroy (surface);
//  return 0;
//}

