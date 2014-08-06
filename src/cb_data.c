#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "compat.h"
#include "extern.h"

/*
 * Build the initial data for the program. This data is passed to all
 * callbacks.
 */
struct cb_data *
cb_data_new(char *argv0)
{
	struct cb_data	*d;
	size_t		 len, cpy_len;

        if ((d = (struct cb_data*)malloc(sizeof(struct cb_data))) == NULL) {
		cb_data_free(d);
		return NULL;
	}

	len = strlen(argv0);
        if ((d->argv0 = calloc(len+1, sizeof(char))) == NULL) {
		cb_data_free(d);
		return NULL;
	}
	cpy_len = strlcpy(d->argv0, argv0, len+1);
        if (cpy_len < len) {
		cb_data_free(d);
		return NULL;
	}

	d->icon_view = NULL;
	d->tree_path = NULL;

	return d;
}

int
cb_data_add_dir(struct cb_data *d, char *dir)
{
	int	cpy_len, len;

	len = strlen(dir);
	if ((d->dir = calloc(len+1, sizeof(char))) == NULL)
		return -1;
	cpy_len = strlcpy(d->dir, dir, len+1);
	if (cpy_len < len)
		return -2;

	return 0;
}

/*
 * Clean up the cb_data struct.
 */
void
cb_data_free(struct cb_data *d)
{
	/*
	 * Do not free icon_view: this is managed by GTK.
	 * Do not free tree_path: this is only created in dnd_drag_motion and
	 * freed in dnd_drag_leave.
	 */
	free(d->dir);
	free(d->argv0);
	free(d);
}
