#ifndef __UI_EVENT
#define __UI_EVENT

#include <gtk/gtk.h>

/* Process the draw event: it draws the passed pixel buffer (pixbuf parameter) onto the passed widget's windows.
 * widget: the widget that has generated the draw event. (handled by g_signal)
 * event: the draw event object. (handled by g_signal)
 * pixbuf: the pixel buffer to write onto the widget. (handled by g_signal) 
 * return: TRUE (handled by g_signal) */
gboolean draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer pixbuf);

#endif // __UI_EVENT

