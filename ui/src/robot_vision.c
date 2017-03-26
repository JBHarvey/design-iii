#include "robot_vision.h"

GtkWidget *robot_vision_widget = NULL;
IplImage *robot_vision_image = NULL;
GdkPixbuf *robot_vision_pixbuf = NULL;

const int ROBOT_VISION_IMAGE_SIZE = 400;

void RobotVision_initialize(GtkWidget *widget)
{
    robot_vision_widget = widget;
}

void RobotVision_setImage(IplImage *image)
{
    robot_vision_image = cvCreateImage(cvSize(ROBOT_VISION_IMAGE_SIZE, ROBOT_VISION_IMAGE_SIZE), IPL_DEPTH_8U, 3);
    IplImage *temporary_image = cvCloneImage(image);
    cvResize(temporary_image, robot_vision_image, CV_INTER_AREA);
    robot_vision_pixbuf = gdk_pixbuf_new_from_data((guchar*) robot_vision_image->imageData,
                          GDK_COLORSPACE_RGB, FALSE, robot_vision_image->depth, robot_vision_image->width,
                          robot_vision_image->height, (robot_vision_image->widthStep),
                          NULL, NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(robot_vision_widget), robot_vision_pixbuf);
    cvReleaseImage(&temporary_image);
}

void RobotVision_finalize(void)
{
    cvReleaseImage(&robot_vision_image);
}


