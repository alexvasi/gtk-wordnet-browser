#include <wn.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "gwb_window.h"


G_DEFINE_TYPE(GwbWindow, gwb_window, GTK_TYPE_WINDOW);
static const gint DEFAULT_SPACING = 6; /* where to get this value? */

static void
make_containers(GwbWindow *window)
{
    GtkWidget *entry_box, *main_box, *scroll_box;

    entry_box = gtk_hbox_new(FALSE, DEFAULT_SPACING);
    gtk_box_pack_start(GTK_BOX(entry_box), window->lookup_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), window->entry, TRUE, TRUE, 0);

    scroll_box = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scroll_box), window->text_view);

    main_box = gtk_vbox_new(FALSE, DEFAULT_SPACING);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), DEFAULT_SPACING);
    gtk_box_pack_start(GTK_BOX(main_box), entry_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), scroll_box, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    gtk_widget_show_all(main_box);
}

static void
on_lookup_word(GtkWidget *sender, GwbWindow *window)
{
    gchar *keyword, *results;
    GtkTextIter iter;
    int pos; /* WordNet part of speech type */

    keyword = g_strdup(gtk_entry_get_text(GTK_ENTRY(window->entry)));
    g_strstrip(keyword);
    g_strdelimit(keyword, " ", '_'); /* WordNet likes underscores. */

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(window->text_buffer), "", -1);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(window->text_buffer), &iter);
    for (pos = 1; pos <= NUMPARTS; pos++) {
        results = findtheinfo(keyword, pos, MAXSEARCH, ALLSENSES);
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(window->text_buffer),
                               &iter, results, -1);
    }

    g_free(keyword);
}

static void
gwb_window_init(GwbWindow *self)
{
    self->entry = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(self->entry), TRUE);

    self->lookup_btn = gtk_button_new_with_label(_("Look up:"));
    gtk_button_set_relief(GTK_BUTTON(self->lookup_btn), GTK_RELIEF_NONE);
    gtk_widget_set_can_default(self->lookup_btn, TRUE);
    g_signal_connect(self->lookup_btn, "clicked",
                     G_CALLBACK(on_lookup_word), self);

    self->text_view = gtk_text_view_new();
    self->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(self->text_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(self->text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(self->text_view), GTK_WRAP_WORD);

    make_containers(self);

    gtk_widget_grab_default(self->lookup_btn);
    gtk_widget_grab_focus(self->entry);
}

static void
gwb_window_class_init(GwbWindowClass *klass)
{

}

GtkWidget*
gwb_window_new()
{
    return g_object_new(GWB_TYPE_WINDOW, NULL);
}
