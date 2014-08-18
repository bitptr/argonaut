#include <stdlib.h>

#include <gtk/gtk.h>

/*
 * Find .argonautinfo. First look in $HOME, then fall back to /.
 */
gchar *
find_argonautinfo()
{
	gchar	*home;

	home = getenv("HOME");
	if (home)
		return g_strjoin("/", home, ".argonautinfo", NULL);
	else
		return "/.argonautinfo";
}
