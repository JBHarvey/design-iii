#include <gtk/gtk.h>
#include "ui_builder.h"
#include "world_vision.h"

/* Constants */

const gchar *UI_RESOURCE_PATH = "/d3/station-resources/station.ui";
const int VIDEO_FEED_REFRESH_RATE_IN_MS = 50; // 20 FPS

/* Global variables */

gint timer_tag;
GtkWidget *logger = NULL;

/* Event callbacks */

void ui_window_destroy_event_callback(GtkWidget *widget, gpointer data)
{
    g_source_remove(timer_tag);
    g_object_unref(widget);
    gtk_main_quit();
}

static gboolean time_handler(GtkWidget *widget)
{
    gtk_widget_queue_draw(GTK_WIDGET(widget));

    return TRUE;
}

/* Main thread */

int main(int argc, char *argv[])
{
    GThread* world_vision_worker_thread = NULL;
    GtkWidget *ui_window = NULL;

    gtk_init(&argc, &argv);

    ui_window = build_ui_and_return_top_level_window(UI_RESOURCE_PATH);

    /* Handles re-drawing in UI */
    g_object_ref(ui_window);
    timer_tag = g_timeout_add(VIDEO_FEED_REFRESH_RATE_IN_MS, (GSourceFunc) time_handler, (gpointer) ui_window);

    set_main_loop_status_running();

    /* Starts worker threads */
    world_vision_worker_thread = g_thread_new("world_camera_feeder", prepare_image_from_world_camera_for_drawing, NULL);

    gtk_window_fullscreen(GTK_WINDOW(ui_window));

    gtk_widget_show_all(GTK_WIDGET(ui_window));

    gtk_main();

    set_main_loop_status_terminated();

    g_thread_join(world_vision_worker_thread);

    return 0;
}

