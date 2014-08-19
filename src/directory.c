#include <err.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "state.h"

/*
 * Open a new argonaut on the specified directory.
 */
void
open_directory(struct state *user_data, gchar *fullpath)
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
