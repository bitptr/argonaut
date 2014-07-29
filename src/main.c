#include <gtk/gtk.h>

#define INTERFACE_PATH "/home/mike/argonaut/argonaut.ui"
#define DEFAULT_HEIGHT 350
#define DEFAULT_WIDTH 700

int
main(int argc, char *argv[]) {
	GtkBuilder *builder;
	GtkWidget *window;

	gtk_init(&argc, &argv);

	builder = gtk_builder_new_from_file(INTERFACE_PATH);
	window = GTK_WIDGET(gtk_builder_get_object(builder, "directory-window"));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_default_size(GTK_WINDOW(window), DEFAULT_WIDTH, DEFAULT_HEIGHT);

	gtk_widget_show(window);

	gtk_main ();

	return 0;
}
