#ifndef __UI_EVENT
#define __UI_EVENT

#include <gtk/gtk.h>

/* The pixel buffer must be valid. */
gboolean drawEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer pixbuf);

#endif // __UI_EVENT

