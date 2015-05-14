#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xdefs.h>
#include <X11/Xatom.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "compat.h"
#include "dnd.h"
#include "drag.h"
#include "drop.h"
#include "extern.h"
#include "pathnames.h"
#include "state.h"
#include "store.h"
#include "watcher.h"

static void	 set_window_geometry(GtkWindow *, GtkWidget *);
static void	 desktopize(GtkWidget *);
static void	 skip_pager(GtkWidget *);
static char	*get_dir();
static void	 populate_model(char *, GtkIconView *);
GtkListStore	*populated_model(char *);
static void	 set_up_icon_view(GtkWidget *, struct state *);
static void	 override_background_color(GtkWidget *, GdkRGBA *);

/*
 * A desktop file manager. This opens files and directories the same way that
 * argonaut(1) does.
 */
int
main(int argc, char *argv[])
{
	GtkBuilder	*builder;
	GtkWidget	*root, *icons;
        struct state	*d;
	char		*dir;

	if ((dir = get_dir()) == NULL)
		err(1, "get_dir");

	if ((d = state_new(BINDIR"/argonaut")) == NULL)
		err(1, "could not build the callback data");

	if (state_add_dir(d, dir) < 0)
		err(1, "state_add_dir");

	gtk_init(&argc, &argv);

	builder = gtk_builder_new_from_file(INTERFACE_PATH);
	icons = GTK_WIDGET(gtk_builder_get_object(builder, "desktop-icons"));
	d->icon_view = GTK_ICON_VIEW(icons);

	root = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_name(root, "argonaut-desktop");

	set_window_geometry(GTK_WINDOW(root), GTK_WIDGET(icons));
	desktopize(root);
	skip_pager(root);
	populate_model(dir, d->icon_view);
	set_up_icon_view(icons, d);

	gtk_container_add(GTK_CONTAINER(root), icons);

	gtk_widget_show(icons);
	gtk_widget_show(root);

	gtk_main();

	state_free(d);
	free(dir);
	return 0;
}

/*
 * Figure out the size of the working area and resize the window to fit into
 * that.
 */
static void
set_window_geometry(GtkWindow *window, GtkWidget *widget)
{
	GdkScreen	*screen;
	GdkRectangle	 rect;

	if ((screen = gdk_screen_get_default()) == NULL)
		errx(1, "gdk_screen_get_default");

	gdk_screen_get_monitor_workarea(screen, 0, &rect);

	gtk_window_set_default_size(window, rect.width, rect.height);
	gtk_widget_set_size_request(widget, rect.width, rect.height);
	gtk_window_move(window, rect.x, rect.y);
}

/*
 * Make the widget a root window. Set its class, set the _NET_WM_WINDOW_TYPE to
 * _NET_WM_WINDOW_TYPE_DESKTOP, and set the is_desktop_window property to true.
 */
static void
desktopize(GtkWidget *root)
{
	GdkAtom		 atom;
	GdkWindow	*gdk_window;

	atom = gdk_atom_intern("_NET_WM_WINDOW_TYPE_DESKTOP", FALSE);

	gtk_window_set_wmclass(GTK_WINDOW(root), "desktop_window", "Argonaut");

	gtk_widget_realize(root);

	if ((gdk_window = gtk_widget_get_window(root)) == NULL)
		errx(1, "gtk_widget_get_window: cannot get window");

	gdk_property_change(gdk_window,
	    gdk_atom_intern("_NET_WM_WINDOW_TYPE", FALSE),
	    gdk_x11_xatom_to_atom(XA_ATOM), 32,
	    GDK_PROP_MODE_REPLACE, (guchar *)&atom, 1);

	g_object_set_data(G_OBJECT(root), "is_desktop_window", 
	    GINT_TO_POINTER(1));
}

/*
 * The window should not show up in the pager. This sets an EWMH property.
 */
static void
skip_pager(GtkWidget *window)
{
	GValue	t = G_VALUE_INIT;

	g_value_init(&t, G_TYPE_BOOLEAN);
	g_value_set_boolean(&t, TRUE);
	g_object_set_property(G_OBJECT(window), "skip-pager-hint", &t);
	g_value_unset(&t);
}

