#ifndef __GWB_WINDOW_H__
#define __GWB_WINDOW_H__

#include <gtk/gtk.h>


#define GWB_TYPE_WINDOW (gwb_window_get_type())
#define GWB_WINDOW(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GWB_TYPE_WINDOW, GwbWindow))
#define GWB_IS_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GWB_TYPE_WINDOW))


typedef struct _GwbWindow GwbWindow;
typedef struct _GwbWindowClass GwbWindowClass;

struct _GwbWindow
{
    GtkWindow parent_instance;

    GtkWidget *entry;
    GtkWidget *lookup_btn;
    GtkWidget *text_view;

    GtkTextBuffer *text_buffer;
};

struct _GwbWindowClass
{
    GtkWindowClass parent_class;
};

GType gwb_window_get_type();
GtkWidget* gwb_window_new();


#endif /* __GWB_WINDOW_H__ */
