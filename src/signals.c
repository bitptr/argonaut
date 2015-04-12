#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include <gtk/gtk.h>

#include "signals.h"
#include "state.h"

void
chld_handler(int sig)
{
	exit_flag = 1;
}
