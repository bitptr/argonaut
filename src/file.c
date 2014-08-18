#include <err.h>

#include <gtk/gtk.h>

#include "directory.h"
#include "extern.h"
#include "state.h"

void
activate(GtkIconView *iconview, GtkTreePath *path, struct state *d)
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
		open_directory(d, fullpath);
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
