#include "opencv2/videoio/videoio_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "world_vision.h"
#include "ui_event.h"

const int WORLD_CAMERA_WIDTH = 1600;
const int WORLD_CAMERA_HEIGHT = 1200;

GdkPixbuf* world_camera_pixbuf;

gpointer world_camera_feeder(gpointer data)
{
    CvCapture *cv_cap;
    IplImage* frame;
    IplImage* frame_BGR;

    /* Find camera. 0 is for the embedded webcam. */
    for(int i = 1; i < 100; i++) {
        cv_cap = cvCreateCameraCapture(i);

        if(cv_cap) {
            break;
        }
    }

    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_WIDTH, WORLD_CAMERA_WIDTH);
    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_HEIGHT, WORLD_CAMERA_HEIGHT);

    while(TRUE) {
        frame_BGR = cvQueryFrame(cv_cap);

        if(!frame) {
            frame = cvCreateImage(cvGetSize((CvArr*) frame_BGR), IPL_DEPTH_8U, 3);
        }

        cvCvtColor(frame_BGR, frame, CV_BGR2RGB);
        world_camera_pixbuf = gdk_pixbuf_new_from_data((guchar*) frame->imageData,
                              GDK_COLORSPACE_RGB, FALSE, frame->depth, frame->width,
                              frame->height, (frame->widthStep), NULL, NULL);
    }

    cvReleaseCapture(&cv_cap);
    cvReleaseImage(&frame_BGR);
    cvReleaseImage(&frame);

    return NULL;
}

gboolean world_camera_draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    return draw_event_callback(widget, event, world_camera_pixbuf);
}
