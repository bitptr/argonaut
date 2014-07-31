/* vim: set sw=0 ts=8 cinoptions=:0,t0,+4,(4: */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <err.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "extern.h"

/*
 * Open a new argonaut on the specified directory.
 */
void
open_directory(struct cb_data *user_data, gchar *parent, gchar *dir_name)
{
	pid_t pid;
	gchar *dirpath;

	dirpath = g_strdup_printf("/%s/%s", parent, dir_name);

	pid = fork();
	switch(pid) {
	case -1:
		warn("could not open directory");
		break;
	case 0:
		execl(user_data->prog_path, user_data->argv0, dirpath, NULL);
		/* NOTREACHED */
		break;
	default:
		break;
	}

	g_free(dirpath);
}
