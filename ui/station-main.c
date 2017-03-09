#include <gtk/gtk.h>
#include "ui_builder.h"
#include "logger.h"
#include "world_vision.h"

/* Constants */

const gchar *UI_RESOURCE_PATH = "/d3/station-resources/station.ui";
const int VIDEO_FEED_REFRESH_RATE_IN_MS = 50; // 20 FPS

/* Global variables */

gint timer_tag;

void uiWindowDestroyEventCallback(GtkWidget *widget, gpointer data)
{
    g_source_remove(timer_tag);
    g_object_unref(widget);
    gtk_main_quit();
}

static gboolean timeHandler(GtkWidget *widget)
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

    ui_window = buildUiAndReturnTopLevelWindow(UI_RESOURCE_PATH);

    /* Handles re-drawing in UI */
    g_object_ref(ui_window);
    timer_tag = g_timeout_add(VIDEO_FEED_REFRESH_RATE_IN_MS, (GSourceFunc) timeHandler, (gpointer) ui_window);

    WorldVision_setMainLoopStatusRunning();

    /* Starts worker threads */
    world_vision_worker_thread = g_thread_new("world_camera_feeder", WorldVision_prepareImageFromWorldCameraForDrawing,
                                 NULL);

    gtk_window_fullscreen(GTK_WINDOW(ui_window));

    gtk_widget_show_all(GTK_WIDGET(ui_window));

    Logger_startMessageSectionAndAppend("Hit the \"Calibrate\" button and follow the instructions.");

    gtk_main();

    WorldVision_setMainLoopStatusTerminated();

    g_thread_join(world_vision_worker_thread);

    return 0;
}

