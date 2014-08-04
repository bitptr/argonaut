/* vim: set sw=0 ts=8 cinoptions=:0,t0,+4,(4: */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <err.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "extern.h"

#include <stdio.h> /* XXX for debugging */

/*
 * Open a new argonaut on the specified directory.
 */
void
open_directory(struct cb_data *user_data, gchar *fullpath)
{
	pid_t	pid;

	pid = fork();
	switch(pid) {
	case -1:
		warn("could not open directory");
		break;
	case 0:
		execlp(user_data->argv0, user_data->argv0, fullpath, NULL);
		/* NOTREACHED */
		break;
	default:
		break;
	}
}
