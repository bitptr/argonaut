#include <gtk/gtk.h>

int
main(int argc, char *argv[]) {
	GtkBuilder *builder;
	GtkWidget *window;

	gtk_init(&argc, &argv);

        builder = gtk_builder_new_from_file("/home/mike/argonaut-py/argonaut.ui");
        window = GTK_WIDGET(gtk_builder_get_object(builder, "directory-window"));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

        gtk_widget_show(window);

	gtk_main ();

	return 0;
}
