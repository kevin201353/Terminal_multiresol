#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include <unistd.h>
#include "cJSON.h"

static const gchar*  g_symsgcss = "systhrdModifymg.css";
static GtkBuilder *builder;
static GObject  *window = NULL;
GtkLabel *labeltext;

extern void setctrlFont(GtkWidget * widget, int nsize);
extern void my_container_resize(GtkContainer* container, float factorX, float factorY);
extern void my_resize(GtkContainer* container, GtkWidget *widget, float factorX, float factorY);
extern void my_resizemain(GtkWidget *widget, float factorX, float factorY);

static  char g_url2[512] = {0};
static struct ServerInfo  serverInfo;
extern int Ovirt_Login_Pass(char *url, char *user, char* password, char* ret);


static int initUrl()
{
	if ( GetServerInfo2(&serverInfo) < 0 )
	{
	 	return -1;
	}
	char sztmp[20] = {0};
	sprintf(sztmp, "%d", serverInfo.nport);
	strcpy(g_url2, "http://");
	strcat(g_url2, serverInfo.szIP);
	strcat(g_url2, "/");
	return 0;
}


static void calpass()
{
	char szPass[MAX_BUFF_SIZE] = {0};
	char szEntry_pass[MAX_BUFF_SIZE] = {0};
	Get_ConfigPass(szPass);
	GObject *entry_pass = gtk_builder_get_object(builder, "Entry_pass");
	char * szContent = gtk_entry_get_text((GtkEntry *)entry_pass);
	printf("OK_button_clicked   %s.\n", szContent);
    if (strcmp(szContent, "") == 0)
    {
    	    gtk_widget_hide((GtkWidget *)window);
        SYMsgDialog2(13, "密码不能为空！");
		gtk_widget_show((GtkWidget *)window);
    }
	if (szContent != NULL && strlen(szContent) > 0)
	{
		sy_md5_encryption(szContent, szEntry_pass);
		if (strcmp(szPass, szEntry_pass) == 0)
		{
			g_showSetting = 1;
			gtk_widget_destroy((GtkWidget *)window);
			gtk_main_quit();
		}else
		{
			g_showSetting = 0;
			gtk_widget_hide((GtkWidget *)window);
		    SYMsgDialog2(13, "密码不正确！");
			gtk_widget_show((GtkWidget *)window);
		}
	}
}


void save_pass()
{
	GObject *entry_modifyuser = gtk_builder_get_object(builder, "entry_modifyuser");
	GObject *entry_oldpass = gtk_builder_get_object(builder, "entry_oldpass");
	GObject *entry_newpass = gtk_builder_get_object(builder, "entry_newpass");
	GObject *entry_okpass = gtk_builder_get_object(builder, "entry_okpass");
	char * oldpass = (char *)gtk_entry_get_text((GtkEntry *)entry_oldpass);
	char * newpass = (char *)gtk_entry_get_text((GtkEntry *)entry_newpass);
	char * okpass = (char *)gtk_entry_get_text((GtkEntry *)entry_okpass);
	char * user = (char *)gtk_entry_get_text((GtkEntry *)entry_modifyuser);
	if (strcmp(oldpass, "") == 0)
	{
		 SYMsgDialog2(23, "密码不正确！");
		 return;
	}
	if (strcmp(user, "") == 0)
	{
		 SYMsgDialog2(23, "用户名为空！");
		 return;
	}
	if (strcmp(newpass, okpass) != 0)
	{
		 SYMsgDialog2(23, "两次输入的密码不一致！");
		 return;
	}
	if (strcmp(newpass, "") == 0)
	{
		 SYMsgDialog2(23, "新密码为空！");
		 return;
	}
	if (strcmp(okpass, "") == 0)
	{
		 SYMsgDialog2(23, "确认密码为空！");
		 return;
	}
	//login
	char ret[10240] = {0};
	char val[1024] = {0};
	char szDebug[10240] = {0};
	Ovirt_Login_Pass(g_url2, user, oldpass, ret);
	sprintf(szDebug, "Ovirt_Login_Pass, return respose data = %s.", ret);
	LogInfo(szDebug);
	parse_json(ret, "success", val, NULL);
	if (strcmp(val, "true") != 0)
	{
		SYMsgDialog2(23, "用户名或密码错误！");
		return;
	}
	//modifypass
	memset(val, 0, sizeof(val));
	parse_json(ret, "data", val, "token");
	sprintf(szDebug, "Ovirt_Login_Pass, return respose token data = %s.", val);
	LogInfo(szDebug);
	Ovirt_ModifyPass(g_url2, user, oldpass, newpass, ret, val);
	parse_json(ret, "success", val, NULL);
	if (strcmp(val, "true") == 0)
	{
	     LogInfo("save_pass, xxxxx,  exit window!");
		gtk_widget_destroy((GtkWidget *)window);
		gtk_main_quit();
	}else
	{
		parse_json(ret, "data", val, "message");
		SYMsgDialog2(23, val);
	}
}

