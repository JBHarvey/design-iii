#include "opencv2/calib3d/calib3d_c.h"
#include "world_vision.h"
#include "station_interface.h"
#include "world_vision_calibration.h"
#include "world_vision_detection.h"
#include "ui_event.h"
#include "logger.h"
#include "station_client_sender.h"

#define SIZE_DRAW_LINES 3
#define BURGUNDY CV_RGB(29, 7, 173)

/* Flags definitions */

enum TooltipStatus {PRINT, PASS};
enum FrameStatus {NOT_READY, READY};

/* Constants */

const int WORLD_CAMERA_DEVICE_ID = 1;
const int NUMBER_OF_ROW_OF_CAMERA_MATRIX = 3;
const int NUMBER_OF_COLUMN_OF_CAMERA_MATRIX = 3;
const int NUMBER_OF_ROW_OF_TRANSLATION_VECTOR = 3;
const int NUMBER_OF_COLUMN_OF_TRANSLATION_VECTOR = 1;
const int NUMBER_OF_ROW_OF_ROTATION_VECTOR = 3;
const int NUMBER_OF_COLUMN_OF_ROTATION_VECTOR = 1;
const char FILE_PATH_OF_DEBUG_MODE_VIDEO_CAPTURE[] = "./build/deploy/camera_calibration/camera_3015_1/video_feed.avi";

/* Global variables */

GMutex world_vision_pixbuf_mutex;
GMutex world_vision_frame_mutex;
GMutex world_vision_camera_frame_mutex;
GMutex world_vision_camera_intrinsics_mutex;
GMutex world_vision_planned_trajectory_mutex;
GMutex world_vision_robot_position_mutex;

enum TooltipStatus world_camera_coordinates_tooltip_status = PRINT;
enum FrameStatus world_camera_frame_status = NOT_READY;
struct Camera *world_camera = NULL;
IplImage *world_camera_frame = NULL;
IplImage *world_camera_back_frame = NULL;
GdkPixbuf *world_camera_pixbuf = NULL;

struct Point2DSet *current_planned_trajectory = NULL;
GSList *robot_position = NULL;

GThread *world_vision_detection_worker_thread = NULL;

gboolean worldCameraDrawEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    gboolean status = FALSE;

    if(G_IS_OBJECT(world_camera_pixbuf)) {
        g_mutex_lock(&world_vision_pixbuf_mutex);
        status = drawEventCallback(widget, event, world_camera_pixbuf);
        g_mutex_unlock(&world_vision_pixbuf_mutex);
    }

    return status;
}

