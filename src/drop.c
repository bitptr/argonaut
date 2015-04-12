#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/stat.h>

#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <gio/gio.h>

#include "compat.h"
#include "drop.h"
#include "directory.h"
#include "state.h"
#include "store.h"
#include "extern.h"

static char	*determine_target(struct state *, gint, gint);
static int	 copy_or_move(GFile *, char *);
static int	 find_mountpoint(GFile *);
static int	 copy_file(GFile *, GFile *, char *);
static int	 handle_drop(GtkSelectionData *, gint, gint, struct state *);
static gboolean	 jump_dir(gpointer);

struct jump_state {
	struct state	*state;
	char		*dir_name;
};

/*
 * Called when the source program sends the data to us. This data must include
 * a URI, which we then operate on.
 */
void
on_icons_drag_data_received(GtkWidget *widget, GdkDragContext *context, gint x,
    gint y, GtkSelectionData *data, guint info, guint time, struct state *d)
{
	int	 should_delete;

	should_delete = (data != NULL) ?  handle_drop(data, x, y, d) : 0;
	gtk_drag_finish(context, TRUE, should_delete, time);
	repopulate(d);
}

/*
 * Called when the pointer is over a potential drag target.
 *
 * Highlight the directory if the pointer is over one, otherwise highlight the
 * icon grid.
 */
gboolean
on_icons_drag_motion(GtkWidget *widget, GdkDragContext *context, gint x, gint y,
    guint time, struct state *d)
{
	GtkTreeModel		*model;
	GtkTreeIter		 iter;
	gint	 		 type;
	struct jump_state	*j;
	char			*name;
	int			 len;

	model = gtk_icon_view_get_model(d->icon_view);

	if (d->tree_path != NULL)
		gtk_icon_view_unselect_path(d->icon_view, d->tree_path);

	if (gtk_icon_view_get_dest_item_at_pos(d->icon_view, x, y,
		    &d->tree_path, NULL)) {
		if (gtk_tree_model_get_iter(model, &iter, d->tree_path)) {
			gtk_tree_model_get(model, &iter,
			    FILE_NAME, &name,
			    FILE_TYPE, &type,
			    -1);
			if (type == DT_DIR) {
				gtk_icon_view_select_path(d->icon_view,
				    d->tree_path);
				gtk_drag_unhighlight(widget);

				if ((j = malloc(sizeof(struct jump_state))) == NULL)
					err(1, "malloc");
				len = strlen(name);
fprintf(stderr, "name = %s\n", name);
				if ((j->dir_name = calloc(len + 1, sizeof(char))) == NULL)
				    err(1, "calloc");
				strlcpy(j->dir_name, name, len+1);
fprintf(stderr, "j->dir_name = %s\n", j->dir_name);
				j->state = d;
				g_timeout_add_seconds(2, jump_dir, j);
			} else
				gtk_drag_highlight(widget);
		}
	} else
		gtk_drag_highlight(widget);

	gdk_drag_status(context, GDK_ACTION_COPY, time);
	return TRUE;
}

gboolean
jump_dir(gpointer user_data)
{
	struct jump_state *j;
	j = (struct jump_state *)user_data;
fprintf(stderr, "j->dir_name = %s\n", j->dir_name);
	open_directory(j->state, j->dir_name);
	free(j->dir_name);
	free(j);
	return 0;
}

/*
 * Called when the pointer leaves a drag target.
 *
 * Unhighlight and free any memory.
 */
void
on_icons_data_leave(GtkWidget *widget, GdkDragContext *context, guint time,
    struct state *d)
{
fprintf(stderr, "on_icons_data_leave\n");
	if (d->tree_path != NULL)
		gtk_icon_view_unselect_path(d->icon_view, d->tree_path);
	gtk_drag_unhighlight(widget);

	gtk_tree_path_free(d->tree_path);
	d->tree_path = NULL;
}

/*
 * For each URI in the selection, copy or move it to the target.
 *
 * RETURN: 0 if it was a copy, non-zero for a move.
 */
int
handle_drop(GtkSelectionData *data, gint x, gint y, struct state *d)
{
	gchar	**uris;
	char	*target;
	GFile	*g_source;
	int	 i, ret = 0;

	if ((uris = gtk_selection_data_get_uris(data)) == NULL)
		return ret;

	for (i = 0; uris[i] != NULL; i++) {
		g_source = g_file_new_for_uri(uris[i]);
		target = determine_target(d, x, y);
		ret = copy_or_move(g_source, target);

		if (ret < 0)
			warnx("failed to copy/move to %s: %i",
			    target, ret);

		free(target);
		g_object_unref(g_source);
	}

	g_strfreev(uris);

	return ret;
}

/*
 * Get the target of the drop.
 *
 * If the (x,y) is on a directory, that directory is the target of the drop;
 * otherwise, the current working directory is the target.
 */
char *
determine_target(struct state *d, gint x, gint y)
{
	GtkTreeModel	*model;
	GtkTreeIter	 iter;
	GtkTreePath	*tree_path;
	gchar		*directory, *basename;
	char		*target;
	gint	 	 type;
	int		 len;

	model = gtk_icon_view_get_model(d->icon_view);

	if (gtk_icon_view_get_dest_item_at_pos(d->icon_view, x, y,
		    &tree_path, NULL))
		if (gtk_tree_model_get_iter(model, &iter, tree_path)) {
			gtk_tree_model_get(model, &iter,
			    FILE_TYPE, &type,
			    FILE_PARENT, &directory,
			    FILE_NAME, &basename,
			    -1);
			if (type == DT_DIR) {
				len = strlen(directory) + strlen(basename) + 3;

				if ((target = calloc(len, sizeof(char))) == NULL)
					err(1, "calloc");

				snprintf(target, len, "%s/%s", directory,
				    basename);

				return target;
			}

			g_free(directory);
			g_free(basename);
		}

	len = strlen(d->dir) + 1;
	if ((target = calloc(len, sizeof(char))) == NULL)
		err(1, "calloc");
	strlcpy(target, d->dir, len);
	return target;
}

