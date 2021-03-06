#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "buildtime.h"
#include <string.h>

#define MAIN_VERSION "ThinView_"
#define SUBVERSION_NUMBER "9.3.1."
GtkLayout *g_aboutlayout;
int about_win_width = 0;
int about_win_height = 0;
extern void setctrlFont(GtkWidget * widget, int nsize);

/************************************************************************/
/* ɾ���ַ�����ָ���ַ� */
/************************************************************************/
void del_char(char* str,char ch)
{
	char *p = str;
	char *q = str;
	while(*q)
	{
		if (*q !=ch)
		{
			*p++ = *q;
		}
		q++;
	}
	*p='\0';
}

GtkLayout * GetAboutLayout()
{
     return g_aboutlayout;
}

void setAbout_win_Size(int width, int height)
{
	about_win_width = width;
	about_win_height = height;
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
	gtk_css_provider_load_from_data (provider,"#label_about {\n"
                                         "background-color: transparent;\n"
                                         "background-image: none;\n"
                                         "color: #282C34;\n"
                                         "}\n", -1, NULL);
	//gtk_css_provider_load_from_path (provider, home, NULL);
	g_object_unref (provider);
}

void SY_AboutWindow()
{
	GtkBuilder *builder = gtk_builder_new();
	GError *error = NULL;
	gtk_builder_add_from_file(builder, "aboutwindow.glade", &error);
	loadcss();
	int win_width = 0;
	int win_height = 0;
	int font_size = 0;
	GdkScreen* screen;
	screen = gdk_screen_get_default();
	int scr_width = gdk_screen_get_width(screen);
	int scr_height = gdk_screen_get_height(screen);
	//set font size
	int nsize = 12;
	if (scr_width == 1024 && scr_height == 768)
	{
		nsize = 9;
	}else if ((scr_width == 1920 && scr_height == 1080) || (scr_width == 1920 && scr_height == 1200))
	{
		nsize = 12;
	}else if ((scr_width == 1440 && scr_height == 900) || (scr_width == 1600 && scr_height == 900) ||  (scr_width == 1600 && scr_height == 896 ) || (scr_width == 1600 && scr_height == 1080) || (scr_width == 1680 && scr_height == 1050))
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
	GObject * layout1 = gtk_builder_get_object(builder, "layout1");
	g_aboutlayout = (GtkLayout *)layout1;
	int space_width = 10;
	int space_height = 20;
	int width_label = about_win_width - space_width*2;
	int height_label = 80;
	LogInfo("SY_AboutWindow, about_win_width =%d, about_win_height =%d.\n", about_win_width, about_win_height);
	gtk_widget_set_size_request(GTK_WIDGET(layout1), about_win_width, about_win_height);
	GObject * label_about = gtk_builder_get_object(builder, "label_about");
	gtk_widget_set_size_request(GTK_WIDGET(label_about), width_label, height_label);
	setctrlFont(GTK_WIDGET(label_about), nsize);
	int x, y = 0;
	x = space_width;
	y = (about_win_height - height_label)/2;
	gtk_layout_move((GtkLayout *)layout1, GTK_WIDGET(label_about), x, y);
	char sztext[MAX_BUFF_SIZE] = {0};
	char szTmp[MAX_BUFF_SIZE] = {0};
	strcpy(szTmp, MAIN_VERSION);
	strcat(szTmp, SUBVERSION_NUMBER);
	char szDest[20] = {0};
	strtok(buildtime, " ");
	del_char(buildtime, '-');//20161014
	strcpy(szDest, buildtime+2);
	strcat(szTmp, szDest);
	sprintf(sztext, "终端登录软件 \n版本 ：%s", szTmp);
	gtk_label_set_text(GTK_LABEL(label_about), sztext);
}
