#include <wn.h>
#include <strings.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "config.h"
#include "gwb_window.h"


G_DEFINE_TYPE(GwbWindow, gwb_window, GTK_TYPE_WINDOW);
static const gint DEFAULT_SPACING = 6; /* where to get this value? */
static const gint DEFAULT_INTEND = 30;
static const gint DEFAULT_LINES_INTERVAL = 4;

static void
set_window_title(GwbWindow *w, gchar *subtitle)
{
    gchar *title;

    if (subtitle && subtitle[0]) {
        title = g_strdup_printf("%s - %s", subtitle, PACKAGE_NAME);
        gtk_window_set_title(GTK_WINDOW(w), title);
        g_free(title);
    }
    else {
        gtk_window_set_title(GTK_WINDOW(w), PACKAGE_NAME);
    }
}

static void
make_text_tags(GtkTextBuffer *buffer)
{
    gtk_text_buffer_create_tag(buffer, "header",
                               "scale", 1.5,
                               "pixels-below-lines", 10,
                               "pixels-above-lines", DEFAULT_SPACING,
                               "weight", PANGO_WEIGHT_BOLD,
                               NULL);

    gtk_text_buffer_create_tag(buffer, "highlighted",
                               "weight", PANGO_WEIGHT_BOLD,
                               NULL);

    gtk_text_buffer_create_tag(buffer, "example",
                               "style", PANGO_STYLE_ITALIC,
                               NULL);
}

static void
append_header(GtkTextIter *iter, const gchar *header)
{
    GtkTextBuffer *buffer;
    GString *text;

    text = g_string_new(header);
    g_string_append_c(text, '\n');
    text->str[0] = g_ascii_toupper(text->str[0]);

    buffer = gtk_text_iter_get_buffer(iter);
    gtk_text_buffer_insert_with_tags_by_name(buffer, iter, text->str, -1,
                                             "header", NULL);

    g_string_free(text, TRUE);
}

static void
append_word(GtkTextIter *iter, const gchar *wn_word, gboolean highlight,
            gboolean is_first)
{
    GtkTextBuffer *buffer;
    gchar *word;

    buffer = gtk_text_iter_get_buffer(iter);
    word = g_strdup(wn_word);
    g_strdelimit(word, "_", ' ');

    if (!is_first) {
        gtk_text_buffer_insert(buffer, iter, ", ", -1);
    }

    if (highlight) {
        gtk_text_buffer_insert_with_tags_by_name(buffer, iter, word, -1,
                                                 "highlighted", NULL);
    }
    else {
        gtk_text_buffer_insert(buffer, iter, word, -1);
    }
}

static void
append_definition(GtkTextIter *iter, const gchar *wn_defn)
{
    GtkTextBuffer *buffer;
    GString *text;
    gchar *example;
    const gchar *em_dash = " \xe2\x80\x94 ";

    buffer = gtk_text_iter_get_buffer(iter);
    text = g_string_new(wn_defn);
    if (text->len)
    {
        gtk_text_buffer_insert(buffer, iter, em_dash, -1);

        /* removing outer parentheses, if any */
        if (text->str[0] == '(' && text->str[text->len-1] == ')') {
            g_string_erase(text, 0, 1);
            g_string_truncate(text, text->len -1);
        }

        example = index(text->str, ';');
        if (example) {
            gtk_text_buffer_insert(buffer, iter, text->str,
                                   example-text->str);
            gtk_text_buffer_insert_with_tags_by_name(buffer, iter, ++example,
                                                     -1, "example", NULL);
        }
        else {
            gtk_text_buffer_insert(buffer, iter, text->str, -1);
        }
    }
    gtk_text_buffer_insert(buffer, iter, "\n", -1);
    g_string_free(text, TRUE);
}

static void
append_separator(GtkTextIter *iter)
{
    GtkTextBuffer *buffer;

    buffer = gtk_text_iter_get_buffer(iter);
    gtk_text_buffer_insert(buffer, iter, "\n", -1);
}

static void
append_part_of_speech(GtkTextIter *iter, SynsetPtr synset)
{
    SynsetPtr curset;
    int word;
    gboolean highlight;

    append_header(iter, partnames[*synset->ppos]);
    for (curset = synset; curset; curset = curset->nextss)
    {
        for (word = 0; word < curset->wcount; ++word) {
            highlight = (curset->whichword == word + 1);
            append_word(iter, curset->words[word], highlight, word == 0);
        }
        append_definition(iter, curset->defn);
    }
}

static void
on_lookup_word(GtkWidget *sender, GwbWindow *window)
{
    gchar *keyword;
    GtkTextIter iter;
    int pos;
    SynsetPtr synset;
    gboolean is_first;

    keyword = g_strdup(gtk_entry_get_text(GTK_ENTRY(window->entry)));
    g_strstrip(keyword);
    set_window_title(window, keyword);
    g_strdelimit(keyword, " ", '_'); /* WordNet likes underscores. */

    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(window->text_buffer), "", -1);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(window->text_buffer), &iter);

    for (pos = 1, is_first=TRUE; pos <= NUMPARTS; ++pos)
    {
        synset = findtheinfo_ds(keyword, pos, MAXSEARCH, ALLSENSES);
        if (synset)
        {
            if (is_first) {
                is_first = FALSE;
            }
            else {
                append_separator(&iter);
            }
            append_part_of_speech(&iter, synset);
        }
        free_syns(synset);
    }

    g_free(keyword);
}

