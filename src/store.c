#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dirent.h>

#include <gtk/gtk.h>

#include "extern.h"
#include "thumbnail.h"

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
