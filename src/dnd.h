#ifndef DND_H
#define DND_H

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "extern.h"

void		dnd_drag_data_received(GtkWidget *, GdkDragContext *, gint, gint,
    GtkSelectionData *, guint, guint, struct cb_data *);
gboolean 	dnd_drag_motion(GtkWidget *, GdkDragContext *, gint, gint,
    guint, struct cb_data *);
void		dnd_drag_leave(GtkWidget *, GdkDragContext *, guint,
    struct cb_data *);
#endif
