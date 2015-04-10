#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dirent.h>
#include <db.h>
#include <err.h>
#include <libgen.h>
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "directory.h"
#include "extern.h"
#include "main.h"
#include "window.h"
#include "state.h"
#include "file.h"

static gboolean	on_middle_click(GtkWidget *, GdkEvent *, gpointer);
static void activate_path(gpointer, gpointer);

/*
 * Open the file using guesses from XDG.
 */
void
on_icons_item_activated(GtkIconView *iconview, GtkTreePath *path, struct state *user_data)
{
	activate(iconview, path, user_data);
}

/*
 * Open the parent directory.
 */
void
on_directory_up_menu_item_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	struct state	*d;
	char		*parent, *cwd;
	size_t		 len;

	d = (struct state *)user_data;
	len = strlen(d->dir);

	if (len < 2)
		return;

	if ((cwd = calloc(len + 1, sizeof(char))) == NULL)
		err(1, "calloc");

	strlcpy(cwd, d->dir, len + 1);

	if ((parent = dirname(cwd)) == NULL) {
		warn("dirname");
		goto done;
	}

	open_directory(d, parent);
done:
	free(cwd);
}

/*
 * Open the selected files using guesses from XDG.
 */
void
on_file_open_menu_item_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	struct state	*d;
	GList		*items;

	d = (struct state *)user_data;
	items = gtk_icon_view_get_selected_items(d->icon_view);

	g_list_foreach(items, activate_path, user_data);

	g_list_free_full(items, (GDestroyNotify)gtk_tree_path_free);
}

/*
 * Activate the given path in the context of the given user_data.
 */
static void
activate_path(gpointer data, gpointer user_data)
{
	GtkTreePath	*path;
	struct state	*d;

	path = (GtkTreePath *)data;
	d = (struct state *)user_data;

	activate(d->icon_view, path, d);
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
		goto done;

	if (e->button != 2)
		goto done;

	if (!gtk_icon_view_get_dest_item_at_pos(d->icon_view, e->x, e->y,
		    &tree_path, NULL))
		goto done;

	gtk_icon_view_unselect_all(d->icon_view);
	gtk_icon_view_select_path(d->icon_view, tree_path);

	activate(d->icon_view, tree_path, d);
	return TRUE;

done:

	if (tree_path != NULL)
		g_object_unref(tree_path);

	return FALSE;
}
