#include "robot_vision.h"

GtkWidget *robot_vision_widget = NULL;
IplImage *robot_vision_image = NULL;
GdkPixbuf *robot_vision_pixbuf = NULL;

void RobotVision_initialize(GtkWidget *widget)
{
    robot_vision_widget = widget;
}

void RobotVision_setImage(IplImage *image)
{
    robot_vision_image = cvCloneImage(image);
    robot_vision_pixbuf = gdk_pixbuf_new_from_data((guchar*) robot_vision_image->imageData,
                          GDK_COLORSPACE_RGB, FALSE, robot_vision_image->depth, robot_vision_image->width,
                          robot_vision_image->height, (robot_vision_image->widthStep),
                          NULL, NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(robot_vision_image), robot_vision_pixbuf);
}

void RobotVision_finalize(void)
{
    g_object_unref(robot_vision_pixbuf);
    cvReleaseImage(&robot_vision_image);
}


