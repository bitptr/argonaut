/* vim: set sw=0 ts=8 cinoptions=:0,t0,+4,(4: */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <err.h>

#include <gtk/gtk.h>

#include "compat.h"
#include "extern.h"

void	on_directory_close(GtkMenuItem *, gpointer);
void	on_item_activated(GtkIconView *, GtkTreePath *, gpointer);

/*
 * Closing the directory is the same as quitting the program.
 */
void
on_directory_close(GtkMenuItem *menuitem, gpointer user_data)
{
	gtk_main_quit();
}

/*
 * Open the file using guesses from XDG.
 */
void
on_item_activated(GtkIconView *iconview, GtkTreePath *path, gpointer user_data)
{
	GdkScreen *screen;
	GtkTreeIter iter;
	GtkTreeModel *model;
	gchar *directory, *fullpath, *name;
	GError *error = NULL;

	if ((model = gtk_icon_view_get_model(iconview)) == NULL)
		errx(66, "could not find the model for the icon view");

	gtk_tree_model_get_iter(
	    GTK_TREE_MODEL(model),
	    &iter,
	    path);

	gtk_tree_model_get(
	    GTK_TREE_MODEL(model),
	    &iter,
	    FILE_NAME, &name,
	    FILE_PARENT, &directory,
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
