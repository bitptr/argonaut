/* vim: set sw=0 ts=8 cinoptions=:0,t0,+4,(4: */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>

#include <dirent.h>
#include <err.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "compat.h"
#include "extern.h"
#include "pathnames.h"

#define DEFAULT_HEIGHT 350
#define DEFAULT_WIDTH 700

extern int errno;

int		 populate(GtkListStore *, char*);
void		 store_insert(GtkListStore *, struct dirent *, char *);
GtkWidget	*prepare_window(struct cb_data *, char *);
__dead void	 usage();
char		*getdir(int, char **);

/*
 * A spatial file manager. Treat directories as independent resources with
 * fixed positions. Useful for navigating your filesystem with a predictable
 * GUI.
 */
int
main(int argc, char *argv[])
{
	GtkWidget *window;
	char *dir, ch;
        struct cb_data *d;

	if ((d = cb_data_new(argv[0])) == NULL)
		err(1, "could not build the callback data");

	gtk_init(&argc, &argv);

	while ((ch = getopt(argc, argv, "")) != -1)
		switch (ch) {
		default:
			usage();
			/* NOTREACHED */
		}
	argc -= optind;
	argv += optind;

	dir = getdir(argc, argv);

	window = prepare_window(d, dir);

	gtk_widget_show(window);
	gtk_main();

	cb_data_free(d);

	return 0;
}

/*
 * Show a usage message.
 */
void
usage()
{
	extern char *__progname;
	fprintf(stderr, "usage: %s [dir]\n", __progname);

	exit(64);
}

/*
 * Pick the right starting directory. First try the name passed on the
 * command-line, then $HOME, and finally fall back to / .
 */
char *
getdir(int argc, char *argv[])
{
	char *dir;

	if (argc > 0)
		dir = argv[0];
	else
		dir = getenv("HOME");

	if (dir == NULL)
		dir = "/";

	return dir;
}

/*
 * Set up the window: build the interface, connect the signals, insert the
 * file icons.
 */
GtkWidget *
prepare_window(struct cb_data *d, char *dir)
{
	GtkBuilder *builder;
	GtkWidget *icons, *window;
	GtkListStore *model;

	builder = gtk_builder_new_from_file(INTERFACE_PATH);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "directory-window"));
	icons = GTK_WIDGET(gtk_builder_get_object(builder, "file-icons"));

	gtk_builder_connect_signals(builder, d);
	g_object_unref(builder);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_default_size(GTK_WINDOW(window), DEFAULT_WIDTH,
	    DEFAULT_HEIGHT);

	model = gtk_list_store_new(4, G_TYPE_STRING, GDK_TYPE_PIXBUF,
	    G_TYPE_STRING, G_TYPE_INT);
	if (populate(model, dir) == -1)
		err(66, "failed to populate icon model from %s", dir);

	gtk_icon_view_set_text_column(GTK_ICON_VIEW(icons), 0);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icons), 1);
	gtk_icon_view_set_model(GTK_ICON_VIEW(icons), GTK_TREE_MODEL(model));
	g_object_unref(model);

	return window;
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
	    FILE_NAME, dp->d_name,
	    FILE_ICON, GDK_PIXBUF(dp->d_type == DT_DIR ? dir_pixbuf : file_pixbuf),
	    FILE_PARENT, directory,
	    FILE_TYPE, dp->d_type,
	    -1);
}
