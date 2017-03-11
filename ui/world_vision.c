#include "world_vision.h"
#include "world_vision_calibration.h"
#include "ui_event.h"
#include "opencv2/videoio/videoio_c.h"
#include "opencv2/calib3d/calib3d_c.h"

/* Flags definitions */

enum ThreadStatus {TERMINATED, RUNNING};
enum CaptureMode {DISTORTED, UNDISTORTED};

/* Constants */

const int WORLD_CAMERA_WIDTH = 1600;
const int WORLD_CAMERA_HEIGHT = 1200;
const int WORLD_CAMERA_DEVICE_ID = 1;
const int NUMBER_OF_ROW_OF_CAMERA_MATRIX = 3;
const int NUMBER_OF_COLUMN_OF_CAMERA_MATRIX = 3;
const char FILE_PATH_OF_DEBUG_MODE_VIDEO_CAPTURE[] = "./camera_calibration/camera_3015_1/video_feed.avi";

/* Type definitions */

struct CameraCapture {
    CvCapture *camera_capture_feed;
};

/* Global variables */

GMutex world_camera_feeder_mutex;

enum ThreadStatus main_loop_status;
enum CaptureMode world_camera_capture_mode = DISTORTED;
struct Camera *world_camera = NULL;
GdkPixbuf *world_camera_pixbuf = NULL;

gboolean worldCameraDrawEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    gboolean status = FALSE;

    if(G_IS_OBJECT(world_camera_pixbuf)) {
        g_mutex_lock(&world_camera_feeder_mutex);
        status = drawEventCallback(widget, event, world_camera_pixbuf);
        g_mutex_unlock(&world_camera_feeder_mutex);
    }

    return status;
}

gboolean worldCameraCalibrationClickedEventCallback(GtkWidget *widget, gpointer data)
{
    if(WorldVisionCalibration_initializeCameraMatrixAndDistortionCoefficientsFromFile(widget,
            world_camera)) {
        gtk_widget_hide(GTK_WIDGET(data));
        WorldVisionCalibration_calibrate(world_camera);
    }

    return TRUE;
}

static void initializeWorldCamera(void)
{
    world_camera = malloc(sizeof(struct Camera));
    world_camera->camera_intrinsics = malloc(sizeof(struct CameraIntrinsics));
    world_camera->camera_extrinsics = malloc(sizeof(struct CameraExtrinsics));
    world_camera->camera_status = UNCALIBRATED;
}

static void initializeCameraCapture(struct CameraCapture *output_camera_capture, int capture_width,
                                    int capture_height)
{
    output_camera_capture->camera_capture_feed = cvCreateCameraCapture(WORLD_CAMERA_DEVICE_ID);

    if(!output_camera_capture->camera_capture_feed) {
        output_camera_capture->camera_capture_feed = cvCreateFileCapture(FILE_PATH_OF_DEBUG_MODE_VIDEO_CAPTURE);
    }

    cvSetCaptureProperty(output_camera_capture->camera_capture_feed, CV_CAP_PROP_FRAME_WIDTH, capture_width);
    cvSetCaptureProperty(output_camera_capture->camera_capture_feed, CV_CAP_PROP_FRAME_HEIGHT, capture_height);
}

static void releaseCameraCapture(struct CameraCapture *input_camera_capture)
{
    cvReleaseCapture(&(input_camera_capture->camera_capture_feed));
    free(input_camera_capture);
}

static void releaseCamera(struct Camera *input_camera)
{
    if(input_camera->camera_status == INTRINSICALLY_AND_EXTRINSICALLY_CALIBRATED ||
       input_camera->camera_status == FULLY_CALIBRATED) {
        cvReleaseMat(&(input_camera->camera_intrinsics->camera_matrix));
        cvReleaseMat(&(input_camera->camera_intrinsics->distortion_coefficients));
        cvReleaseMat(&(input_camera->camera_extrinsics->rotation_vector));
        cvReleaseMat(&(input_camera->camera_extrinsics->translation_vector));
    } else if(input_camera->camera_status == INTRINSICALLY_CALIBRATED) {
        cvReleaseMat(&(input_camera->camera_intrinsics->camera_matrix));
        cvReleaseMat(&(input_camera->camera_intrinsics->distortion_coefficients));
    }

    free(input_camera->camera_intrinsics);
    free(input_camera->camera_extrinsics);
    free(input_camera);
}

static void cleanExitIfMainLoopTerminated(struct CameraCapture *world_camera_capture)
{
    g_mutex_lock(&world_camera_feeder_mutex);

    if(main_loop_status == TERMINATED) {
        g_object_unref(world_camera_pixbuf);
        g_mutex_unlock(&world_camera_feeder_mutex);
        releaseCameraCapture(world_camera_capture);
        releaseCamera(world_camera);
        g_thread_exit((gpointer) TRUE);
    } else {
        g_mutex_unlock(&world_camera_feeder_mutex);
    }
}

static void undistortCameraCapture(IplImage *input_frame, IplImage *output_undistorted_frame)
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

gpointer WorldVision_prepareImageFromWorldCameraForDrawing(gpointer data)
{
    IplImage* frame = NULL;
    IplImage* frame_BGR = NULL;
    IplImage* frame_BGR_corrected = NULL;

    initializeWorldCamera();

    g_mutex_lock(&world_camera_feeder_mutex);
    world_camera_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, IPL_DEPTH_8U, WORLD_CAMERA_WIDTH,
                                         WORLD_CAMERA_HEIGHT);
    g_mutex_unlock(&world_camera_feeder_mutex);

    struct CameraCapture *world_camera_capture = malloc(sizeof(struct CameraCapture));

    initializeCameraCapture(world_camera_capture, WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT);

    while(TRUE) {

        cleanExitIfMainLoopTerminated(world_camera_capture);

        frame_BGR = cvQueryFrame(world_camera_capture->camera_capture_feed);

        if(frame_BGR == NULL) {
            cvSetCaptureProperty(world_camera_capture->camera_capture_feed, CV_CAP_PROP_POS_AVI_RATIO, 0);
            continue;
        }

        frame_BGR_corrected = cvCloneImage(frame_BGR);
        frame = cvCloneImage(frame_BGR);

        if(world_camera_capture_mode == UNDISTORTED) {
            undistortCameraCapture(frame_BGR, frame_BGR_corrected);
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

void WorldVision_setMainLoopStatusRunning(void)
{
    g_mutex_lock(&world_camera_feeder_mutex);
    main_loop_status = RUNNING;
    g_mutex_unlock(&world_camera_feeder_mutex);
}

void WorldVision_setMainLoopStatusTerminated(void)
{
    g_mutex_lock(&world_camera_feeder_mutex);
    main_loop_status = TERMINATED;
    g_mutex_unlock(&world_camera_feeder_mutex);
}

void WorldVision_setWorldCameraCaptureModeUndistorted(void)
{
    g_mutex_lock(&world_camera_feeder_mutex);
    world_camera_capture_mode = UNDISTORTED;
    g_mutex_unlock(&world_camera_feeder_mutex);
}

