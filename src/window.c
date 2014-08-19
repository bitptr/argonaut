#include <sys/time.h>

#include <err.h>
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

#include "compat.h"
#include "window.h"

struct match {
	char		*name;		/* The name of the window */
	int		 len;		/* The length of the name */
	WnckWindow	*result;	/* The result */
};

static struct match	*match_new(char *);
static void		 set_match(gpointer, gpointer);

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

/*
 * Get the list of all open windows and find the one that matches us.
 *
 * RETURN: The matching WnckWindow*, or NULL.
 */
WnckWindow*
window_by_title(char *dir)
{
	WnckScreen	*screen;
	GList		*windows;
	struct match	*m;
	WnckWindow	*result;

	result = NULL;
	m = NULL;

	if ((screen = wnck_screen_get_default()) == NULL) {
		warnx("wnck_screen_get_default");
		goto done;
	}

	wnck_screen_force_update(screen);

	windows = wnck_screen_get_windows(screen);

	m = match_new(dir);

	g_list_foreach(windows, set_match, m);

	if (m->result != NULL) {
		if ((result = malloc(sizeof(WnckWindow))) == NULL)
			err(1, "malloc");
		memcpy(result, m->result, sizeof(WnckWindow));
	}

done:

	free(m->name);
	free(m);

	return result;
}

/*
 * Activate the window.
 *
 * RETURN: 0 on success, -1 on failure.
 */
int
window_activate(WnckWindow *w)
{
	struct timeval	*tv;
	int		 ret;

	ret = 0;

	if ((tv = malloc(sizeof(struct timeval))) == NULL)
		err(1, "malloc");

	if (gettimeofday(tv, NULL) == -1) {
		warn("gettimeofday");
		ret = -1;
		goto done;
	}

	wnck_window_activate(w, tv->tv_sec);

done:

	free(tv);
	if (w != NULL)
		g_object_unref(w);
	return ret;
}

/*
 * If the WnckWindow in data matches the desired struct match in user_data, set
 * the result in the user_data.
 */
static void
set_match(gpointer data, gpointer user_data)
{
	const char	*name, *class;
	struct match	*m;
	WnckWindow	*w;
	w = WNCK_WINDOW(data);
	m = (struct match *)user_data;

	if (m->result != NULL)
		return;

	class = wnck_window_get_class_instance_name(w);

	if (strncmp(class, "argonaut", 9) != 0)
		return;

	name = wnck_window_get_name(w);

	if (strncmp(name, m->name, m->len) != 0)
		return;

	m->result = w;
}

/*
 * Allocate a new match structure with an empty result. The name for the match
 * is the directory name.
 *
 * RETURN: the new match structure. Deallocate it with free(3).
 */
struct match *
match_new(char *dir)
{
	int		 cpy_len;
	struct match	*m;

	if ((m = malloc(sizeof(struct match))) == NULL)
		err(1, "malloc");

	m->len = strlen(dir) + 1;

	if ((m->name = calloc(m->len, sizeof(char))) == NULL)
		err(1, "calloc");

	cpy_len = strlcpy(m->name, dir, m->len);
        if (cpy_len < m->len - 1)
		err(1, "strlcpy");

	m->result = NULL;

	return m;
}
