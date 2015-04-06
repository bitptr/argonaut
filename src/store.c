#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dirent.h>
#include <err.h>

#include <gtk/gtk.h>

#include "extern.h"
#include "thumbnail.h"
#include "store.h"

static void	store_insert(GtkListStore *, struct dirent *, char *);

/*
 * Add each file in the directory into the model.
 */
int
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
 * Add each file in the directory in the state into the model for the icon view
 * in the state.
 */
void
repopulate(struct state *d)
{
	GtkTreeModel	*model;

	if ((model = gtk_icon_view_get_model(d->icon_view)) == NULL)
		errx(66, "could not find the model for the icon view");

	gtk_list_store_clear(GTK_LIST_STORE(model));
	if (populate(GTK_LIST_STORE(model), d->dir) == -1)
		errx(66, "failed to populate icon model from %s", d->dir);
}

/*
 * Add to the model the file name, directory name, and an icon.
 */
static void
store_insert(GtkListStore *model, struct dirent *dp, char *directory)
{
	GdkPixbuf	*pixbuf;
	GtkTreeIter	 iter;

	pixbuf = find_thumbnail(dp, directory);

	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
	    FILE_NAME, dp->d_name,
	    FILE_ICON, pixbuf,
	    FILE_PARENT, directory,
	    FILE_TYPE, dp->d_type,
	    -1);
}
