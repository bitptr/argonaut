#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

void on_directory_close(GtkMenuItem *, gpointer);

void
on_directory_close (GtkMenuItem *menuitem, gpointer user_data) {
	gtk_main_quit();
}
