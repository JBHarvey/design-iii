#include "world_vision.h"
#include "world_vision_calibration.h"
#include "ui_event.h"
#include "opencv2/videoio/videoio_c.h"
#include "opencv2/calib3d/calib3d_c.h"

/* Flags definitions */

enum thread_status {TERMINATED, RUNNING};
enum camera_status {UNCALIBRATED, CALIBRATED};

/* Constants */

const int WORLD_CAMERA_WIDTH = 1600;
const int WORLD_CAMERA_HEIGHT = 1200;
const int WORLD_CAMERA_DEVICE_ID = 1;
const int NUMBER_OF_ROW_OF_CAMERA_MATRIX = 3;
const int NUMBER_OF_COLUMN_OF_CAMERA_MATRIX = 3;

/* Type definitions */

struct camera_capture {
    CvCapture *camera_capture_feed;
};

/* Global variables */

GMutex world_camera_feeder_mutex;

enum thread_status main_loop_status;
enum camera_status world_camera_status = UNCALIBRATED;
struct camera *world_camera = NULL;
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
    if(initialize_camera_matrix_and_distortion_coefficients_from_file(widget, world_camera->camera_intrinsics)) {
        gtk_widget_hide(GTK_WIDGET(data));
        gather_user_points_for_camera_pose_computation(0);
        compute_camera_pose_from_user_points(world_camera);
    }

    return TRUE;
}

static void initialize_world_camera(void)
{
    world_camera = malloc(sizeof(struct camera));
    world_camera->camera_intrinsics = malloc(sizeof(struct camera_intrinsics));
    world_camera->camera_extrinsics = malloc(sizeof(struct camera_extrinsics));
}

static void initialize_camera_capture(struct camera_capture *output_camera_capture, int capture_width,
                                      int capture_height)
{
    output_camera_capture->camera_capture_feed = cvCreateCameraCapture(WORLD_CAMERA_DEVICE_ID);

    if(!output_camera_capture->camera_capture_feed) {
        output_camera_capture->camera_capture_feed  = cvCreateCameraCapture(CV_CAP_ANY);
    }

    cvSetCaptureProperty(output_camera_capture->camera_capture_feed, CV_CAP_PROP_FRAME_WIDTH, capture_width);
    cvSetCaptureProperty(output_camera_capture->camera_capture_feed, CV_CAP_PROP_FRAME_HEIGHT, capture_height);
}

static void release_camera_capture(struct camera_capture *input_camera_capture)
{
    cvReleaseCapture(&(input_camera_capture->camera_capture_feed));
    free(input_camera_capture);
}

static void release_camera(struct camera *input_camera, enum camera_status input_camera_status)
{
    if(input_camera_status == CALIBRATED) {
        cvReleaseMat(&(input_camera->camera_intrinsics->camera_matrix));
        cvReleaseMat(&(input_camera->camera_intrinsics->distortion_coefficients));
        cvReleaseMat(&(input_camera->camera_extrinsics->rotation_vector));
        cvReleaseMat(&(input_camera->camera_extrinsics->translation_vector));
    }

    free(input_camera->camera_intrinsics);
    free(input_camera->camera_extrinsics);
    free(input_camera);
}

static void clean_exit_if_main_loop_terminated(struct camera_capture *world_camera_capture)
{
    g_mutex_lock(&world_camera_feeder_mutex);

    if(main_loop_status == TERMINATED) {
        g_object_unref(world_camera_pixbuf);
        g_mutex_unlock(&world_camera_feeder_mutex);
        release_camera_capture(world_camera_capture);
        release_camera(world_camera, world_camera_status);
        g_thread_exit(0);
    } else {
        g_mutex_unlock(&world_camera_feeder_mutex);
    }
}

static void undistort_camera_capture(IplImage *input_frame, IplImage *output_undistorted_frame)
{
    CvMat *optimal_camera_matrix = cvCreateMat(NUMBER_OF_ROW_OF_CAMERA_MATRIX, NUMBER_OF_COLUMN_OF_CAMERA_MATRIX, CV_64F);
    cvGetOptimalNewCameraMatrix(world_camera->camera_intrinsics->camera_matrix,
                                world_camera->camera_intrinsics->distortion_coefficients,
                                cvSize(WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT),
                                1, optimal_camera_matrix,
                                cvSize(WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT), 0, 0);
    cvUndistort2(input_frame, output_undistorted_frame, world_camera->camera_intrinsics->camera_matrix,
                 world_camera->camera_intrinsics->distortion_coefficients, optimal_camera_matrix);
    cvReleaseMat(&optimal_camera_matrix);
}

/* Worker thread */

gpointer prepare_image_from_world_camera_for_drawing(gpointer data)
{
    IplImage* frame = NULL;
    IplImage* frame_BGR = NULL;
    IplImage* frame_BGR_corrected = NULL;

    initialize_world_camera();

    g_mutex_lock(&world_camera_feeder_mutex);
    world_camera_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, IPL_DEPTH_8U, WORLD_CAMERA_WIDTH,
                                         WORLD_CAMERA_HEIGHT);
    g_mutex_unlock(&world_camera_feeder_mutex);

    struct camera_capture *world_camera_capture = malloc(sizeof(struct camera_capture));

    initialize_camera_capture(world_camera_capture, WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT);

    while(TRUE) {

        clean_exit_if_main_loop_terminated(world_camera_capture);

        frame_BGR = cvQueryFrame(world_camera_capture->camera_capture_feed);
        frame_BGR_corrected = cvCloneImage(frame_BGR);
        frame = cvCloneImage(frame_BGR);

        if(world_camera_status == CALIBRATED) {
            undistort_camera_capture(frame_BGR, frame_BGR_corrected);
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

