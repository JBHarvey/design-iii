#include "opencv2/videoio/videoio_c.h"
#include "opencv2/calib3d/calib3d_c.h"
#include "world_vision.h"
#include "station_interface.h"
#include "world_vision_calibration.h"
#include "ui_event.h"
#include "logger.h"
#include "world_vision_detection.h"
#include "station_client_sender.h"

/* Flags definitions */

enum TooltipStatus {PRINT, PASS};
enum FrameStatus {NOT_READY, READY};
extern enum ThreadStatus main_loop_status;

/* Constants */

const int WORLD_CAMERA_DEVICE_ID = 1;
const int NUMBER_OF_ROW_OF_CAMERA_MATRIX = 3;
const int NUMBER_OF_COLUMN_OF_CAMERA_MATRIX = 3;
const int NUMBER_OF_ROW_OF_TRANSLATION_VECTOR = 3;
const int NUMBER_OF_COLUMN_OF_TRANSLATION_VECTOR = 1;
const int NUMBER_OF_ROW_OF_ROTATION_VECTOR = 3;
const int NUMBER_OF_COLUMN_OF_ROTATION_VECTOR = 1;
const char FILE_PATH_OF_DEBUG_MODE_VIDEO_CAPTURE[] = "./build/deploy/camera_calibration/camera_3015_1/video_feed.avi";

/* Type definitions */

struct CameraCapture {
    CvCapture *camera_capture_feed;
};

/* Global variables */

GMutex world_camera_feeder_mutex;

enum TooltipStatus world_camera_coordinates_tooltip_status = PRINT;
enum FrameStatus world_camera_frame_status = NOT_READY;
struct Camera *world_camera = NULL;
IplImage* world_camera_frame = NULL;
IplImage* world_camera_back_frame = NULL;
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

gboolean worldCameraQueryTooltipEventCallback(GtkWidget *widget, gint x, gint y, gboolean keyboard_mode,
        GtkTooltip *tooltip,
        gpointer data)
{
    if(world_camera != NULL && world_camera->camera_status == FULLY_CALIBRATED
       && world_camera_coordinates_tooltip_status == PRINT) {
        char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
        struct Point3D coordinates = WorldVisionCalibration_convertImageCoordinatesToWorldCoordinates(PointTypes_createPoint2D(
                                         x * WORLD_CAMERA_WIDTH / (double) gtk_widget_get_allocated_width(widget),
                                         y * WORLD_CAMERA_HEIGHT / (double) gtk_widget_get_allocated_height(widget)),
                                     world_camera);
        sprintf(text_buffer, "x: %f mm, y: %f mm, z: %f mm", coordinates.x, coordinates.y, coordinates.z);
        gtk_tooltip_set_text(tooltip, text_buffer);
        return TRUE;
    }

    return FALSE;
}

static void forceHideWorldCameraCoordinatesTooltip(GtkWidget *widget)
{
    world_camera_coordinates_tooltip_status = PASS;
    gtk_widget_trigger_tooltip_query(widget);
    world_camera_coordinates_tooltip_status = PRINT;
}

gboolean worldCameraMotionEventCallback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    forceHideWorldCameraCoordinatesTooltip(widget);

    return TRUE;
}

