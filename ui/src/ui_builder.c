#include "ui_builder.h"
#include "resources.c"
#include "logger.h"

GtkWidget* buildUiAndReturnTopLevelWindow(const gchar* resource_path)
{
    GtkBuilder *builder = gtk_builder_new_from_resource(resource_path);

    /* Connects all callback defined in the UI resource file
     * to the function with the same name thanks to the --export-dynamic
     * option of lgmodule-2.0
    */
    gtk_builder_connect_signals(builder, NULL);
    GObject *ui_window = gtk_builder_get_object(builder, "ui_window");
    g_object_set(gtk_widget_get_settings(GTK_WIDGET(ui_window)), "gtk-tooltip-timeout", 0, NULL);
    Logger_initialize(GTK_WIDGET(gtk_builder_get_object(builder, "logger")));
    g_object_unref(builder);

    return GTK_WIDGET(ui_window);
}
