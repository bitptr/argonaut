#ifndef DND_H
#define DND_H

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "state.h"

enum {
	TARGET_URI_LIST,
	TARGET_COUNT
};

void		on_icons_drag_data_received(GtkWidget *, GdkDragContext *,
    gint, gint, GtkSelectionData *, guint, guint, struct state *);
gboolean	on_icons_drag_motion(GtkWidget *, GdkDragContext *, gint, gint,
    guint, struct state *);
void		on_icons_data_leave(GtkWidget *, GdkDragContext *, guint,
    struct state *);

#endif /* DND_H */
