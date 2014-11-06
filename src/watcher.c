#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_KQUEUE

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "store.h"

struct thread_data {
	GtkListStore	*model;
	char		*dir;
};

gpointer			 watch_dir_func(gpointer);
gboolean			 repopulate(gpointer);
static struct thread_data	*thread_data_new(GtkListStore *, char *);
static void			 thread_data_free(struct thread_data *);

/*
 * Start a thread that watches a directory; update the model when this
 * directory changes.
 */
void
watch_dir(GtkListStore *model, char *dir)
{
	GThread			*watcher_thr;
	struct thread_data	*thr_d;

	thr_d = thread_data_new(model, dir);

	watcher_thr = g_thread_new("watcher", watch_dir_func, thr_d);
	g_thread_unref(watcher_thr);
}

/*
 * Watch a directory; when it changes, update the model.
 *
 * This runs in a non-main thread.
 */
gpointer
watch_dir_func(gpointer data)
{
	int			 kq, fd, ret;
	struct kevent		 kev;
	char			*dir;
	struct thread_data	*thr_d;

	thr_d = (struct thread_data *)data;
	dir = thr_d->dir;

	if ((fd = open(dir, O_RDONLY)) == -1)
		err(1, "open");

	if ((kq = kqueue()) == -1)
		err(1, "kqueue");

	EV_SET(&kev, fd, EVFILT_VNODE, EV_ADD | EV_CLEAR, NOTE_WRITE, 0, NULL);

	if (kevent(kq, &kev, 1, NULL, 0, NULL) == -1)
		err(1, "kevent");

	for (;;) {
		ret = kevent(kq, NULL, 0, &kev, 1, NULL);

		if (ret == -1)
			err(1, "kevent");

		if (ret == 0)
			break;

		g_idle_add(repopulate, data);
	}

	thread_data_free(data);

	if (close(fd) == -1)
		err(1, "close");

	return NULL;
}

/*
 * Update the model. When done, remove this function from the list of sources
 * to run when idle.
 *
 * RETURN: False, to indicate that the function should be run only once.
 */
gboolean
repopulate(gpointer data)
{
	GtkListStore		*model;
	char			*dir;
	struct thread_data	*thr_d;

	thr_d = (struct thread_data *)data;
	model = thr_d->model;
	dir = thr_d->dir;

	gtk_list_store_clear(model);
	if (populate(model, dir) == -1)
		errx(66, "failed to populate icon model from %s", dir);

	return G_SOURCE_REMOVE;
}

/*
 * Construct a new thread_data structure storing the given model and dir.
 *
 * RETURN: The new thread_data structure. Deallocate it with
 * thread_data_free().
 */
struct thread_data *
thread_data_new(GtkListStore *model, char *dir)
{

	struct thread_data	*thr_d;
	size_t			 dir_len, cpy_len;

	dir_len = strlen(dir);

	if ((thr_d = (struct thread_data*)malloc(sizeof(struct thread_data))) == NULL)
		err(1, "malloc");

	thr_d->model = model;

	if ((thr_d->dir = (char *)calloc(dir_len, sizeof(char))) == NULL)
		err(1, "malloc");
	cpy_len = strlcpy(thr_d->dir, dir, dir_len+1);
         if (cpy_len < dir_len)
		err(1, "strlcpy");

	return thr_d;
}

/*
 * Clean up the given thread_data structure.
 */
void
thread_data_free(struct thread_data *thr_d)
{
	free(thr_d->dir);
	free(thr_d);
}

#else


/*
 * This file needs a "translation unit" - it needs anything for the compiler to
 * compile.
 */
int unused;

#endif
