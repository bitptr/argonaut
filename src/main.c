#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>

#include <dirent.h>
#include <err.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "pathnames.h"

#define DEFAULT_HEIGHT 350
#define DEFAULT_WIDTH 700

extern int errno;

int	populate(GtkListStore *, char*);
void	store_insert(GtkListStore *, struct dirent *, char *);

/*
 * A spatial file manager. Treat directories as independent resources with
 * fixed positions. Useful for navigating your filesystem with a predictable
 * GUI.
 */
int
main(int argc, char *argv[])
{
	GtkBuilder *builder;
	GtkWidget *icons, *window;
	GtkListStore *model;
	char *dir;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new_from_file(INTERFACE_PATH);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "directory-window"));
	icons = GTK_WIDGET(gtk_builder_get_object(builder, "file-icons"));

	gtk_builder_connect_signals(builder, NULL);
	g_object_unref(builder);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_default_size(GTK_WINDOW(window), DEFAULT_WIDTH, DEFAULT_HEIGHT);

	if ((dir = getenv("HOME")) == NULL)
		dir = "/";

	model = gtk_list_store_new(3, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING);
	if (populate(model, dir) == -1)
		err(66, "failed to populate icon model from %s", dir);

	gtk_icon_view_set_text_column(GTK_ICON_VIEW(icons), 0);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icons), 1);
	gtk_icon_view_set_model(GTK_ICON_VIEW(icons), GTK_TREE_MODEL(model));
	g_object_unref(model);

	gtk_widget_show(window);

	gtk_main ();

	return 0;
}

/*
 * Add each file in the directory into the model.
 */
int
populate(GtkListStore *model, char *directory)
{
	DIR *dirp;
	struct dirent *dp;

	dirp = opendir(directory);
	if (dirp) {
		while ((dp = readdir(dirp)) != NULL)
			if (dp->d_name[0] != '.')
				store_insert(model, dp, directory);
		return closedir(dirp);
	}

	return -1;
}

/*
 * Add to the model the file name, directory name, and an icon.
 */
void
store_insert(GtkListStore *model, struct dirent *dp, char *directory)
{
	GdkPixbuf *dir_pixbuf, *file_pixbuf;
	GtkIconTheme *icon_theme;
	GtkTreeIter iter;

	icon_theme = gtk_icon_theme_get_default();
	file_pixbuf = gtk_icon_theme_load_icon(
	    icon_theme,
	    "text-x-generic", /* icon name */
	    32, /* icon size */
	    0,  /* flags */
	    NULL);
	if (!file_pixbuf)
		errx(66, "could not load the file pixbuf");

	dir_pixbuf = gtk_icon_theme_load_icon(
	    icon_theme,
	    "folder",
	    32,
	    0,
	    NULL);
	if (!dir_pixbuf)
		errx(66, "could not load the directory pixbuf");

	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
	    0, dp->d_name,
	    1, GDK_PIXBUF(dp->d_type == DT_DIR ? dir_pixbuf : file_pixbuf),
	    2, directory,
	    -1);
}
