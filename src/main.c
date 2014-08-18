#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>

#include <dirent.h>
#include <db.h>
#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gio/gio.h>
#include <gtk/gtk.h>

#include "compat.h"
#include "callbacks.h"
#include "dnd.h"
#include "extern.h"
#include "state.h"
#include "main.h"
#include "pathnames.h"
#include "window.h"

extern int errno;

static int		 populate(GtkListStore *, char*);
static void		 store_insert(GtkListStore *, struct dirent *, char *);
static GtkWidget	*prepare_window(char *, struct geometry *, struct state *);
__dead void	 	 usage();
static char		*getdir(int, char **);
static void		 getgeometry(char *dir, struct geometry *);

/*
 * A spatial file manager. Treat directories as independent resources with
 * fixed positions. Useful for navigating your filesystem with a predictable
 * GUI.
 */
int
main(int argc, char *argv[])
{
	struct geometry	*geometry;
	GtkWidget	*window;
        struct state	*d;
	char		*dir, ch;

	if ((d = state_new(argv[0])) == NULL)
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
	if (state_add_dir(d, dir) < 0)
		err(1, "state_add_dir");

	if ((geometry = malloc(sizeof(struct geometry))) == NULL)
		err(1, "malloc");
	getgeometry(dir, geometry);
	window = prepare_window(dir, geometry, d);
	free(geometry);

	gtk_widget_show(window);
	gtk_main();

	state_free(d);

	return 0;
}

/*
 * Show a usage message.
 */
void
usage()
{
	extern char	*__progname;
	fprintf(stderr, "usage: %s [dir]\n", __progname);

	exit(64);
}

/*
 * Pick the right starting directory. First try the name passed on the
 * command-line, then $HOME, and finally fall back to / .
 */
static char *
getdir(int argc, char *argv[])
{
	GFile		*g_dir;
	const char	*cwd;
	char		*dir;

	dir = NULL;

	if (argc > 0) {
		if ((cwd = getcwd(NULL, 0)) == NULL)
			warnx("could not get the current working directory");
		else {
			g_dir = g_file_new_for_commandline_arg_and_cwd(
			    argv[0], cwd);
			if ((dir = g_file_get_path(g_dir)) == NULL)
				warnx("no such directory: %s", argv[0]);

			g_object_unref(g_dir);
		}
	} else
		dir = getenv("HOME");

	if (dir == NULL)
		dir = "/";

	return dir;
}

/*
 * Look up the geometry for the directory, storing it in the `geometry'
 * pointer.
 */
static void
getgeometry(char *dir, struct geometry *geometry)
{
	struct geometry  g;
	DB		*db;
	DBT		 key, lookup;

	memset(&key, 0, sizeof(DBT));
	memset(&lookup, 0, sizeof(DBT));

	key.data = dir;
	key.size = strlen(dir)+1;

	if (db_create(&db, NULL, 0) < 0)
		err(1, "could not open the db");

	if (db->open(db, NULL, find_argonautinfo(), NULL,
		    DB_HASH, DB_CREATE, 0) < 0)
		err(1, "could not create the db");

	memset(&g, 0, sizeof(struct geometry));
	lookup.data = &g;
	lookup.ulen = sizeof(struct geometry);
	lookup.flags = DB_DBT_USERMEM;

	if (db->get(db, NULL, &key, &lookup, 0) == 0)
		memcpy(geometry, lookup.data, sizeof(struct geometry));
	else {
		geometry->x = -1;
		geometry->y = -1;
		geometry->h = DEFAULT_HEIGHT;
		geometry->w = DEFAULT_WIDTH;
	}

	if (db->close(db, 0) < 0)
		warn("could not close the db");
}

/*
 * Set up the window: build the interface, connect the signals, insert the
 * file icons.
 */
static GtkWidget *
prepare_window(char *dir, struct geometry *geometry, struct state *d)
{
	GtkBuilder	*builder;
	GtkWidget	*icons, *window, *directory_close;
	GtkListStore	*model;

	builder = gtk_builder_new_from_file(INTERFACE_PATH);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	icons = GTK_WIDGET(gtk_builder_get_object(builder, "icons"));
	directory_close = GTK_WIDGET(gtk_builder_get_object(builder, "directory-close-menu-item"));

	d->icon_view = GTK_ICON_VIEW(icons);

	g_object_unref(builder);

	gtk_window_set_default_size(GTK_WINDOW(window), geometry->w,
	    geometry->h);
	gtk_window_move(GTK_WINDOW(window), geometry->x, geometry->y);

	model = gtk_list_store_new(4, G_TYPE_STRING, GDK_TYPE_PIXBUF,
	    G_TYPE_STRING, G_TYPE_INT);
	if (populate(model, dir) == -1)
		err(66, "failed to populate icon model from %s", dir);

	gtk_icon_view_set_text_column(GTK_ICON_VIEW(icons), 0);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icons), 1);
	gtk_icon_view_set_model(GTK_ICON_VIEW(icons), GTK_TREE_MODEL(model));
	g_object_unref(model);
	gtk_drag_dest_set(icons, GTK_DEST_DEFAULT_ALL, NULL, 0,
	    GDK_ACTION_COPY);
	gtk_drag_dest_add_text_targets(icons);
	gtk_drag_dest_add_uri_targets(icons);

	g_signal_connect(icons, "drag-motion", G_CALLBACK(on_icons_drag_motion), d);
	g_signal_connect(icons, "drag-leave", G_CALLBACK(on_icons_data_leave), d);
	g_signal_connect(icons, "drag-data-received", G_CALLBACK(on_icons_drag_data_received), d);
	g_signal_connect(icons, "item-activated", G_CALLBACK(on_icons_item_activated), d);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(window, "configure-event", G_CALLBACK(on_window_configure_event), dir);
	g_signal_connect(directory_close, "activate", G_CALLBACK(gtk_main_quit), NULL);

	return window;
}

/*
 * Add each file in the directory into the model.
 */
static int
populate(GtkListStore *model, char *directory)
{
	DIR		*dirp;
	struct dirent	*dp;

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
static void
store_insert(GtkListStore *model, struct dirent *dp, char *directory)
{
	GdkPixbuf	*dir_pixbuf, *file_pixbuf;
	GtkIconTheme	*icon_theme;
	GtkTreeIter	 iter;

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