static void
on_quit(GtkAction *action, GwbWindow *window)
{
    gtk_widget_destroy(GTK_WIDGET(window));
}

static void
on_select_entry(GtkAction *action, GwbWindow *window)
{
    gtk_widget_grab_focus(window->entry);
}

static void
make_menu_bar(GwbWindow *w)
{
    GError *error = NULL;
    GtkAccelGroup *accel_group;

    static const gchar *ui = 
"<ui>                                   \
  <menubar name='Menu'>                 \
    <menu action='File'>                \
      <menuitem action='Quit' />        \
    </menu>                             \
    <menu action='Edit'>                \
      <menuitem action='SelectEntry' /> \
    </menu>                             \
  </menubar>                            \
</ui>";

    static const GtkActionEntry actions[] = {
        {"File", NULL, "_File"},
        {"Quit", NULL, "_Quit", "<control>Q", NULL, G_CALLBACK(on_quit)},

        {"Edit", NULL, "_Edit"},
        {"SelectEntry", NULL, "_Select lookup word", "<control>L", NULL,
         G_CALLBACK(on_select_entry)}
    };
    w->action_group = gtk_action_group_new("Actions");
    gtk_action_group_add_actions(w->action_group, actions,
                                 G_N_ELEMENTS(actions), w);

    w->ui_manager = gtk_ui_manager_new();
    gtk_ui_manager_insert_action_group(w->ui_manager, w->action_group, 0);
    accel_group = gtk_ui_manager_get_accel_group(w->ui_manager);
    gtk_window_add_accel_group(GTK_WINDOW(w), accel_group);
    if (gtk_ui_manager_add_ui_from_string(w->ui_manager, ui, -1, &error))
    {
        w->menu_bar = gtk_ui_manager_get_widget(w->ui_manager, "/Menu");
    }
    else
    {
        g_critical("Can't create main menu: %s", error->message);
        g_error_free(error);
        w->menu_bar = gtk_menu_bar_new();
    }
}

static void
make_widgets(GwbWindow *w)
{
    w->entry = gtk_entry_new();
    gtk_entry_set_activates_default(GTK_ENTRY(w->entry), TRUE);

    w->lookup_btn = gtk_button_new_with_label(_("Look up:"));
    gtk_button_set_relief(GTK_BUTTON(w->lookup_btn), GTK_RELIEF_NONE);
    gtk_widget_set_can_default(w->lookup_btn, TRUE);
    g_signal_connect(w->lookup_btn, "clicked", G_CALLBACK(on_lookup_word), w);

    w->text_view = gtk_text_view_new();
    w->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w->text_view));
    make_text_tags(GTK_TEXT_BUFFER(w->text_buffer));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(w->text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(w->text_view), GTK_WRAP_WORD);
    gtk_text_view_set_indent(GTK_TEXT_VIEW(w->text_view), -DEFAULT_INTEND);
    gtk_text_view_set_pixels_below_lines(GTK_TEXT_VIEW(w->text_view),
                                         DEFAULT_LINES_INTERVAL);
    /* using default margins to make TextView inner padding */
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(w->text_view), DEFAULT_SPACING);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(w->text_view), DEFAULT_SPACING);
}

static void
make_containers(GwbWindow *window)
{
    GtkWidget *entry_box, *main_box, *scroll_box, *window_box;

    entry_box = gtk_hbox_new(FALSE, DEFAULT_SPACING);
    gtk_box_pack_start(GTK_BOX(entry_box), window->lookup_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), window->entry, TRUE, TRUE, 0);

    scroll_box = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_box),
                                        GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(scroll_box), window->text_view);

    main_box = gtk_vbox_new(FALSE, DEFAULT_SPACING);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), DEFAULT_SPACING);
    gtk_box_pack_start(GTK_BOX(main_box), entry_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), scroll_box, TRUE, TRUE, 0);

    window_box = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(window_box), window->menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(window_box), main_box, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), window_box);
    gtk_widget_show_all(window_box);
}

static void
gwb_window_dispose(GObject *obj)
{
    GwbWindow *window = GWB_WINDOW(obj);

    if (window->ui_manager) {
        g_object_unref(window->ui_manager);
        window->ui_manager = NULL;
    }
    if (window->action_group) {
        g_object_unref(window->action_group);
        window->action_group = NULL;
    }

    G_OBJECT_CLASS(gwb_window_parent_class)->dispose(obj);
}

static void
gwb_window_init(GwbWindow *self)
{
    set_window_title(self, NULL);
    make_menu_bar(self);
    make_widgets(self);
    make_containers(self);

    gtk_widget_grab_default(self->lookup_btn);
    gtk_widget_grab_focus(self->entry);
}

static void
gwb_window_class_init(GwbWindowClass *klass)
{
    G_OBJECT_CLASS(klass)->dispose = gwb_window_dispose;
}

GtkWidget*
gwb_window_new()
{
    return g_object_new(GWB_TYPE_WINDOW, NULL);
}
