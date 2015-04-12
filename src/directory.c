#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "state.h"

/*
 * Open a new argonaut on the specified directory.
 */
void
open_directory(struct state *user_data, gchar *fullpath)
{
	pid_t	 pid, my_pid;
	char	*my_pid_char;
	size_t	 len;

	my_pid = getpid();
	pid = fork();

	switch(pid) {
	case -1:
		warn("could not open directory");
		break;
	case 0:
		if (user_data->in_drag) {
			len = (size_t)(ceil(log10(my_pid))+1);
			if ((my_pid_char = calloc(len, sizeof(char))) == NULL)
				err(1, "calloc");
			snprintf(my_pid_char, len, "%d", my_pid);

			execlp(user_data->argv0, user_data->argv0,
			    "-9", my_pid_char, fullpath,
			    NULL);
		} else
			execlp(user_data->argv0, user_data->argv0,
			    fullpath, NULL);

		/* NOTREACHED */
		free(my_pid_char);
		err(1, "execlp");
		break;
	default:
		break;
	}
}
