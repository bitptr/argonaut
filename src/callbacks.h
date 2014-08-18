#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "state.h"

void		on_icons_item_activated(GtkIconView *, GtkTreePath *,
    struct state *);
gboolean	on_window_configure_event(GtkWidget *, GdkEvent  *, char *);
#endif /* CALLBACKS_H */
