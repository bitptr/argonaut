#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include <gtk/gtk.h>

#include "signals.h"
#include "state.h"

volatile sig_atomic_t chld_flag = 0;

void
chld_handler(int sig)
{
	chld_flag = 1;
}

gboolean
chld_notifier(gpointer data)
{
	struct state	*d;

	if (chld_flag) {
		d = (struct state *)data;
		if (d->pid > 1)
			if (kill(d->pid, SIGCHLD) != 0)
				warn("kill");
		exit(0);
	}

	return G_SOURCE_CONTINUE;
}
