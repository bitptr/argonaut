#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include <libwnck/libwnck.h>

gchar		*find_argonautinfo();
int		 window_activate(WnckWindow *);
WnckWindow	*window_by_title(char *);
#endif /* WINDOW_H */
