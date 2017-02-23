#include <gtk/gtk.h>
#include "ui_builder.h"
#include "world_vision.h"

const gchar *UI_RESOURCE_PATH = "/d3/station-resources/station.ui";
const int VIDEO_FEED_REFRESH_RATE_IN_MS = 50; // 20 FPS

static gboolean time_handler(GtkWidget *widget)
{
    gtk_widget_queue_draw(GTK_WIDGET(widget));

    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkWidget *ui_window;

    gtk_init(&argc, &argv);

    ui_window = build_ui(UI_RESOURCE_PATH);

    /* Handles re-drawing in UI */
    g_timeout_add(VIDEO_FEED_REFRESH_RATE_IN_MS, (GSourceFunc) time_handler, (gpointer) ui_window);

    /* Starts worker threads */
    g_thread_new("world_camera_feeder", world_camera_feeder, NULL);

    // gtk_window_fullscreen(GTK_WINDOW(ui_window));
    gtk_widget_show_all(GTK_WIDGET(ui_window));

    gtk_main();

    return 0;
}
