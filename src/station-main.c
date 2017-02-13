#include <gtk/gtk.h>
#include "opencv2/highgui/highgui_c.h"

gint t = 0;
GdkPixbuf* pix;
IplImage* frame;

gboolean expose_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    GdkWindow *window = gtk_widget_get_window(widget);
    GdkDrawingContext *drawing_context;
    cairo_t *cairo_context;
    CvCapture *cv_cap = cvCreateCameraCapture(0);

    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_WIDTH, 1600);
    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_HEIGHT, 1200);

    frame = cvQueryFrame(cv_cap);
    pix = gdk_pixbuf_new_from_data((guchar*) frame->imageData,
                                   GDK_COLORSPACE_RGB, FALSE, frame->depth, frame->width,
                                   frame->height, (frame->widthStep), NULL, NULL);

    cairo_surface_t *cairo_surface = gdk_cairo_surface_create_from_pixbuf(pix, 0, window);
    cairo_region_t *cairo_region = gdk_cairo_region_create_from_surface(cairo_surface);

    drawing_context = gdk_window_begin_draw_frame(window,
                      cairo_region);
    cairo_context = gdk_drawing_context_get_cairo_context(drawing_context);

    gdk_cairo_set_source_pixbuf(cairo_context, pix, 0, 0);

    cairo_paint(cairo_context);

    gdk_window_end_draw_frame(window, drawing_context);

    cairo_region_destroy(cairo_region);
    cairo_surface_destroy(cairo_surface);
    cvReleaseCapture(&cv_cap);

    return TRUE;
}

static gboolean time_handler(GtkWidget *widget)
{
    gtk_widget_queue_draw(GTK_WIDGET(widget));

    if(t < 20) {
        t++;
    } else if(t >= 20) {
        t = 0;
    }

    printf("T_VAL: %d\n", t);
    return TRUE;
}

extern int main(int argc, char *argv[])
{
    GtkBuilder *builder;
    GObject *ui_window;
    GObject *world_camera_canvas;
    GObject *world_camera;

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_resource("/d3/station-resources/station.ui");

    ui_window = gtk_builder_get_object(builder, "ui_window");
    world_camera_canvas = gtk_builder_get_object(builder, "world_camera_canvas");
    world_camera = gtk_builder_get_object(builder, "world_camera");

    gtk_window_fullscreen(GTK_WINDOW(ui_window));

    gtk_builder_connect_signals(builder, NULL);
    g_signal_connect(world_camera, "draw", G_CALLBACK(expose_event_callback), NULL);

    g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) world_camera);

    g_object_unref(builder);

    gtk_widget_show(GTK_WIDGET(world_camera_canvas));
    gtk_widget_show(GTK_WIDGET(world_camera));
    gtk_widget_show(GTK_WIDGET(ui_window));
    gtk_main();

    return 0;
}
