#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "compat.h"
#include "state.h"

/*
 * Build the initial data for the program. This data is passed to all
 * callbacks.
 */
struct state *
state_new(char *argv0)
{
	struct state	*d;
	size_t		 len, cpy_len;

        if ((d = (struct state*)malloc(sizeof(struct state))) == NULL) {
		state_free(d);
		return NULL;
	}

	len = strlen(argv0);
        if ((d->argv0 = calloc(len+1, sizeof(char))) == NULL) {
		state_free(d);
		return NULL;
	}
	cpy_len = strlcpy(d->argv0, argv0, len+1);
        if (cpy_len < len) {
		state_free(d);
		return NULL;
	}

	d->icon_view = NULL;
	d->tree_path = NULL;
	d->dir = NULL;
	d->pid = 0;

	return d;
}

int
state_add_dir(struct state *d, char *dir)
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

void
state_add_known_pid(struct state *d, pid_t pid)
{
	d->pid = pid;
}

/*
 * Clean up the state struct.
 */
void
state_free(struct state *d)
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