void parse_json(char *data, char *key, char *val, char* arrkey)
{
	cJSON *json , *json_data;
	char szDebug[10240] = {0};
    // �������ݰ�
    LogInfo("parse_json, data = :%s.", data);
    json = cJSON_Parse(data);
    if (!json)
    {
		sprintf(szDebug, "Error before: [%s]\n", cJSON_GetErrorPtr());
		LogInfo(szDebug);
    }else
    	{
		// ��������ֵ
		json_data = cJSON_GetObjectItem(json, key);
		if (json_data->type == cJSON_False)
		{
		 	sprintf(szDebug, "parse_json Number, key=%s, val=%d", key, json_data->valueint);
	    		LogInfo(szDebug);
			strcpy(val, "false");
		}
		if (json_data->type == cJSON_True)
		{
			sprintf(szDebug, "parse_json Number 22, key=%s, val=%d", key, json_data->valueint);
	    		LogInfo(szDebug);
			strcpy(val, "true");
		}
		if( json_data->type == cJSON_String )
		{
		    // ��valueint�л��ý���
		    sprintf(szDebug, "parse_json, key=%s, val=%s", key, json_data->valuestring);
		    LogInfo(szDebug);
			strcpy(val, json_data->valuestring);
		}
		if (json_data->type == cJSON_Array)
		{
		     LogInfo("parse_json, data is array.");
			cJSON* pArrayItem = NULL;
			int nCount = cJSON_GetArraySize(json_data);
			sprintf(szDebug, "parse_json, data array count = %d.", nCount);
		    LogInfo(szDebug);
			for (int i=0; i<nCount; i++)
			{
				pArrayItem = cJSON_GetArrayItem(json_data, i);
				cJSON*  json_data = cJSON_GetObjectItem(pArrayItem, arrkey);
				if (json_data != NULL && json_data->type == cJSON_String)
				{
					//parse_json(json_data->valuestring, arrkey, val, NULL);
					sprintf(szDebug, "parse_json, message xxxx, key=%s, val=%s", arrkey, json_data->valuestring);
					LogInfo(szDebug);
					strcpy(val, json_data->valuestring);
					break;
				}
			}
		}
		if (json_data->type == cJSON_Object)
		{
			LogInfo("parse_json, data is object.");
			cJSON*  json_data1 = cJSON_GetObjectItem(json_data, arrkey);
			if (json_data1 != NULL && json_data1->type == cJSON_String)
			{
				sprintf(szDebug, "parse_json, message xxxx, key=%s, val=%s", arrkey, json_data1->valuestring);
				LogInfo(szDebug);
				strcpy(val, json_data1->valuestring);
			}
		}
		// �ͷ��ڴ��ռ�
		cJSON_Delete(json);
	}
}

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    switch(event->keyval)
    {
		case GDK_KEY_Return:
			{
				//calpass();
				//LogInfo("Debug zhaosenhua setting pass input dialog , gdk_key_return.\n");
			}
			break;
    }
    return FALSE;
}


static gboolean close_button_clicked(GtkButton *button,  gpointer user_data)
{
    if (window != NULL)
    {
         g_showSetting = -1;
	    gtk_widget_destroy((GtkWidget *)window);
		window = NULL;
	    gtk_main_quit();
    	}
}

static gboolean OK_button_clicked(GtkButton *button,  gpointer user_data)
{
    //calpass();
    save_pass();
}

