#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dirent.h>
#include <db.h>
#include <err.h>
#include <string.h>

#include <gtk/gtk.h>

#include "directory.h"
#include "extern.h"
#include "main.h"
#include "window.h"
#include "state.h"

/*
 * Open the file using guesses from XDG.
 */
void
on_icons_item_activated(GtkIconView *iconview, GtkTreePath *path, struct state *user_data)
{
	gchar		*directory, *fullpath, *name;
	GdkScreen	*screen;
	GtkTreeModel	*model;
	GtkTreeIter	 iter;
	gint		 type;
	GError		*error;
	
	error = NULL;
        
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
	    FILE_TYPE, &type,
	    -1);

	fullpath = g_strdup_printf("file://%s/%s", directory, name);

	if (type == DT_DIR) {
		open_directory(user_data, fullpath);
	} else {
		screen = gdk_screen_get_default();
		gtk_show_uri(screen, fullpath, GDK_CURRENT_TIME, &error);
		if (error)
			g_print("%s\n", error->message);
	}

	g_free(name);
	g_free(directory);
	g_free(fullpath);
}

/*
 * Save the window size and position.
 */
gboolean
on_window_configure_event(GtkWidget *widget, GdkEvent *event, char *dir)
{
	DBT		 key, value;
	int		 ret, x, y;
	DB		*db;
	struct geometry	 g;

	memset(&key, 0, sizeof(DBT));
	memset(&value, 0, sizeof(DBT));

	/* event->configure.y is off by a decoration size, arbitrarily. */
	gtk_window_get_position(GTK_WINDOW(widget), &x, &y);

	g.x = x;
	g.y = y;
	g.h = event->configure.height;
	g.w = event->configure.width;

	key.data = dir;
	key.size = strlen(dir)+1;
	value.data = &g;
	value.size = sizeof(struct geometry);

	if (db_create(&db, NULL, 0) < 0)
		err(1, "could not open the db");

	if (db->open(db, NULL, find_argonautinfo(), NULL,
		    DB_HASH, DB_CREATE, 0) < 0)
		err(1, "could not create the db");

	if ((ret = db->put(db, NULL, &key, &value, 0)) != 0)
		db->err(db, ret, "could not save the geometry");

	if (db != NULL && db->close(db, 0) < 0)
		warn("could not close the db");

	return FALSE;
}
