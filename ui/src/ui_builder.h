#ifndef __UI_BUILDER
#define __UI_BUILDER

#include <gtk/gtk.h>

/* The resource_path must be appended by the project prefix. */
GtkWidget* UiBuilder_buildUiAndReturnTopLevelWindow(const gchar* resource_path);

#endif // __UI_BUILDER