static gboolean Cancel_button_clicked(GtkButton *button,  gpointer user_data)
{
    if (window != NULL)
    {
    		g_showSetting = -1;
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

void setctrlFontxx(GtkWidget * widget, int nsize, float x)
{
	PangoFontDescription *font1 = pango_font_description_from_string("Microsoft YaHei");
	pango_font_description_set_size(font1, nsize * PANGO_SCALE * x);
	gtk_widget_override_font(widget, font1);
}


static float g_scr_old_width = 1920.0;
static float g_scr_old_height = 1080.0;

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


	GObject *label_user = gtk_builder_get_object(builder, "label_user");
	GObject *entry_modifyuser = gtk_builder_get_object(builder, "entry_modifyuser");

	struct LoginInfo info = {"", "", "", "", 3389, 0, 0, 0};
	GetLoginInfo(&info);
	gtk_entry_set_text(GTK_ENTRY(entry_modifyuser), info.user);
	///
	GObject *label_oldpass = gtk_builder_get_object(builder, "label_oldpass");
	GObject *entry_oldpass = gtk_builder_get_object(builder, "entry_oldpass");
	GObject *label_newpass = gtk_builder_get_object(builder, "label_newpass");
	GObject *entry_newpass = gtk_builder_get_object(builder, "entry_newpass");
	GObject *label_okpass = gtk_builder_get_object(builder, "label_okpass");
	GObject *entry_okpass = gtk_builder_get_object(builder, "entry_okpass");

	///
	GObject *label_rul1 = gtk_builder_get_object(builder, "label_rul1");
	GObject *label_rul2 = gtk_builder_get_object(builder, "label_rul2");
	gtk_label_set_text((GtkWidget *)label_rul1, "任意字符，最大20位");
	gtk_label_set_text((GtkWidget *)label_rul2, "任意字符，最大20位");

	GObject *image_sylogo = gtk_builder_get_object(builder, "image_sylogo");
	GObject *btnsymsg_close = gtk_builder_get_object(builder, "btnsymsg_close");


	float factor_x = (float)scr_width/g_scr_old_width;
	float factor_y = (float)scr_height/g_scr_old_height;

       

        //my_resizemain((GtkWidget *)window, factor_x, factor_y);
       
        
	/*
	my_resize((GtkContainer *)layout_thrdtitle, (GtkWidget *)image_sylogo, factor_x, factor_y);
	my_resize((GtkContainer *)layout_thrdtitle, (GtkWidget *)btnsymsg_close, factor_x, factor_y);
	*/
       
	/* 
	my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)label_user, factor_x, factor_y);
	my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)entry_modifyuser, factor_x, factor_y);
	my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)label_oldpass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)entry_oldpass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)label_newpass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)entry_newpass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)label_okpass, factor_x, factor_y);
	my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)entry_okpass, factor_x, factor_y);
	*/

	//my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)label_rul1, factor_x, factor_y);
	//my_resize((GtkContainer *)layout_thrdtext, (GtkWidget *)label_rul2, factor_x, factor_y);

	//my_resizemain((GtkWidget *)btn_OK, factor_x, factor_y);
	//my_resizemain((GtkWidget *)btn_Cancel, factor_x, factor_y);
        

#if 1
	my_resizemain((GtkWidget *)window, factor_x, factor_y);
#endif

	int nsize = 10;
	if (scr_width == 1024 && scr_height == 768)
		nsize = 12;
	else if (scr_width == 1440 && scr_height == 900 ||
		scr_width == 1600 && scr_height == 900)
		nsize = 12;
	else if (scr_width == 1280 && scr_height == 1024)
	{
		nsize = 12;
	}
	setctrlFontxx(GTK_WIDGET(label_user), nsize, factor_x);
	setctrlFontxx(GTK_WIDGET(label_oldpass), nsize, factor_x);
	setctrlFontxx(GTK_WIDGET(label_newpass), nsize, factor_x);
	setctrlFontxx(GTK_WIDGET(label_okpass), nsize, factor_x);


	setctrlFontxx(GTK_WIDGET(label_rul1), nsize, factor_x);
	setctrlFontxx(GTK_WIDGET(label_rul2), nsize, factor_x);
	setctrlFontxx(GTK_WIDGET(btn_OK), nsize, factor_x);

	/*
	entry_user = gtk_builder_get_object(builder, "btn_OK");
	entry_oldpass = gtk_builder_get_object(builder, "btn_Cancel");
	entry_newpass = gtk_builder_get_object(builder, "label_thrdtext");
	entry_okpass = gtk_builder_get_object(builder, "Entry_pass");

	int btn_OK_width = 0;
	int btn_OK_height = 0;
	int btn_Cancel_width = 0;
	int btn_Cancel_height = 0;
	int label_text_width = 0;
	int label_text_height = 0;
	int layout_thrdtitle_height = 0;
	int layout_thrdtext_height = 0;
	int entrypass_width = 0;
	int entrypass_height = 0;
	int nsize = 0;
	if (scr_width == 1024 && scr_height == 768)
	{
		win_width = 250;
		win_height = 90;
		btn_OK_width = 30;
		btn_OK_height = 5;
		btn_Cancel_width = 30;
		btn_Cancel_height = 5;
		label_text_width = 45;
		label_text_height = 30;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 90;
		entrypass_width = 80;
		entrypass_height = 30;
		nsize = 8;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		win_width = 470;
		win_height = 470; //160;
		btn_Cancel_width = 60;
		btn_Cancel_height = 20;
		btn_OK_width = 60;
		btn_OK_height = 20;
		label_text_width = 100;
		label_text_height = 30;
		layout_thrdtitle_height = 30;
		layout_thrdtext_height = 200;
		entrypass_width = 100;
		entrypass_height = 30;
		nsize = 12;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   ||
	  (scr_width == 1600 && scr_height == 1080) )
	{
		win_width = 300;
		win_height = 100;
		btn_OK_width = 30;
		btn_OK_height = 5;
		btn_Cancel_width = 30;
		btn_Cancel_height = 5;
		label_text_width = 60;
		label_text_height = 30;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 90;
		entrypass_width = 90;
		entrypass_height = 30;
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
		label_text_width = 60;
		label_text_height = 30;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 90;
		entrypass_width = 100;
		entrypass_height = 30;
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
		label_text_width = 60;
		label_text_height = 30;
		layout_thrdtitle_height = 20;
		layout_thrdtext_height = 90;
		entrypass_width = 90;
		entrypass_height = 30;
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
	gtk_widget_set_size_request(GTK_WIDGET(entry_pass), entrypass_width, entrypass_height);
	setctrlFont(GTK_WIDGET(label_thrdtext), nsize);
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
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080))
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
	x = 20; //win_width - label_text_width - entrypass_width;
	y = 20;
	gtk_layout_move((GtkLayout *)layout_thrdtext, GTK_WIDGET(label_thrdtext), x, y);
	gtk_layout_move((GtkLayout *)layout_thrdtext, GTK_WIDGET(entry_pass), x +label_text_width+10, y);
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
	gtk_widget_grab_focus(GTK_WIDGET(entry_pass));
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ALWAYS);
	*/
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
		(scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 )   || (scr_width == 1600 && scr_height == 1080))
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

void SYPassModifyDialog()
{
    builder = gtk_builder_new();
    GError *error1 = NULL;
    gtk_builder_add_from_file (builder, "SYPasMofidyDlg.glade", &error1);
    window = gtk_builder_get_object(builder, "window_Pass");
    GObject * label_thrdtext;
    label_thrdtext = gtk_builder_get_object(builder, "label_thrdtext");
    labeltext = (GtkLabel *)label_thrdtext;
	loadcss();
	init_ctrl_size(builder);
	init_ctrl_image(builder);
	initUrl();
    GObject * btnsymsg_close;
    btnsymsg_close = gtk_builder_get_object(builder, "btnsymsg_close");
    g_signal_connect(GTK_BUTTON(btnsymsg_close), "clicked", G_CALLBACK(close_button_clicked), NULL);
    g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(gtk_main_quit), NULL);
    //ok-cancel btn
    GObject * btn_OK;
    GObject * btn_Cancel;
    btn_OK = gtk_builder_get_object(builder, "btn_OK");
    btn_Cancel = gtk_builder_get_object(builder, "btn_Cancel");
	gtk_button_set_label(GTK_BUTTON(btn_OK), "保存");
	gtk_button_set_label(GTK_BUTTON(btn_Cancel), "取消");
	gtk_widget_set_visible(GTK_WIDGET(btn_OK), TRUE);
	gtk_widget_set_sensitive(GTK_WIDGET(btn_OK), TRUE);
	gtk_widget_set_visible(GTK_WIDGET(btn_Cancel), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(btn_Cancel), FALSE);
	g_signal_connect(GTK_BUTTON(btn_OK), "clicked", G_CALLBACK(OK_button_clicked), NULL);
	g_signal_connect(GTK_BUTTON(btn_Cancel), "clicked", G_CALLBACK(Cancel_button_clicked), NULL);
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(on_key_press), NULL);
	//gtk_window_set_modal (GTK_WINDOW (window), TRUE);
	 gtk_window_set_transient_for(GTK_WINDOW(window), g_mainWindow);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    gtk_main();
    g_object_unref (G_OBJECT(builder));
}

//int main(int argc, char *argv[])
//{
//	gtk_init (&argc, &argv);
//	SYPassDialog();
//}
