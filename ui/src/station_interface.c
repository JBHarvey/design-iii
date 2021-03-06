#include <pthread.h>
#include "station_interface.h"
#include "ui_builder.h"
#include "logger.h"
#include "timer.h"
#include "robot_vision.h"
#include "world_vision.h"
#include "station_client.h"
#include "station_client_sender.h"

/* Constants */

const gchar *UI_RESOURCE_PATH = "/d3/station-resources/station.ui";
const int VIDEO_FEED_REFRESH_RATE_IN_MS = 50; // 20 FPS
const int ROBOT_SERVER_PORT = 35794;
const char *ROBOT_SERVER_IP = "10.42.0.1";
//const char *ROBOT_SERVER_IP = "127.0.0.1";
//const char *ROBOT_SERVER_IP = "10.248.223.248";

/* Global variables */

GMutex main_loop_status_mutex;
GMutex robot_connection_status_mutex;

enum ThreadStatus main_loop_status;
enum ConnectionStatus robot_connection_status;
gint timer_tag;
int is_first_cycle = 1;
struct StationClient *station_client = NULL;
GtkWidget *start_cycle_button_widget = NULL;

void debugManchesterEventCallback(GtkWidget *widget, gpointer data)
{
    StationClientSender_sendManchesterDebug(2, 4, 'W'); /* Possibilities 'N' 'E' 'S' 'W' */
}

void uiWindowDestroyEventCallback(GtkWidget *widget, gpointer data)
{
    Logger_finalize();
    Timer_release();
    RobotVision_finalize();
    g_source_remove(timer_tag);
    g_object_unref(widget);
    gtk_main_quit();
}

void startCycleClickedEventCallback(GtkWidget *widget, gpointer data)
{
    WorldVision_resetRobotPosition();
    StationClientSender_sendStartCycleCommand();

    if(!is_first_cycle) {
        WorldVision_recalibrateForMoving();
        Timer_resume();
    } else {
        Timer_start();
        is_first_cycle = 0;
    }

    Logger_startMessageSectionAndAppend("Cycle started!");
    StationInterface_deactivateStartCycleButton();
}

static gboolean timeHandler(GtkWidget *widget)
{
    gtk_widget_queue_draw(GTK_WIDGET(widget));
    Timer_redraw();

    return TRUE;
}

int StationInterface_isRunning(void)
{
    g_mutex_lock(&main_loop_status_mutex);
    enum ThreadStatus status = main_loop_status;
    g_mutex_unlock(&main_loop_status_mutex);

    return status == RUNNING;
}

void StationInterface_setRobotConnectionStatusOn(void)
{
    g_mutex_lock(&robot_connection_status_mutex);
    robot_connection_status = CONNECTED;
    g_mutex_unlock(&robot_connection_status_mutex);
}

int StationInterface_isConnectedToRobot(void)
{
    g_mutex_lock(&robot_connection_status_mutex);
    enum ConnectionStatus status = robot_connection_status;
    g_mutex_unlock(&robot_connection_status_mutex);

    return status == CONNECTED;
}

void StationInterface_initializeStartCycleButton(GtkWidget* widget)
{
    start_cycle_button_widget = widget;
}

void StationInterface_activateStartCycleButton(void)
{
    gtk_widget_set_sensitive(start_cycle_button_widget, TRUE);
}

void StationInterface_deactivateStartCycleButton(void)
{
    gtk_widget_set_sensitive(start_cycle_button_widget, FALSE);
}

/* Main thread */

void StationInterface_launch(int argc, char *argv[])
{
    GThread *world_vision_worker_thread = NULL;
    GtkWidget *ui_window = NULL;

    gtk_init(&argc, &argv);

    ui_window = UiBuilder_buildUiAndReturnTopLevelWindow(UI_RESOURCE_PATH);

    /* Handles re-drawing in UI */
    g_object_ref(ui_window);
    timer_tag = g_timeout_add(VIDEO_FEED_REFRESH_RATE_IN_MS, (GSourceFunc) timeHandler, (gpointer) ui_window);

    g_mutex_lock(&main_loop_status_mutex);
    main_loop_status = RUNNING;
    g_mutex_unlock(&main_loop_status_mutex);

    station_client = StationClient_new(ROBOT_SERVER_PORT, ROBOT_SERVER_IP);
    StationClient_init(station_client);

    /* Starts worker thread */
    world_vision_worker_thread = g_thread_new("world_camera_feeder",
                                 (GThreadFunc) WorldVision_prepareImageFromWorldCameraForDrawing,
                                 (gpointer) station_client);

    gtk_window_fullscreen(GTK_WINDOW(ui_window));
    gtk_widget_show_all(GTK_WIDGET(ui_window));

    gtk_main();

    g_mutex_lock(&main_loop_status_mutex);
    main_loop_status = TERMINATED;
    g_mutex_unlock(&main_loop_status_mutex);

    g_thread_join(world_vision_worker_thread);
    StationClient_delete(station_client);
}