gboolean worldCameraCalibrationClickedEventCallback(GtkWidget *widget, gpointer data)
{
    if(WorldVisionCalibration_initializeCameraMatrixAndDistortionCoefficientsFromFile(widget,
            world_camera)) {
        gtk_widget_set_sensitive(GTK_WIDGET(data), FALSE);
        WorldVisionDetection_initialize();
        world_vision_detection_worker_thread = g_thread_new("world_camera_detector",
                                               (GThreadFunc) WorldVisionDetection_detectObstaclesAndRobot, (gpointer) world_camera);
        WorldVisionCalibration_calibrateWithTableCorners(world_camera);
        WorldVisionDetection_detectGreenSquareCorners(world_camera);
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
    world_camera->camera_capture = malloc(sizeof(struct CameraCapture));
    world_camera->camera_status = UNCALIBRATED;
    world_camera->camera_extrinsics->translation_vector = cvCreateMat(NUMBER_OF_ROW_OF_TRANSLATION_VECTOR,
            NUMBER_OF_COLUMN_OF_TRANSLATION_VECTOR, CV_64F);
    world_camera->camera_extrinsics->rotation_vector = cvCreateMat(NUMBER_OF_ROW_OF_ROTATION_VECTOR,
            NUMBER_OF_COLUMN_OF_ROTATION_VECTOR, CV_64F);
    world_camera->camera_capture->camera_capture_feed = cvCreateCameraCapture(WORLD_CAMERA_DEVICE_ID);

    if(!world_camera->camera_capture->camera_capture_feed) {

        world_camera->camera_capture->camera_capture_feed = cvCreateFileCapture(FILE_PATH_OF_DEBUG_MODE_VIDEO_CAPTURE);
    }

    cvSetCaptureProperty(world_camera->camera_capture->camera_capture_feed, CV_CAP_PROP_FRAME_WIDTH, WORLD_CAMERA_WIDTH);
    cvSetCaptureProperty(world_camera->camera_capture->camera_capture_feed, CV_CAP_PROP_FRAME_HEIGHT, WORLD_CAMERA_HEIGHT);
    world_camera->camera_capture->current_raw_frame = cvQueryFrame(world_camera->camera_capture->camera_capture_feed);
    world_camera->camera_capture->current_safe_copy_frame = cvCloneImage(world_camera->camera_capture->current_raw_frame);
}

static void releaseWorldCamera(void)
{
    if(world_camera->camera_status == FULLY_CALIBRATED) {
        cvReleaseMat(&(world_camera->camera_intrinsics->camera_matrix));
        cvReleaseMat(&(world_camera->camera_intrinsics->distortion_coefficients));
        cvReleaseMat(&(world_camera->camera_extrinsics->rotation_vector));
        cvReleaseMat(&(world_camera->camera_extrinsics->translation_vector));
    } else if(world_camera->camera_status == INTRINSICALLY_CALIBRATED) {
        cvReleaseMat(&(world_camera->camera_intrinsics->camera_matrix));
        cvReleaseMat(&(world_camera->camera_intrinsics->distortion_coefficients));
    }

    cvReleaseCapture(&(world_camera->camera_capture->camera_capture_feed));
    cvReleaseImage(&(world_camera->camera_capture->current_safe_copy_frame));
    free(world_camera->camera_intrinsics);
    free(world_camera->camera_extrinsics);
    free(world_camera->camera_capture);
    free(world_camera);
}

static void cleanExitIfMainLoopTerminated(void)
{
    if(!StationInterface_isRunning()) {
        g_mutex_lock(&world_vision_pixbuf_mutex);
        g_object_unref(world_camera_pixbuf);
        g_mutex_unlock(&world_vision_pixbuf_mutex);

        if(world_vision_detection_worker_thread != NULL) {

            g_thread_join(world_vision_detection_worker_thread);
        }

        if(current_planned_trajectory != NULL) {

            PointTypes_releasePoint2DSet(current_planned_trajectory);
            current_planned_trajectory = NULL;
        }

        releaseWorldCamera();
        cvReleaseImage(&world_camera_frame);
        cvReleaseImage(&world_camera_back_frame);

        g_thread_exit((gpointer) TRUE);
    }
}

static void undistortFrame(IplImage *input_output_frame)
{
    g_mutex_lock(&world_vision_camera_intrinsics_mutex);
    IplImage *temporary_frame = cvCloneImage(input_output_frame);
    CvMat *optimal_camera_matrix = cvCreateMat(NUMBER_OF_ROW_OF_CAMERA_MATRIX, NUMBER_OF_COLUMN_OF_CAMERA_MATRIX, CV_64F);
    cvGetOptimalNewCameraMatrix(world_camera->camera_intrinsics->camera_matrix,
                                world_camera->camera_intrinsics->distortion_coefficients,
                                cvSize(WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT),
                                1, optimal_camera_matrix,
                                cvSize(WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT), 0, 0);
    cvUndistort2(temporary_frame, input_output_frame, world_camera->camera_intrinsics->camera_matrix,
                 world_camera->camera_intrinsics->distortion_coefficients, optimal_camera_matrix);
    cvReleaseImage(&temporary_frame);
    cvReleaseMat(&optimal_camera_matrix);
    g_mutex_unlock(&world_vision_camera_intrinsics_mutex);
}

static void correctFrameColor(IplImage *input_output_frame)
{
    IplImage *temporary_frame = cvCloneImage(input_output_frame);
    cvCvtColor(temporary_frame, input_output_frame, CV_BGR2RGB);
    cvReleaseImage(&temporary_frame);
}

static void worldCameraQueryNextFrame(void)
{
    g_mutex_lock(&world_vision_camera_frame_mutex);

    world_camera->camera_capture->current_raw_frame = cvQueryFrame(world_camera->camera_capture->camera_capture_feed);

    while(world_camera->camera_capture->current_raw_frame == NULL) {

        cvSetCaptureProperty(world_camera->camera_capture->camera_capture_feed, CV_CAP_PROP_POS_AVI_RATIO, 0);
        world_camera->camera_capture->current_raw_frame = cvQueryFrame(world_camera->camera_capture->camera_capture_feed);
    }

    if(world_camera->camera_status != UNCALIBRATED) {

        undistortFrame(world_camera->camera_capture->current_raw_frame);
    }

    correctFrameColor(world_camera->camera_capture->current_raw_frame);

    g_mutex_unlock(&world_vision_camera_frame_mutex);
}

void WorldVision_createWorldCameraFrameSafeCopy(void)
{
    g_mutex_lock(&world_vision_camera_frame_mutex);
    cvCopy(world_camera->camera_capture->current_raw_frame, world_camera->camera_capture->current_safe_copy_frame, NULL);
    g_mutex_unlock(&world_vision_camera_frame_mutex);
}

static void drawPlannedTrajectory(void)
{
    for(int i = 0; i < PointTypes_getNumberOfPointStoredInPoint2DSet(current_planned_trajectory) - 1; i++) {
        struct Point2D origin = PointTypes_getPointFromPoint2DSet(current_planned_trajectory, i);
        struct Point2D end = PointTypes_getPointFromPoint2DSet(current_planned_trajectory, i + 1);
        cvLine(world_camera_frame, cvPoint((int) origin.x, (int) origin.y), cvPoint((int) end.x, (int) end.y),
               BURGUNDY, SIZE_DRAW_LINES, 8, 0);
    }
}

void WorldVision_applyWorldCameraBackFrame(void)
{
    g_mutex_lock(&world_vision_frame_mutex);
    cvCopy(world_camera_back_frame, world_camera_frame, NULL);

    g_mutex_lock(&world_vision_planned_trajectory_mutex);

    if(current_planned_trajectory != NULL) {

        drawPlannedTrajectory();
    }

    g_mutex_unlock(&world_vision_planned_trajectory_mutex);

    world_camera_frame_status = READY;
    g_mutex_unlock(&world_vision_frame_mutex);
}

/* Worker thread */

gpointer WorldVision_prepareImageFromWorldCameraForDrawing(struct StationClient *station_client)
{
    initializeWorldCamera();

    world_camera_frame = cvCloneImage(world_camera->camera_capture->current_raw_frame);
    world_camera_back_frame = cvCloneImage(world_camera->camera_capture->current_raw_frame);

    g_mutex_lock(&world_vision_pixbuf_mutex);
    world_camera_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, IPL_DEPTH_8U, WORLD_CAMERA_WIDTH,
                                         WORLD_CAMERA_HEIGHT);
    g_mutex_unlock(&world_vision_pixbuf_mutex);

    while(TRUE) {

        cleanExitIfMainLoopTerminated();

        worldCameraQueryNextFrame();

        if(world_camera->camera_status == FULLY_CALIBRATED) {

            cvCopy(world_camera->camera_capture->current_raw_frame, world_camera_back_frame, NULL);

            if(StationInterface_isRunning()) {

                WorldVisionDetection_drawObstaclesAndRobot(world_camera_back_frame);
            }
        }

        if(world_camera->camera_status != FULLY_CALIBRATED) {
            cvCopy(world_camera->camera_capture->current_raw_frame, world_camera_frame, NULL);
            world_camera_frame_status = READY;
        }

        if(world_camera_frame_status == READY) {

            g_mutex_lock(&world_vision_pixbuf_mutex);
            g_mutex_lock(&world_vision_frame_mutex);

            g_object_unref(world_camera_pixbuf);
            world_camera_pixbuf = gdk_pixbuf_new_from_data((guchar*) world_camera_frame->imageData,
                                  GDK_COLORSPACE_RGB, FALSE, world_camera_frame->depth, world_camera_frame->width,
                                  world_camera_frame->height, (world_camera_frame->widthStep),
                                  NULL, NULL);
            world_camera_frame_status = NOT_READY;

            g_mutex_unlock(&world_vision_frame_mutex);
            g_mutex_unlock(&world_vision_pixbuf_mutex);

        }

        StationClientSender_sendReceiveData(station_client);
    }

    return NULL;
}

