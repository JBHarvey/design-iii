#include "world_vision.h"
#include "world_vision_calibration.h"
#include "ui_event.h"
#include "opencv2/videoio/videoio_c.h"
#include "opencv2/calib3d/calib3d_c.h"

/* Flags definitions */

enum thread_status {TERMINATED, RUNNING};
enum camera_status {UNCALIBRATED, CALIBRATED};

/* Constants */

const int WORLD_CAMERA_WIDTH = 1280;
const int WORLD_CAMERA_HEIGHT = 720;

/* Global variables */

GMutex world_camera_feeder_mutex;

enum thread_status main_loop_status;
enum camera_status world_camera_status = UNCALIBRATED;
struct camera_intrinsics *world_camera_intrinsics = NULL;
GdkPixbuf *world_camera_pixbuf = NULL;

/* Event callbacks */

gboolean world_camera_draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    gboolean status = FALSE;

    if(G_IS_OBJECT(world_camera_pixbuf)) {
        g_mutex_lock(&world_camera_feeder_mutex);
        status = draw_event_callback(widget, event, world_camera_pixbuf);
        g_mutex_unlock(&world_camera_feeder_mutex);
    }

    return status;
}

gboolean world_camera_calibration_clicked_event_callback(GtkWidget *widget, gpointer data)
{
    if(initialize_camera_matrix_and_distortion_coefficients_from_file(widget, world_camera_intrinsics)) {
        gtk_widget_hide(GTK_WIDGET(data));
        //CvPoint3D64f green_square_model_points[4] = {cvPoint3D64f(0, 0, 0), cvPoint3D64f(0, 660, 0), cvPoint3D64f(660, 660, 0), cvPoint3D64f(660, 0, 0)};
    }

    return TRUE;
}

/* Worker thread functions */

gpointer world_camera_feeder(gpointer data)
{
    IplImage* frame = NULL;
    IplImage* frame_BGR = NULL;
    IplImage* frame_BGR_corrected = NULL;

    g_mutex_lock(&world_camera_feeder_mutex);
    world_camera_intrinsics = (struct camera_intrinsics *) malloc(sizeof(struct camera_intrinsics));
    world_camera_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, IPL_DEPTH_8U, WORLD_CAMERA_WIDTH,
                                         WORLD_CAMERA_HEIGHT);
    g_mutex_unlock(&world_camera_feeder_mutex);

    CvCapture *world_camera_feed = NULL;

    /* Find camera. 0 is for the embedded webcam. */
    for(int i = 1; i < 100; i++) {
        world_camera_feed = cvCreateCameraCapture(i);

        if(world_camera_feed) {
            break;
        }
    }

    if(!world_camera_feed) {
        world_camera_feed = cvCreateCameraCapture(0);
    }

    cvSetCaptureProperty(world_camera_feed, CV_CAP_PROP_FRAME_WIDTH, WORLD_CAMERA_WIDTH);
    cvSetCaptureProperty(world_camera_feed, CV_CAP_PROP_FRAME_HEIGHT, WORLD_CAMERA_HEIGHT);

    while(TRUE) {
        g_mutex_lock(&world_camera_feeder_mutex);

        if(main_loop_status == TERMINATED) {
            g_object_unref(world_camera_pixbuf);
            g_mutex_unlock(&world_camera_feeder_mutex);
            cvReleaseCapture(&world_camera_feed);

            if(world_camera_status != UNCALIBRATED) {
                cvReleaseMat(&(world_camera_intrinsics->camera_matrix));
                cvReleaseMat(&(world_camera_intrinsics->distortion_coefficients));
            }

            free(world_camera_intrinsics);
            g_thread_exit(0);
        } else {
            g_mutex_unlock(&world_camera_feeder_mutex);
        }

        frame_BGR = cvQueryFrame(world_camera_feed);
        frame = cvCloneImage(frame_BGR);

        if(world_camera_status == CALIBRATED) {
            CvMat *optimal_camera_matrix = cvCreateMat(3, 3, CV_64F);
            cvGetOptimalNewCameraMatrix(world_camera_intrinsics->camera_matrix, world_camera_intrinsics->distortion_coefficients,
                                        cvSize(WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT),
                                        1.0, optimal_camera_matrix,
                                        cvSize(WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT), 0, 0);
            frame_BGR_corrected = cvCloneImage(frame_BGR);
            cvUndistort2(frame_BGR, frame_BGR_corrected, world_camera_intrinsics->camera_matrix,
                         world_camera_intrinsics->distortion_coefficients, optimal_camera_matrix);
            cvReleaseMat(&optimal_camera_matrix);
        } else if(world_camera_status == UNCALIBRATED) {
            frame_BGR_corrected = cvCloneImage(frame_BGR);
        }

        cvCvtColor((CvArr*) frame_BGR_corrected, frame, CV_BGR2RGB);

        g_mutex_lock(&world_camera_feeder_mutex);
        g_object_unref(world_camera_pixbuf);

        world_camera_pixbuf = gdk_pixbuf_new_from_data((guchar*) frame->imageData,
                              GDK_COLORSPACE_RGB, FALSE, frame->depth, frame->width,
                              frame->height, (frame->widthStep), NULL, NULL);

        g_mutex_unlock(&world_camera_feeder_mutex);

        cvReleaseImage(&frame_BGR_corrected);
        cvReleaseImage(&frame);
    }

    return NULL;
}

/* Public functions */

void set_main_loop_status_running(void)
{
    g_mutex_lock(&world_camera_feeder_mutex);
    main_loop_status = RUNNING;
    g_mutex_unlock(&world_camera_feeder_mutex);
}

void set_main_loop_status_terminated(void)
{
    g_mutex_lock(&world_camera_feeder_mutex);
    main_loop_status = TERMINATED;
    g_mutex_unlock(&world_camera_feeder_mutex);
}

void set_world_camera_status_calibrated(void)
{
    g_mutex_lock(&world_camera_feeder_mutex);
    world_camera_status = CALIBRATED;
    g_mutex_unlock(&world_camera_feeder_mutex);
}
