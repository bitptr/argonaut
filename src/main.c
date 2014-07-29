#include <gtk/gtk.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>

#define INTERFACE_PATH "/home/mike/argonaut/argonaut.ui"
#define DEFAULT_HEIGHT 350
#define DEFAULT_WIDTH 700

extern int errno;

int populate(GtkListStore *, char*);
void store_insert(GtkListStore *, struct dirent *);

int
main(int argc, char *argv[]) {
	GtkBuilder *builder;
	GtkWidget *window;
	GtkWidget *icons;
	GtkListStore *model;
        char *dir;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new_from_file(INTERFACE_PATH);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "directory-window"));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_default_size(GTK_WINDOW(window), DEFAULT_WIDTH, DEFAULT_HEIGHT);

	if ((dir = getenv("HOME")) == NULL)
		dir = "/";

	model = gtk_list_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
	if (populate(model, dir) == -1)
	  err(66, "failed to populate icon model from %s", dir);

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
populate(GtkListStore *model, char *directory) {
	DIR *dirp;
	struct dirent *dp;

	dirp = opendir(directory);
	if (dirp) {
		while ((dp = readdir(dirp)) != NULL)
			if (dp->d_name[0] != '.')
                        	store_insert(model, dp);
		return closedir(dirp);
	}

	return -1;
}

void
store_insert(GtkListStore *model, struct dirent *dp) {
	GtkTreeIter iter;
	GdkPixbuf *file_pixbuf, *dir_pixbuf;

	file_pixbuf = gdk_pixbuf_new_from_file(
	    "/usr/local/share/icons/gnome/32x32/mimetypes/gtk-file.png",
	    NULL);
	if (!file_pixbuf)
	  errx(66, "could not load the file pixbuf");

	dir_pixbuf = gdk_pixbuf_new_from_file(
	    "/usr/local/share/icons/gnome/32x32/places/folder.png",
	    NULL);
	if (!dir_pixbuf)
	  errx(66, "could not load the directory pixbuf");

	gtk_list_store_append(model, &iter);
	gtk_list_store_set(model, &iter,
	    0, dp->d_name,
	    1, GDK_PIXBUF(dp->d_type == DT_DIR ? dir_pixbuf : file_pixbuf),
	    -1);
}
