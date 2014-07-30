#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>

#include "global.h"

void on_directory_close(GtkMenuItem *, gpointer);
void on_item_activated(GtkIconView *, GtkTreePath *, gpointer);

void
on_directory_close (GtkMenuItem *menuitem, gpointer user_data) {
	gtk_main_quit();
}

void
on_item_activated(GtkIconView *iconview, GtkTreePath *path, gpointer user_data) {
	GtkTreeIter iter;
	gchar *name;
	gchar *directory;
	gchar *fullpath;
	GdkScreen *screen;
	GError *error = NULL;

	gtk_tree_model_get_iter(
	    GTK_TREE_MODEL(model),
	    &iter,
	    path);

	gtk_tree_model_get(
	    GTK_TREE_MODEL(model),
	    &iter,
	    0, &name,
	    2, &directory,
	    -1);

	fullpath = g_strdup_printf("file://%s/%s", directory, name);
	screen = gdk_screen_get_default();
	gtk_show_uri(screen,
	   fullpath,
	   GDK_CURRENT_TIME,
	   &error);

	if (error)
	  g_print("%s\n", error->message);

	g_free(name);
	g_free(directory);
	g_free(fullpath);
}
