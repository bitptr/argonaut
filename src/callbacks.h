#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

#include "state.h"

void		on_icons_item_activated(GtkIconView *, GtkTreePath *,
    struct state *);
gboolean	on_window_configure_event(GtkWidget *, GdkEvent  *, char *);
gboolean	on_icons_button_press_event(GtkWidget *, GdkEvent *, gpointer);
#endif /* CALLBACKS_H */