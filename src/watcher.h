#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef WATCHER_H
#define WATCHER_H

#ifdef HAVE_KQUEUE

#include <gtk/gtk.h>

void	watch_dir(GtkListStore *, char *);

#else

#define watch_dir(m,d)

#endif
#endif /* WATCHER_H */
