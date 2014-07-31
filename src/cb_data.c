/* vim: set sw=0 ts=8 cinoptions=:0,t0,+4,(4: */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
	char		*progname;
	size_t		 len, cpy_len;

        if ((d = (struct cb_data*)malloc(sizeof(struct cb_data))) == NULL)
		goto error;

	if ((progname = realpath(argv0, NULL)) == NULL)
		goto error;

        len = strlen(progname);
        if ((d->prog_path = calloc(len+1, sizeof(char))) == NULL)
		goto error;
	cpy_len = strlcpy(d->prog_path, progname, len+1);
        if (cpy_len < len)
		goto error;

	len = strlen(argv0);
        if ((d->argv0 = calloc(len+1, sizeof(char))) == NULL)
		goto error;
	cpy_len = strlcpy(d->argv0, argv0, len+1);
        if (cpy_len < len)
		goto error;

	return d;

error:
	free(d->prog_path);
	free(d->argv0);
	free(d);
	return NULL;
}

/*
 * Clean up the cb_data struct.
 */
void
cb_data_free(struct cb_data *d)
{
        free(d->argv0);
        free(d->prog_path);
        free(d);
}

