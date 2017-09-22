#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "global.h"
#include <unistd.h>

static GtkWidget *label_stu_address;
static GtkWidget *label_upgrade;
static GtkWidget *label_port1;
static GtkWidget *label_port2;
static GtkWidget *entry_stu_address;
static GtkWidget *entry_upgrade;
static GtkWidget *entry_stu_port;
static GtkWidget *entry_upgrade_port;

static GtkWidget *label_terminal_name;
static GtkWidget *entry_terminal_name;

static GtkWidget *label_teacher_no;
static GtkWidget *entry_teacher_no;


static int stwidth = 0;
static int stheight = 0;

#define OVIRT_KEY_TELNAME   "user_name"
#define OVIRT_KEY_TEACHNO   "user_dpt_name"

extern int get_ovirt_conf(char *key, char *value);
extern int set_ovirt_conf(char * key, char *value);

struct StuServerInfo  stu_server_info = {"192.168.0.220", "192.168.0.220", 8090, 8000, "", ""};
static void save_ovirt_tcmstu()
{
	char szport[20] = {0};
	char szurl[MAX_BUFF_SIZE] = {0};
	memset(szurl, 0, MAX_BUFF_SIZE);
	strcpy(szurl, "http://");
	strcat(szurl, stu_server_info.upgrade_addr);
	strcat(szurl, ":");
	memset(szport, 0, 20);
	sprintf(szport, "%d", stu_server_info.upgrade_port);
	strcat(szurl, szport);
	char szteacher[MAX_BUFF_SIZE] = {0};
	strcpy(szteacher, "http://");
	strcat(szteacher, stu_server_info.stu_addr);
	strcat(szteacher, ":");
	memset(szport, 0, 20);
	sprintf(szport, "%d", stu_server_info.stu_port);
	strcat(szteacher, szport);
	printf("save_ovirt_tcm , tcm = %s , teacher = %s.\n", szurl, szteacher);
	set_ovirt_tcm(szurl, szteacher);
}


void set_stu_server_win_size(int width, int height)
{
	stwidth = width;
	stheight = height;
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
	gtk_css_provider_load_from_data (provider,"#entry_stu_address, #entry_stu_port,#entry_upgrade,#entry_upgrade_port,#entry_terminal_name,#entry_teacher_no{\n"
                                         "border-image: none;\n"
                                         "background-image: none;\n"
                                         "border-style: solid;\n"
                                         "border-width: 1px;\n"
                                         "color: #282C34;\n"
                                         "border-color: #282C34;\n"
                                         "border-radius: 0.2px;\n"
                                         "}\n", -1, NULL);
	//gtk_css_provider_load_from_path (provider, home, NULL);
	g_object_unref (provider);
}

static gboolean save_button_clicked(GtkButton *button,  gpointer user_data)
{
	strcpy(stu_server_info.stu_addr,  gtk_entry_get_text(entry_stu_address));
	strcpy(stu_server_info.upgrade_addr,  gtk_entry_get_text(entry_upgrade));
	strcpy(stu_server_info.terminal_name,  gtk_entry_get_text(entry_terminal_name));
	strcpy(stu_server_info.techer_no,  gtk_entry_get_text(entry_teacher_no));
	stu_server_info.stu_port = atoi(gtk_entry_get_text(entry_stu_port));
	stu_server_info.upgrade_port = atoi(gtk_entry_get_text(entry_upgrade_port));
	if (!check_ipv4_valid(stu_server_info.stu_addr))
	{
	   SYMsgDialog2(4, "IP 格式不正确,请重新输入！");
	   return;
	}
	if (!check_ipv4_valid(stu_server_info.upgrade_addr))
	{
	   SYMsgDialog2(4, "IP 格式不正确,请重新输入！");
	   return;
	}
	Save_StuServerInfo(stu_server_info);
	save_ovirt_tcmstu();
	set_ovirt_conf(OVIRT_KEY_TELNAME, stu_server_info.terminal_name);
	set_ovirt_conf(OVIRT_KEY_TEACHNO, stu_server_info.techer_no);
}

static void  on_entry_stu_address_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
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

static void  on_entry_upgrade_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
{
    if (new_text[0]  > '9'  || new_text [0] < '0')
    {
        if (new_text[0] == '.')
	   	   ;
	   else
       		new_text[0] = '\0';
   	}
    return;
}

static void  on_entry_stu_port_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
{
    if (new_text[0]  > '9'  || new_text [0] < '0')
    {
       	new_text[0] = '\0';
   	}
    return;
}

static void  on_entry_upgrade_port_insert_text(GtkWidget* entry, char* new_text, int new_text_length, int* position)
{
    if (new_text[0]  > '9'  || new_text [0] < '0')
    {
       	new_text[0] = '\0';
   	}
    return;
}


