#ifndef __WORLD_VISION
#define __WORLD_VISION

#include <gtk/gtk.h>

/* Intended to be a worker thread that takes camera capture continuously and feed a pixel buffer with them.
 * data: not used (g_thread requests it)
 * return: NULL (g_thread requests it)*/
gpointer world_camera_feeder(gpointer data);

/* Callback used by draw event on the world camera's GtkDrawingArea that process a redraw of the area with 
 * the world camera feed's pixel buffer.
 * SHOULD NOT BE CALLED IMPLICITLY
 * widget: the widget that has generated the draw event. (handled by g_signal)
 * event: the draw event object. (handled by g_signal)
 * data: not used. (handled by g_signal)
 * return: TRUE (handled by g_signal)*/
gboolean world_camera_draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);

gboolean world_camera_calibration_clicked_event_callback(GtkWidget *widget, gpointer data);

void set_main_loop_status_running(void);

void set_main_loop_status_terminated(void);

GMutex mutex;

#endif // __WORLD_VISION
