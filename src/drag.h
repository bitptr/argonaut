#ifndef DRAG_H
#define DRAG_H

void	on_icons_drag_begin(GtkWidget *, GdkDragContext *, gpointer);
void	on_icons_drag_end(GtkWidget *, GdkDragContext *, gpointer);
void	on_icons_drag_data_get(GtkWidget *, GdkDragContext *,
    GtkSelectionData *, guint, guint, gpointer);

#endif /* DRAG_H */
