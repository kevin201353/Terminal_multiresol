#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "global.h"
#include <unistd.h>

static int stwidth = 0;
static int stheight = 0;
extern void setctrlFont(GtkWidget * widget, int nsize);
static GtkWidget *hscale = NULL;
static GtkWidget *hscale_align = NULL;


struct SoundInfo  soundInfo = {0};

void set_soundwin_size(int width, int height)
{
	stwidth = width;
	stheight = height;
}

int get_sound_value()
{
    int nsound = 0;
    if (hscale != NULL)
    	{
		nsound = gtk_range_get_value(hscale);
		printf("SoundWind soundInfo.volume 000 =%d .\n", nsound);
    	}
	return nsound;
}


int set_sound_value(int value)
{
	if (hscale != NULL)
		gtk_range_set_value(hscale, value);
}

static gboolean save_button_clicked(GtkButton *button,  gpointer user_data)
{
    soundInfo.volume = get_sound_value();
	printf("SoundWind soundInfo.volume =%d .\n", soundInfo.volume);
	Save_Sound(soundInfo);
}

GtkWidget * SYSoundWin()
{
  GtkWidget *layout;
  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;
  GtkWidget *label_sound;
  GtkWidget *button;
  layout = gtk_layout_new(NULL, NULL);
  gtk_widget_set_size_request(layout, stwidth, stheight);
  printf("SYSoundWin:  width = %d,  height = %d .\n", stwidth, stheight);
  hscale = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
  gtk_widget_set_size_request(hscale, stwidth - 150, 40);
  hscale_align = gtk_alignment_new(0.50, 0.50, 1.00, 1.00);
  gtk_alignment_set_padding (GTK_ALIGNMENT(hscale_align),0,10,20,0);
  gtk_layout_put(GTK_LAYOUT(layout), hscale_align, 80, 20);
  gtk_container_add (GTK_CONTAINER(hscale_align), hscale);

  //gtk_container_add (GTK_CONTAINER(layout), hscale_align);
  gtk_scale_set_digits(GTK_SCALE(hscale), 0);
  gtk_scale_set_value_pos(GTK_SCALE(hscale), GTK_POS_TOP);
  label_sound = gtk_label_new("音量");
  gtk_widget_set_size_request(label_sound, 60, 35);
  gtk_layout_put(GTK_LAYOUT(layout), label_sound, 10, 54);
  setctrlFont(label_sound, 11);

  button = gtk_button_new_with_label("保存");
  gtk_widget_set_size_request(button, 60, 20);
  setctrlFont(button, 11);
  gtk_scale_set_digits(hscale, 0);

  Get_Sound(&soundInfo);
  set_sound_value(soundInfo.volume);

  int x, y = 0;
  x = stwidth - 10 - 60;
  y = stheight - 40 - 20;
  gtk_layout_put(GTK_LAYOUT(layout), button, x, y);
  g_signal_connect(GTK_BUTTON(button), "clicked", G_CALLBACK(save_button_clicked), NULL);
  provider = gtk_css_provider_new ();
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gsize bytes_written, bytes_read;
  const gchar* home = "./scale.css";
  GError *error = 0;
  gtk_css_provider_load_from_path (provider,
                                  g_filename_to_utf8(home, strlen(home), &bytes_read, &bytes_written, &error),
                                  NULL);
  g_object_unref(provider);
  gtk_widget_show(layout);
  return layout;
}
