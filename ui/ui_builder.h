#ifndef __UI_BUILDER
#define __UI_BUILDER

#include <gtk/gtk.h>

/* Build GUI from resource file.
 * resource_path: the path to the resource file appended by the project prefix.
 * return: a pointer to the top level widget (the application window) */
GtkWidget* build_ui(const gchar* resource_path);

#endif // __UI_BUILDER