void WorldVision_sendWorldInformationToRobot(struct Communication_Object robot,
        struct Communication_Object obstacles[MAXIMUM_OBSTACLE_NUMBER], int environment_has_changed)
{
    StationClientSender_sendWorldInformationsToRobot(obstacles, MAXIMUM_OBSTACLE_NUMBER, robot,
            environment_has_changed);
}

void WorldVision_setPlannedTrajectory(struct Point3DSet *world_trajectory)
{
    g_mutex_lock(&world_vision_planned_trajectory_mutex);

    if(current_planned_trajectory != NULL) {

        PointTypes_releasePoint2DSet(current_planned_trajectory);
        current_planned_trajectory = NULL;
    }

    current_planned_trajectory = PointTypes_initializePoint2DSet(PointTypes_getNumberOfPointStoredInPoint3DSet(
                                     world_trajectory));

    if(world_camera->camera_status == FULLY_CALIBRATED) {
        WorldVisionCalibration_convertWorldCoordinatesSetToImageCoordinatesSet(world_trajectory, current_planned_trajectory,
                world_camera);
    }

    g_mutex_unlock(&world_vision_planned_trajectory_mutex);
}

void WorldVision_recalibrateForDrawing(void)
{
    WorldVisionDetection_setRobotStateToDrawing();
    WorldVisionCalibration_calibrateWithGreenSquareCorners(world_camera);
}

void WorldVision_recalibrateForMoving(void)
{
    WorldVisionCalibration_calibrateWithTableCorners(world_camera);
    WorldVisionDetection_setRobotStateToMoving();
}

void WorldVision_setRobotPosition(struct Point3D world_coordinates)
{
    g_mutex_lock(&world_vision_robot_position_mutex);

    struct Point2D image_coordinates = WorldVisionCalibration_convertWorldCoordinatesToImageCoordinates(
                                           world_coordinates, world_camera);
    struct Point2D *image_coordinates_pointer = malloc(sizeof(struct Point2D));
    memcpy(image_coordinates_pointer, &image_coordinates, sizeof(struct Point2D));
    robot_position = g_slist_append(robot_position, (gpointer) image_coordinates_pointer);

    g_mutex_unlock(&world_vision_robot_position_mutex);
}

static void freeRobotPosition(gpointer data)
{
    free((struct Point2D *) data);
}

void WorldVision_resetRobotPosition(void)
{
    g_mutex_lock(&world_vision_robot_position_mutex);

    if(robot_position != NULL) {
        g_slist_free_full(robot_position, (GDestroyNotify) freeRobotPosition);
        robot_position = NULL;
    }

    g_mutex_unlock(&world_vision_robot_position_mutex);
}

