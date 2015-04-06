#include <err.h>
#include <stddef.h>

#include <gtk/gtk.h>

#include "grid.h"
#include "extern.h"

/* TODO: use in activate() */
/*
 * Get the file URI for the tree path in the icon view.
 *
 * RETURN: a string with the URI in it. It must be free()'d.
 */
char *
tree_path_file_uri(GtkIconView *icon_view, GtkTreePath *path)
{
	gchar		*directory, *fileuri, *name;
	GError		*error;
	GtkTreeModel	*model;
	GtkTreeIter	 iter;

	error = NULL;

	if ((model = gtk_icon_view_get_model(icon_view)) == NULL)
		errx(66, "could not find the model for the icon view");

	gtk_tree_model_get_iter(model, &iter, path);

	gtk_tree_model_get(model, &iter,
	    FILE_NAME, &name,
	    FILE_PARENT, &directory,
	    -1);

	fileuri = g_strdup_printf("file://%s/%s", directory, name);

	g_free(name);
	g_free(directory);

	return fileuri;
}
