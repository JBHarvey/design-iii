#ifndef __UI_EVENT
#define __UI_EVENT

#include <gtk/gtk.h>

/* Process the draw event: it draws the passed pixel buffer (pixbuf parameter) onto the passed widget's windows. 
 * The pixel buffer must be valid. */
gboolean draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer pixbuf);

#endif // __UI_EVENT

