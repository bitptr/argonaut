#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <db.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gio/gio.h>
#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "compat.h"
#include "callbacks.h"
#include "dnd.h"
#include "drag.h"
#include "drop.h"
#include "extern.h"
#include "file.h"
#include "main.h"
#include "pathnames.h"
#include "window.h"
#include "watcher.h"
#include "state.h"
#include "store.h"
#include "thumbnail.h"

extern int errno;

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
	WnckWindow	*w;
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

	if (((w = window_by_title(dir)) != NULL) && window_activate(w) != -1)
		goto done;

	if (state_add_dir(d, dir) < 0)
		err(1, "state_add_dir");

	if ((geometry = malloc(sizeof(struct geometry))) == NULL)
		err(1, "malloc");
	getgeometry(dir, geometry);
	window = prepare_window(dir, geometry, d);
	free(geometry);

	gtk_widget_show(window);
	gtk_main();

done:
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
	GtkWidget	*icons, *window, *directory_close, *file_open;
	GtkWidget	*directory_up;
	GtkListStore	*model;

	builder = gtk_builder_new_from_file(INTERFACE_PATH);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	icons = GTK_WIDGET(gtk_builder_get_object(builder, "icons"));
	directory_up = GTK_WIDGET(gtk_builder_get_object(builder, "directory-up-menu-item"));
	directory_close = GTK_WIDGET(gtk_builder_get_object(builder, "directory-close-menu-item"));
	file_open = GTK_WIDGET(gtk_builder_get_object(builder, "file-open-menu-item"));

	d->icon_view = GTK_ICON_VIEW(icons);

	g_object_unref(builder);

	gtk_window_set_default_size(GTK_WINDOW(window), geometry->w,
	    geometry->h);
	gtk_window_move(GTK_WINDOW(window), geometry->x, geometry->y);
	gtk_window_set_title(GTK_WINDOW(window), dir);

	model = gtk_list_store_new(MODEL_CNT,
	    G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);
	if (populate(model, dir) == -1)
		err(66, "failed to populate icon model from %s", dir);

	watch_dir(model, dir);

	gtk_icon_view_set_text_column(GTK_ICON_VIEW(icons), 0);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icons), 1);
	gtk_icon_view_set_model(GTK_ICON_VIEW(icons), GTK_TREE_MODEL(model));
	g_object_unref(model);

	gtk_widget_set_sensitive(directory_up, strlen(d->dir) > 1);

	/* Drag */
	gtk_drag_source_set(icons, GDK_BUTTON1_MASK,
	    dnd_targets, TARGET_COUNT,
	    GDK_ACTION_COPY | GDK_ACTION_MOVE);
	gtk_drag_source_add_text_targets(icons);
	gtk_drag_source_add_uri_targets(icons);
	g_signal_connect(icons, "drag-begin", G_CALLBACK(on_icons_drag_begin), d);
	g_signal_connect(icons, "drag-data-get", G_CALLBACK(on_icons_drag_data_get), d);
	g_signal_connect(icons, "drag-end", G_CALLBACK(on_icons_drag_end), d);

	/* Drop */
	gtk_drag_dest_set(icons, GTK_DEST_DEFAULT_ALL,
	    dnd_targets, TARGET_COUNT,
	    GDK_ACTION_COPY | GDK_ACTION_MOVE);
	gtk_drag_dest_add_text_targets(icons);
	gtk_drag_dest_add_uri_targets(icons);
	g_signal_connect(icons, "drag-motion", G_CALLBACK(on_icons_drag_motion), d);
	g_signal_connect(icons, "drag-leave", G_CALLBACK(on_icons_data_leave), d);
	g_signal_connect(icons, "drag-data-received", G_CALLBACK(on_icons_drag_data_received), d);

	/* Activations */
	g_signal_connect(icons, "item-activated", G_CALLBACK(on_icons_item_activated), d);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(window, "configure-event", G_CALLBACK(on_window_configure_event), dir);
	g_signal_connect(directory_up, "activate", G_CALLBACK(on_directory_up_menu_item_activate), d);
	g_signal_connect(directory_close, "activate", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(file_open, "activate", G_CALLBACK(on_file_open_menu_item_activate), d);
	g_signal_connect(icons, "button-press-event", G_CALLBACK(on_icons_button_press_event), d);

	return window;
}
