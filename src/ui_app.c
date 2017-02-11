#include <gtk/gtk.h>

#include "ui_app.h"
#include "ui_window.h"

struct _UiApp {
    GtkApplication parent;
};

G_DEFINE_TYPE(UiApp, ui_app, GTK_TYPE_APPLICATION);

static void ui_app_init(UiApp *app)
{
}

static void ui_app_activate(GApplication *app)
{
    UiWindow *window;

    window = ui_window_new(UI_APP(app));
    gtk_window_present(GTK_WINDOW(window));
}

static void ui_app_open(GApplication  *app,
                        GFile        **files,
                        gint           n_files,
                        const gchar   *hint)
{
    GList *windows;
    UiWindow *window;
    int i;

    windows = gtk_application_get_windows(GTK_APPLICATION(app));

    if(windows) {
        window = UI_WINDOW(windows->data);
    } else {
        window = ui_window_new(UI_APP(app));
    }

    for(i = 0; i < n_files; i++) {
        ui_window_open(window, files[i]);
    }

    gtk_window_present(GTK_WINDOW(window));
}

static void ui_app_class_init(UiAppClass *class)
{
    G_APPLICATION_CLASS(class)->activate = ui_app_activate;
    G_APPLICATION_CLASS(class)->open = ui_app_open;
}

UiApp *ui_app_new(void)
{
    return g_object_new(UI_APP_TYPE,
                        "application-id", "d3.station",
                        "flags", G_APPLICATION_HANDLES_OPEN,
                        NULL);
}