static void initializeWorldCamera(void)
{
    world_camera = malloc(sizeof(struct Camera));
    world_camera->camera_intrinsics = malloc(sizeof(struct CameraIntrinsics));
    world_camera->camera_extrinsics = malloc(sizeof(struct CameraExtrinsics));
    world_camera->camera_status = UNCALIBRATED;
    world_camera->camera_extrinsics->translation_vector = cvCreateMat(NUMBER_OF_ROW_OF_TRANSLATION_VECTOR,
            NUMBER_OF_COLUMN_OF_TRANSLATION_VECTOR, CV_64F);
    world_camera->camera_extrinsics->rotation_vector = cvCreateMat(NUMBER_OF_ROW_OF_ROTATION_VECTOR,
            NUMBER_OF_COLUMN_OF_ROTATION_VECTOR, CV_64F);
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

static void cleanExitIfMainLoopTerminated(struct CameraCapture *world_camera_capture,
        IplImage *frame_BGR_corrected, IplImage *frame_RGB_corrected,
        GThread *world_vision_detection_worker_thread)
{
    if(main_loop_status == TERMINATED) {
        g_mutex_lock(&world_camera_feeder_mutex);
        g_object_unref(world_camera_pixbuf);
        g_mutex_unlock(&world_camera_feeder_mutex);
        releaseCameraCapture(world_camera_capture);
        releaseCamera(world_camera);
        cvReleaseImage(&world_camera_frame);
        cvReleaseImage(&world_camera_back_frame);
        cvReleaseImage(&frame_BGR_corrected);
        cvReleaseImage(&frame_RGB_corrected);

        if(world_vision_detection_worker_thread != NULL) {
            g_thread_join(world_vision_detection_worker_thread);
        }

        g_thread_exit((gpointer) TRUE);
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

static void freeWorldPixbufData(guchar *pixels, gpointer data)
{
    cvReleaseImage((IplImage **) &data);
}

void WorldVision_applyWorldCameraBackFrame(void)
{
    g_mutex_lock(&world_camera_feeder_mutex);
    cvCopy(world_camera_back_frame, world_camera_frame, NULL);
    world_camera_frame_status = READY;
    g_mutex_unlock(&world_camera_feeder_mutex);
}

IplImage *WorldVision_getWorldCameraBackFrame(void)
{
    return world_camera_back_frame;
}

/* Worker thread */

gpointer WorldVision_prepareImageFromWorldCameraForDrawing(struct StationClient *station_client)
{
    IplImage *frame_BGR = NULL;
    IplImage *frame_BGR_corrected = NULL;
    IplImage *frame_RGB_corrected = NULL;
    GThread *world_vision_detection_worker_thread = NULL;
    //CvMemStorage *opencv_storage = cvCreateMemStorage(0);

    initializeWorldCamera();

    g_mutex_lock(&world_camera_feeder_mutex);
    world_camera_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, IPL_DEPTH_8U, WORLD_CAMERA_WIDTH,
                                         WORLD_CAMERA_HEIGHT);
    g_mutex_unlock(&world_camera_feeder_mutex);

    struct CameraCapture *world_camera_capture = malloc(sizeof(struct CameraCapture));

    initializeCameraCapture(world_camera_capture, WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT);

    while(TRUE) {

        cleanExitIfMainLoopTerminated(world_camera_capture, frame_BGR_corrected, frame_RGB_corrected,
                                      world_vision_detection_worker_thread);

        frame_BGR = cvQueryFrame(world_camera_capture->camera_capture_feed);

        if(frame_BGR == NULL) {
            cvSetCaptureProperty(world_camera_capture->camera_capture_feed, CV_CAP_PROP_POS_AVI_RATIO, 0);
            continue;
        }

        if(world_camera_frame == NULL) {
            world_camera_frame = cvCloneImage(frame_BGR);
        }

        if(frame_BGR_corrected == NULL) {
            frame_BGR_corrected = cvCloneImage(frame_BGR);
        }

        if(frame_RGB_corrected == NULL) {
            frame_RGB_corrected = cvCloneImage(frame_BGR);
        }


        if(world_camera->camera_status != UNCALIBRATED) {

            undistortCameraCapture(frame_BGR, frame_BGR_corrected);

            if(world_camera->camera_status == FULLY_CALIBRATED) {

                if(world_vision_detection_worker_thread == NULL) {
                    world_vision_detection_worker_thread = g_thread_new("world_camera_detector",
                                                           (GThreadFunc) WorldVisionDetection_detectObstaclesAndRobot, (gpointer) world_camera);
                }

                //struct Detected_Things detected = detectDrawObstaclesRobot(opencv_storage, frame_BGR_corrected, world_camera);

                /* if(detected.robot_detected) {
                     StationClientSender_sendWorldInformationsToRobot(station_client, detected.obstacles, MAX_OBSTACLES, detected.robot);
                 }*/
            }
        } else {
            cvCopy(frame_BGR, frame_BGR_corrected, NULL);
        }

        cvCvtColor(frame_BGR_corrected, frame_RGB_corrected, CV_BGR2RGB);

        if(world_camera_back_frame == NULL) {
            world_camera_back_frame = cvCloneImage(frame_RGB_corrected);
        }

        if(world_camera->camera_status == UNCALIBRATED) {
            cvCopy(frame_RGB_corrected, world_camera_frame, NULL);
            world_camera_frame_status = READY;
        }

        if(world_camera_frame_status == READY) {

            g_mutex_lock(&world_camera_feeder_mutex);

            g_object_unref(world_camera_pixbuf);
            world_camera_pixbuf = gdk_pixbuf_new_from_data((guchar*) world_camera_frame->imageData,
                                  GDK_COLORSPACE_RGB, FALSE, world_camera_frame->depth, world_camera_frame->width,
                                  world_camera_frame->height, (world_camera_frame->widthStep),
                                  NULL, NULL);
            //(GdkPixbufDestroyNotify) freeWorldPixbufData, world_camera_frame);
            world_camera_frame_status = NOT_READY;

            g_mutex_unlock(&world_camera_feeder_mutex);

        }

        StationClientSender_sendReceiveData(station_client);
    }

    //cvReleaseMemStorage(&opencv_storage);
    return NULL;
}

