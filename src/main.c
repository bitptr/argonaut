#include <gtk/gtk.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>

#define INTERFACE_PATH "/home/mike/argonaut/argonaut.ui"
#define DEFAULT_HEIGHT 350
#define DEFAULT_WIDTH 700

extern int errno;

int populate(GtkListStore *, const char*);

int
main(int argc, char *argv[]) {
	GtkBuilder *builder;
	GtkWidget *window;
	GtkWidget *icons;
	GtkListStore *model;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new_from_file(INTERFACE_PATH);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "directory-window"));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_default_size(GTK_WINDOW(window), DEFAULT_WIDTH, DEFAULT_HEIGHT);

	model = gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
	if (populate(model, "/home/mike") == -1)
	  err(66, "failed to populate icon model from /home/mike");

	icons = GTK_WIDGET(gtk_builder_get_object(builder, "file-icons"));
	gtk_icon_view_set_text_column(GTK_ICON_VIEW(icons), 0);
	gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icons), 1);
	gtk_icon_view_set_model(GTK_ICON_VIEW(icons), GTK_TREE_MODEL(model));
	g_object_unref(model);

	gtk_widget_show(window);

	gtk_main ();

	return 0;
}

int
populate(GtkListStore *model, const char *directory) {
	DIR *dirp;
	struct dirent *dp;
	GtkTreeIter iter;
	GdkPixbuf *pixbuf;

	pixbuf = gdk_pixbuf_new_from_file(
	    "/usr/local/share/icons/gnome/32x32/mimetypes/gtk-file.png",
	    NULL);
	if (!pixbuf)
	  errx(66, "could not load the pixbuf");

	dirp = opendir(directory);
	if (dirp) {
		while ((dp = readdir(dirp)) != NULL) {
			gtk_list_store_append(model, &iter);
			gtk_list_store_set(model, &iter,
			    0, "hello",
			    1, GDK_PIXBUF(pixbuf),
			    -1);
		}
		return closedir(dirp);
	}

	return -1;
}