/*
 * Produce the directory that is represented by the desktop. If we can get the
 * $HOME environment variable, return that; otherwise return "/".
 *
 * The result should be free(3)'ed.
 */
static char *
get_dir()
{
	char	*dir, *home;

	if ((home = getenv("HOME")) == NULL)
		if ((dir = calloc(2, sizeof(char))) == NULL)
			return NULL;
		else
			strlcpy(dir, "/", 2);
	else if ((dir = calloc(strlen(home) + 1, sizeof(char))) == NULL)
		return NULL;
	else
		strlcpy(dir, home, strlen(home) + 1);

	return dir;
}

/*
 * Set up the model and attach it to the GtkIconView.
 */
static void
populate_model(char *dir, GtkIconView *icon_view)
{
	GtkListStore *model;
	model = populated_model(dir);
	gtk_icon_view_set_model(icon_view, GTK_TREE_MODEL(model));
	g_object_unref(model);
}

/*
 * Return a populated model.
 *
 * The result should be freed using g_object_unref.
 */
GtkListStore *
populated_model(char *dir)
{
	GtkListStore	*model;

	model = gtk_list_store_new(MODEL_CNT,
	    G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_INT);

	if (populate(model, dir) == -1)
		errx(66, "failed to populate icon model from %s", dir);

	watch_dir(model, dir);

	return model;
}

/*
 * Configure the GtkIconView: background color, visual columns, drag-and-drop,
 * and icon activation.
 */
static void
set_up_icon_view(GtkWidget *icons, struct state *d)
{
	/* Background color */
	GdkRGBA	transparent = {0, 0, 0, 0};
	override_background_color(icons, &transparent);

	/* Columns */
	gtk_icon_view_set_text_column(d->icon_view, FILE_NAME);
	gtk_icon_view_set_pixbuf_column(d->icon_view, FILE_ICON);

	/* Drag */
	gtk_drag_source_set(icons, GDK_BUTTON1_MASK,
	    dnd_targets, TARGET_COUNT,
	    GDK_ACTION_COPY | GDK_ACTION_MOVE);
	gtk_drag_source_add_text_targets(icons);
	gtk_drag_source_add_uri_targets(icons);
	g_signal_connect(icons, "drag-begin", G_CALLBACK(on_icons_drag_begin), d);
	g_signal_connect(icons, "drag-data-get", G_CALLBACK(on_icons_drag_data_get), d);
	g_signal_connect(icons, "drag-end", G_CALLBACK(on_icons_drag_end), d);

	/* Drop */
	gtk_drag_dest_set(icons, GTK_DEST_DEFAULT_ALL,
	    dnd_targets, TARGET_COUNT,
	    GDK_ACTION_COPY | GDK_ACTION_MOVE);
	gtk_drag_dest_add_text_targets(icons);
	gtk_drag_dest_add_uri_targets(icons);
	g_signal_connect(icons, "drag-motion", G_CALLBACK(on_icons_drag_motion), d);
	g_signal_connect(icons, "drag-leave", G_CALLBACK(on_icons_data_leave), d);
	g_signal_connect(icons, "drag-data-received", G_CALLBACK(on_icons_drag_data_received), d);

	/* Activations */
	g_signal_connect(icons, "item-activated", G_CALLBACK(on_icons_item_activated), d);
	g_signal_connect(icons, "button-press-event", G_CALLBACK(on_desktop_icon_button_press_event), d);
}

static void
override_background_color(GtkWidget *widget, GdkRGBA *rgba)
{
	gchar          *css;
	GtkCssProvider *provider;

	provider = gtk_css_provider_new();

	css = g_strdup_printf("* { background-color: %s; }",
	    gdk_rgba_to_string(rgba));
	gtk_css_provider_load_from_data(provider, css, -1, NULL);
	g_free(css);

	gtk_style_context_add_provider(
	    gtk_widget_get_style_context(widget),
	    GTK_STYLE_PROVIDER(provider),
	    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	g_object_unref(provider);
}
