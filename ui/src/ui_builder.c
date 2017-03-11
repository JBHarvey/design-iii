#include "ui_builder.h"

GtkWidget* build_ui(const gchar* resource_path)
{
    GtkBuilder *builder = gtk_builder_new_from_resource(resource_path);

    /* Connects all callback defined in the UI resource file
     * to the function with the same name thanks to the --export-dynamic
     * option of lgmodule-2.0
    */
    gtk_builder_connect_signals(builder, NULL);
    GObject *ui_window = gtk_builder_get_object(builder, "ui_window");
    g_object_unref(builder);

    return GTK_WIDGET(ui_window);
}
