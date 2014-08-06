#ifndef EXTERN_H
#define EXTERN_H

#include <gtk/gtk.h>

enum model_storage {
	FILE_NAME,
	FILE_ICON,
	FILE_PARENT,
	FILE_TYPE
};

struct cb_data {
	char		*argv0;		/* How the user calls argonaut */
	char		*dir;		/* The directory */
	GtkIconView	*icon_view;	/* The grid of icons */
	GtkTreePath	*tree_path;	/* The tree path to the drop target */
};

struct cb_data	*cb_data_new(char *);
void		 cb_data_free(struct cb_data *);
int		 cb_data_add_dir(struct cb_data *, char *);

#endif /* EXTERN_H */
