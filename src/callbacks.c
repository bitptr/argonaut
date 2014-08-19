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
#include "file.h"

static gboolean	on_middle_click(GtkWidget *, GdkEvent *, gpointer);

/*
 * Open the file using guesses from XDG.
 */
void
on_icons_item_activated(GtkIconView *iconview, GtkTreePath *path, struct state *user_data)
{
	activate(iconview, path, user_data);
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

/*
 * If the user middle-clicks, activate the icon; if that works, quit.
 */
gboolean
on_icons_button_press_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	if (on_middle_click(widget, event, user_data))
		gtk_main_quit();
	return FALSE;
}

/*
 * If the user middle-clicks, activate the icon.
 */
gboolean
on_desktop_icon_button_press_event(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	on_middle_click(widget, event, user_data);
	return FALSE;
}

/*
 * Return TRUE on activate, FALSE otherwise.
 */
static gboolean
on_middle_click(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	GdkEventButton	*e;
        struct state	*d;
	GtkTreePath	*tree_path;

	e = NULL;
	d = (struct state *)user_data;
	tree_path = NULL;

	if (event->type == GDK_BUTTON_PRESS)
		e = (GdkEventButton *)event;
	else
		goto error;

	if (e->button != 2)
		goto error;

	if (!gtk_icon_view_get_dest_item_at_pos(d->icon_view, e->x, e->y,
		    &tree_path, NULL))
		goto error;

	gtk_icon_view_unselect_all(d->icon_view);
	gtk_icon_view_select_path(d->icon_view, tree_path);

	activate(d->icon_view, tree_path, d);
	return TRUE;

error:

	if (tree_path != NULL)
		g_object_unref(tree_path);

	return FALSE;
}
