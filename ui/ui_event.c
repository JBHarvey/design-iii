#include "ui_event.h"

gboolean draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer pixbuf)
{
    GdkPixbuf* source_pixbuf = GDK_PIXBUF(pixbuf);

    if(source_pixbuf != NULL) {

        GdkWindow *window = gtk_widget_get_window(widget);
        cairo_surface_t *cairo_surface = gdk_cairo_surface_create_from_pixbuf(source_pixbuf, 0, window);
        cairo_region_t *cairo_region = gdk_cairo_region_create_from_surface(cairo_surface);
        gfloat widget_width = gtk_widget_get_allocated_width(widget);
        gfloat widget_height = gtk_widget_get_allocated_height(widget);
        gfloat image_width = cairo_image_surface_get_width(cairo_surface);
        gfloat image_height = cairo_image_surface_get_height(cairo_surface);
        gfloat x_scaling = widget_width / image_width;
        gfloat y_scaling = widget_height / image_height;

        GdkDrawingContext *drawing_context = gdk_window_begin_draw_frame(window, cairo_region);

        cairo_t *cairo_context = gdk_drawing_context_get_cairo_context(drawing_context);

        cairo_scale(cairo_context, x_scaling, y_scaling);
        cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
        cairo_paint(cairo_context);

        gdk_window_end_draw_frame(window, drawing_context);

        cairo_region_destroy(cairo_region);
        cairo_surface_destroy(cairo_surface);
    }

    return TRUE;
}