GtkWidget * stu_server_win()
{
	GtkWidget *layout = NULL;
	GtkWidget *button;
	layout = gtk_layout_new(NULL, NULL);
	gtk_widget_set_size_request(layout, stwidth, stheight);
	printf("stu_server_win:  width = %d,  height = %d .\n", stwidth, stheight);
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);

	int center_pos = stheight / 2;
	int label_height = 25;
	int label_width = 100;
	int entry_width = 80;
	int entry_height = 25;
	int interval = 10;

	label_stu_address = gtk_label_new("教室服务器地址");
	label_upgrade = gtk_label_new("升级服务器地址");

	label_port1 = gtk_label_new("端口");
	label_port2 = gtk_label_new("端口");

	label_terminal_name = gtk_label_new("终端名称");
	label_teacher_no = gtk_label_new("教室号");

	entry_stu_address = gtk_entry_new();
	entry_upgrade = gtk_entry_new();
	entry_stu_port = gtk_entry_new();
	entry_upgrade_port = gtk_entry_new();

	entry_terminal_name = gtk_entry_new();
	entry_teacher_no = gtk_entry_new();

	int nsize = 12;
	if (scr_width == 1024 && scr_height == 768)
	{
		nsize = 9;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200) )
	{
		nsize = 12;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080))
	{
		nsize = 10;
	}else if ((scr_width == 1280 && scr_height == 720) || (scr_width == 1280 && scr_height == 768) ||
	  (scr_width == 1280 && scr_height == 1024) )
	{
		nsize = 9;
	}else if ((scr_width == 1366 && scr_height == 768) || (scr_width == 1368 && scr_height == 768) ||
	  ((scr_width == 1360 && scr_height == 768)))
	{
		nsize = 9;
	}
	setctrlFont(GTK_WIDGET(label_stu_address), nsize);
	setctrlFont(GTK_WIDGET(label_upgrade), nsize);
	setctrlFont(GTK_WIDGET(label_port1), nsize);
	setctrlFont(GTK_WIDGET(label_port2), nsize);

	setctrlFont(GTK_WIDGET(label_terminal_name), nsize);
	setctrlFont(GTK_WIDGET(label_teacher_no), nsize);

	gtk_widget_set_size_request(GTK_WIDGET(label_stu_address), label_width, label_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_upgrade), label_width, label_height);
	gtk_widget_set_size_request(GTK_WIDGET(entry_stu_address), entry_width, entry_height);
	gtk_widget_set_size_request(GTK_WIDGET(entry_upgrade), entry_width, entry_height);
	gtk_widget_set_size_request(GTK_WIDGET(entry_stu_port), entry_width/2, entry_height);
	gtk_widget_set_size_request(GTK_WIDGET(entry_upgrade_port), entry_width/2, entry_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_port1), entry_width/2, entry_height);
	gtk_widget_set_size_request(GTK_WIDGET(label_port2), entry_width/2, entry_height);

	//
	gtk_widget_set_size_request(GTK_WIDGET(label_terminal_name), 60, 20);
	gtk_widget_set_size_request(GTK_WIDGET(label_teacher_no), 60, 20);

	gtk_entry_set_max_length(GTK_ENTRY(entry_stu_address), 15);
	gtk_entry_set_width_chars(GTK_ENTRY(entry_stu_address), 15);
	gtk_widget_set_name(GTK_WIDGET(entry_stu_address), "entry_stu_address");
	gtk_entry_set_max_length(GTK_ENTRY(entry_stu_port), 4);
	gtk_entry_set_width_chars(GTK_ENTRY(entry_stu_port), 4);
	gtk_widget_set_name(GTK_WIDGET(entry_stu_port), "entry_stu_port");
	//
	gtk_entry_set_max_length(GTK_ENTRY(entry_upgrade), 15);
	gtk_entry_set_width_chars(GTK_ENTRY(entry_upgrade), 15);
	gtk_widget_set_name(GTK_WIDGET(entry_upgrade), "entry_upgrade");
	gtk_entry_set_max_length(GTK_ENTRY(entry_upgrade_port), 4);
	gtk_entry_set_width_chars(GTK_ENTRY(entry_upgrade_port), 4);
	gtk_widget_set_name(GTK_WIDGET(entry_upgrade_port), "entry_upgrade_port");
	gtk_entry_set_input_purpose(entry_upgrade_port, GTK_INPUT_PURPOSE_NUMBER);
	gtk_entry_set_input_purpose(entry_stu_port, GTK_INPUT_PURPOSE_NUMBER);
	//
	gtk_entry_set_max_length(GTK_ENTRY(entry_terminal_name), 15);
	gtk_entry_set_width_chars(GTK_ENTRY(entry_terminal_name), 10);
	gtk_widget_set_name(GTK_WIDGET(entry_terminal_name), "entry_terminal_name");
	//
	gtk_entry_set_max_length(GTK_ENTRY(entry_teacher_no), 15);
	gtk_entry_set_width_chars(GTK_ENTRY(entry_teacher_no), 10);
	gtk_widget_set_name(GTK_WIDGET(entry_teacher_no), "entry_teacher_no");

    loadcss();
	int x, y = 0;

	x = interval;
	y = center_pos - interval - label_height;
	printf("label_stu_address :  x = %d,  y = %d .\n", x, y);
	gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(label_stu_address), x, y);
	x = interval + label_width + interval*2;
	printf("entry_stu_address :  x = %d,  y = %d .\n", x, y);
	gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(entry_stu_address), x, y);
	x = interval + label_width + interval*2 + entry_width*2 + interval;
	gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(label_port1), x, y);
	x = interval + label_width + interval*2 + entry_width*2 + entry_width/2 + interval;
	gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(entry_stu_port), x, y);

	x = interval;
	y = center_pos + interval * 2;
	gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(label_upgrade), x, y);
	x = interval + label_width + interval*2;
	gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(entry_upgrade), x, y);
	x = interval + label_width + interval*2 + entry_width*2 + interval;
	gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(label_port2), x, y);
	x = interval + label_width + interval*2 + entry_width*2 + entry_width/2 + interval;
	gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(entry_upgrade_port), x, y);


	//add 
	if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
	    x = interval + 20;
		y = center_pos - interval - label_height * 3;
		printf("label_stu_address :  x = %d,  y = %d .\n", x, y);
		gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(label_terminal_name), x, y);
		x = interval + label_width + interval*2;
		printf("entry_stu_address :  x = %d,  y = %d .\n", x, y);
		gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(entry_terminal_name), x, y);
		x = interval + label_width + interval*2 + entry_width*2 + interval - 30;
		gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(label_teacher_no), x, y);
		x = interval + label_width + interval*2 + entry_width*2 + entry_width/2 + interval;
		gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(entry_teacher_no), x, y);
	}else
	{
		int ter_stu_width = 60;
		x = interval;
		y = center_pos - interval - label_height * 3;
		printf("label_stu_address :  x = %d,  y = %d .\n", x, y);
		gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(label_terminal_name), x, y);
		x = interval + ter_stu_width + 10;
		printf("entry_stu_address :  x = %d,  y = %d .\n", x, y);
		gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(entry_terminal_name), x, y);
		x = interval + ter_stu_width + interval + entry_width*2/2 + interval*2;
		gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(label_teacher_no), x, y);
		x = interval + ter_stu_width + interval + entry_width*2/2 + ter_stu_width + interval*2;
		gtk_layout_put((GtkLayout *)layout, GTK_WIDGET(entry_teacher_no), x, y);
	}

	button = gtk_button_new_with_label("保存");
	gtk_widget_set_size_request(button, 60, 20);
	setctrlFont(button, 11);
	x = stwidth - 10 - 60;
	y = stheight - 40 - 20;
	gtk_layout_put(GTK_LAYOUT(layout), button, x, y);
	g_signal_connect(GTK_BUTTON(button), "clicked", G_CALLBACK(save_button_clicked), NULL);
	//event
	g_signal_connect(G_OBJECT(entry_stu_address), "insert-text", G_CALLBACK(on_entry_stu_address_insert_text), NULL);
	g_signal_connect(G_OBJECT(entry_upgrade), "insert-text", G_CALLBACK(on_entry_upgrade_insert_text), NULL);
	g_signal_connect(G_OBJECT(entry_stu_port), "insert-text", G_CALLBACK(on_entry_stu_port_insert_text), NULL);
	g_signal_connect(G_OBJECT(entry_upgrade_port), "insert-text", G_CALLBACK(on_entry_upgrade_port_insert_text), NULL);

	Get_StuServerInfo(&stu_server_info);
	get_ovirt_conf(OVIRT_KEY_TELNAME, stu_server_info.terminal_name);
	get_ovirt_conf(OVIRT_KEY_TEACHNO, stu_server_info.techer_no);
	gtk_entry_set_text(entry_stu_address, stu_server_info.stu_addr);
	gtk_entry_set_text(entry_upgrade, stu_server_info.upgrade_addr);
	gtk_entry_set_text(entry_terminal_name, stu_server_info.terminal_name);
	gtk_entry_set_text(entry_teacher_no, stu_server_info.techer_no);
	char szTmp[MAX_BUFF_SIZE] = {0};
	sprintf(szTmp, "%d", stu_server_info.stu_port);
	gtk_entry_set_text(entry_stu_port, szTmp);
	memset(szTmp, 0, MAX_BUFF_SIZE);
	sprintf(szTmp, "%d", stu_server_info.upgrade_port);
	gtk_entry_set_text(entry_upgrade_port, szTmp);
	gtk_widget_show(layout);
	return layout;
}
