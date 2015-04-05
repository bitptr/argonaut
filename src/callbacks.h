#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

#include "state.h"

void		on_icons_item_activated(GtkIconView *, GtkTreePath *,
    struct state *);
void		on_file_open_menu_item_activate(GtkMenuItem *, gpointer);
gboolean	on_window_configure_event(GtkWidget *, GdkEvent  *, char *);
gboolean	on_icons_button_press_event(GtkWidget *, GdkEvent *, gpointer);
gboolean	on_desktop_icon_button_press_event(GtkWidget *, GdkEvent *, gpointer);
#endif /* CALLBACKS_H */
