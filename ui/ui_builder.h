#ifndef __UI_BUILDER
#define __UI_BUILDER

#include <gtk/gtk.h>

/* The resource_path must be appended by the project prefix. */
GtkWidget* build_ui_and_return_top_level_window(const gchar* resource_path);

#endif // __UI_BUILDER

