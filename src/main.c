#include <wn.h>
#include <gtk/gtk.h>
#include <stdlib.h>

#include "gwb_window.h"


static void
on_window_destroyed(GtkObject *obj, gpointer data)
{
    gtk_main_quit();
}

static GtkWidget*
create_main_window()
{
    GtkWidget *window;

    window = gwb_window_new();
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroyed), NULL);
    gtk_widget_show(window);

    return window;
}

int
main(int argc, char *argv[])
{
    gtk_set_locale();
    gtk_init(&argc, &argv);

    if (wninit()) {
        g_critical("Can't open WordNet database.");
        return EXIT_FAILURE;
    }
    create_main_window();

    gtk_main();
    return EXIT_SUCCESS;
}