/*
 * Either copy or move the g_path in to the target directory.
 *
 * If the source and target are on the same mountpoint, do a move; otherwise,
 * copy.
 *
 * RETURN: 0 if copy, 1 if move, <0 on failure
 */
int
copy_or_move(GFile *g_source, char *target)
{
	GFile	*g_target, *g_target_file;
	int	 ret, is_err;
	dev_t	 source_dev, target_dev;
	GError	*error;
	char	*source_basename;

	ret = 0;
	is_err = 0;
	error = NULL;
	g_target_file = NULL;

	g_target = g_file_new_for_path(target);

	if ((is_err = find_mountpoint(g_source)) < 0) {
		ret = is_err;
		goto error;
	} else
		source_dev = (dev_t)is_err;

	if ((is_err = find_mountpoint(g_target)) < 0) {
		ret = is_err;
		goto error;
	} else
		target_dev = (dev_t)is_err;

	if ((source_basename = g_file_get_basename(g_source)) == NULL) {
		ret = -6;
		warnx("g_file_get_basename");
		goto error;
	}
	g_target_file = g_file_get_child(g_target, source_basename);

	if (source_dev == target_dev) {
		if (!g_file_move(g_source, g_target_file, G_FILE_COPY_NONE, NULL,
			    NULL, NULL, &error)) {
			ret = -3;
			warnx("g_file_move: %s", error->message);
		} else
			ret = 0;
	} else {
		if (copy_file(g_source, g_target_file, source_basename) < 0) {
			ret = -4;
		} else
			ret = 0;
	}

error:
	g_object_unref(g_target_file);
	g_object_unref(g_target);

	return ret;
}

/*
 * Find the mountpoint for a given path.
 *
 * RETURN: a dev_t representing the file status device, or a negative number on
 * failure.
 */
int
find_mountpoint(GFile *g_path)
{
	char		*path;
	struct stat	*sb;
	dev_t		 dev;

	if ((path = g_file_get_path(g_path)) == NULL) {
		warnx("g_file_get_path: Could not get the path");
		return -1;
	}

	if ((sb = malloc(sizeof(struct stat))) == NULL)
		err(1, "malloc");

	if (stat(path, sb) == -1) {
		warn("stat");
		return -5;
	}

	dev = sb->st_dev;

	free(sb);

	return dev;
}

/*
 * Copy a file. If the file is a directory, make the directory hierarchy and
 * then copy the file.
 *
 * This is more readable in shell:
 *
 *     if [ -d $g_source ]; then
 *       mkdir -p $g_target
 *       for child in $g_source / *; do
 *         copy_file "$g_source/$child" "$g_target/$child"
 *       done
 *     else
 *       cp $g_source $g_target
 *     fi
 *
 * RETURN: 0 on success, negative on failure.
 */
int
copy_file(GFile *g_source, GFile *g_target, char *source_basename)
{
	int		 ret;
	GError		*error;
	GFileInfo	*g_info;
	char		*source, *child_basename;
	const char	*child;
	GDir		*g_dir;
	GFile		*g_child, *g_target_child;

	error = NULL;
	source = NULL;
	ret = 0;
	g_child = NULL;
	g_target_child = NULL;
	child_basename = NULL;

	if ((g_info = g_file_query_info(g_source, "standard::type",
			    G_FILE_QUERY_INFO_NONE, NULL, &error)) == NULL) {
		ret = -6;
		warnx("g_file_query_info: %s", error->message);
		goto error;
	}

	if (g_file_info_get_file_type(g_info) == G_FILE_TYPE_DIRECTORY) {
		if (!g_file_make_directory_with_parents(
			    g_target, NULL, &error)) {
			ret = -9;
			warnx("g_file_make_directory_with_parents: %s",
			    error->message);
			goto error;
		}

		if ((source = g_file_get_path(g_source)) == NULL) {
			ret = -7;
			goto error;
		}

		if ((g_dir = g_dir_open(source, 0, &error)) == NULL) {
			ret = -8;
			warnx("g_dir_open: %s", error->message);
			goto error;
		}

		while ((child = g_dir_read_name(g_dir)) != NULL) {
			g_child = g_file_get_child(g_source, child);
			g_target_child = g_file_get_child(g_target, child);

			if ((child_basename = g_file_get_basename(g_child)) == NULL) {
				ret = -10;
				warnx("g_file_get_basename");
				goto error;
			}

			copy_file(
			    g_child,
			    g_target_child,
			    child_basename);

			if (g_child != NULL) {
				g_object_unref(g_child);
				g_child = NULL;
			}
			if (g_target_child != NULL) {
				g_object_unref(g_target_child);
				g_target_child = NULL;
			}

			g_free(child_basename);
			child_basename = NULL;
		}

		g_dir_close(g_dir);
	} else {
		if (!g_file_copy(g_source, g_target, G_FILE_COPY_NONE, NULL,
			    NULL, NULL, &error)) {
			ret = -4;
			warnx("g_file_copy: %s", error->message);
			goto error;
		} else
			ret = 0;
	}

error:

	g_free(child_basename);
	g_free(source);

	if (g_child != NULL) {
		g_object_unref(g_child);
		g_child = NULL;
	}

	if (g_target_child != NULL) {
		g_object_unref(g_target_child);
		g_target_child = NULL;
	}

	if (g_info != NULL) {
		g_object_unref(g_info);
		g_info = NULL;
	}
	return ret;
}
