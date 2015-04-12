#include <err.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include "drag.h"
#include "grid.h"
#include "extern.h"
#include "state.h"
#include "store.h"

/*
 * Accumulate the URIs.
 */
void
on_icons_drag_begin(GtkWidget *widget, GdkDragContext *context,
    gpointer user_data)
{
	struct state	*d;
	GList		*acc;
	char		*fileuri;
	size_t		 len, elem_len;
	int	 	 i;
	GList		*selecteds;
	GtkTreePath	*path;

	d = (struct state *)user_data;
	acc = NULL;

	d->in_drag = 1;

	selecteds = gtk_icon_view_get_selected_items(d->icon_view);
	len = g_list_length(selecteds);

	if ((d->selected_uris = calloc(len + 1, sizeof(char*))) == NULL)
		err(1, "calloc");

	for (i = 0; selecteds != NULL; i++, selecteds = selecteds->next) {
		path = (GtkTreePath *)selecteds->data;
		fileuri = tree_path_file_uri(d->icon_view, path);
		elem_len = strlen(fileuri);
		d->selected_uris[i] = calloc(elem_len + 1, sizeof(char));
		if (d->selected_uris == NULL)
			err(1, "calloc");

		strlcpy(d->selected_uris[i], fileuri, elem_len + 1);

		free(fileuri);
	}

	d->selected_uris[len] = NULL;

	g_list_free_full(selecteds, (GDestroyNotify)gtk_tree_path_free);
}

/*
 * Free the URIs.
 */
void
on_icons_drag_end(GtkWidget *widget, GdkDragContext *context,
    gpointer user_data)
{
	struct state	*d;
	int		 i;

	d = (struct state *)user_data;
	d->in_drag = 0;
	for (i = 0; d->selected_uris[i] != NULL; i++)
		free(d->selected_uris[i]);
	free(d->selected_uris);

	repopulate(d);
}

void
on_icons_drag_data_get(GtkWidget *widget, GdkDragContext *context,
    GtkSelectionData *data, guint info, guint time, gpointer user_data)
{
	struct state	*d;

	d = (struct state *)user_data;

	if (gtk_selection_data_set_uris(data, d->selected_uris) == FALSE)
		warnx("gtk_selection_data_set_uris failed");
}
