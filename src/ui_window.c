#include <gtk/gtk.h>

#include "ui_app.h"
#include "ui_window.h"

struct _UiWindow {
    GtkApplicationWindow parent;
};

G_DEFINE_TYPE(UiWindow, ui_window, GTK_TYPE_APPLICATION_WINDOW);

static void ui_window_init(UiWindow *window)
{
    gtk_widget_init_template(GTK_WIDGET(window));
}

static void ui_window_class_init(UiWindowClass *class)
{
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
            "/app.ui");
}

UiWindow *ui_window_new(UiApp *app)
{
    return g_object_new(UI_WINDOW_TYPE, "application", app, NULL);
}

void ui_window_open(UiWindow *window,
                    GFile    *file)
{
}

