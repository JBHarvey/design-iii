#include <math.h>
#include <unistd.h>
#include "CommunicationStructures.h"
#include "world_vision_detection.h"
#include "world_vision_calibration.h"
#include "station_interface.h"

#define ROBOT_HEIGHT_CM 24.5
#define ROBOT_RADIUS_CM 16.0

#define OBSTACLE_HEIGHT_CM 41.0
#define OBSTACLE_RADIUS_CM 7.0

#define ANGLE_RATIO 100000.0
#define LENGTH_MM_RATIO (10)
#define LENGTH_CM_RATIO (10 * LENGTH_MM_RATIO)

#define NEGATIVE_ANGLE_VALUE (MINUS_HALF_PI)
#define POSITIVE_ANGLE_VALUE (HALF_PI)

#define SIZE_DRAW_LINES 3
#define RADIUS_DRAW_CIRCLE 40
#define PINK CV_RGB(255, 0, 255)
#define RED CV_RGB(255, 0, 0)
#define GREEN CV_RGB(0, 255, 0)
#define CYAN CV_RGB(255, 255, 0)
#define YELLOW CV_RGB(0, 255, 255)

#define ANGLE_TOLERANCE 0.0872665
#define POSITION_TOLERANCE 2

#define ONE_SECOND_IN_MICROSECONDS 1000000

enum RobotState {MOVING, DRAWING};

GMutex robot_state_mutex;

int first_detection_happened = 0;
enum RobotState robot_state = MOVING;
struct DetectedThings *detected = NULL;


static int convertAngleToRobotAngle(double angle)
{
    return (angle * ANGLE_RATIO) + 0.5;
}

static int convertObstacleAngleToRobotAngle(double angle)
{
    return angle > 0 ? POSITIVE_ANGLE_VALUE : NEGATIVE_ANGLE_VALUE;
}

static int convertCMToRobot(double length)
{
    return (length * LENGTH_CM_RATIO) + 0.5;
}

static int convertMMToRobot(double length)
{
    return (length * LENGTH_MM_RATIO) + 0.5;
}

static void drawMarkerLocationOnImage(IplImage *image, struct Marker marker)
{
    if(marker.valid) {

        cvLine(image, cvPoint(marker.x, marker.y), cvPoint(marker.x + RADIUS_DRAW_CIRCLE * cos(marker.angle),
                marker.y + RADIUS_DRAW_CIRCLE * sin(marker.angle)), PINK, SIZE_DRAW_LINES, 8, 0);
        cvCircle(image, cvPoint(marker.x, marker.y), RADIUS_DRAW_CIRCLE, PINK, SIZE_DRAW_LINES, 8, 0);
    }
}

static void drawObstacleOnImage(IplImage *image, struct Obstacle obstacle)
{
    if(obstacle.type == OBSTACLE_CIRCLE) {
        cvCircle(image, cvPoint(obstacle.x, obstacle.y), RADIUS_DRAW_CIRCLE / 2, RED, SIZE_DRAW_LINES,
                 8, 0);
        cvCircle(image, cvPoint(obstacle.x, obstacle.y), RADIUS_DRAW_CIRCLE, RED, SIZE_DRAW_LINES, 8,
                 0);
    }

    if(obstacle.type == OBSTACLE_TRIANGLE) {
        cvLine(image, cvPoint(obstacle.x, obstacle.y),
               cvPoint(obstacle.x + RADIUS_DRAW_CIRCLE * cos(obstacle.angle),
                       obstacle.y + RADIUS_DRAW_CIRCLE * sin(obstacle.angle)), GREEN, SIZE_DRAW_LINES, 8, 0);
        cvCircle(image, cvPoint(obstacle.x, obstacle.y), RADIUS_DRAW_CIRCLE, GREEN, SIZE_DRAW_LINES, 8,
                 0);
    }
}

static void drawSquareOnImage(IplImage *image, struct Square square, CvScalar color)
{
    unsigned int i;

    for(i = 0; i < NUM_SQUARE_CORNERS; ++i) {
        cvLine(image, fixedCvPointFrom32f(square.corner[i]),
               fixedCvPointFrom32f(square.corner[(i + 1) % NUM_SQUARE_CORNERS]), color, SIZE_DRAW_LINES, 8, 0);
    }
}

void WorldVisionDetection_drawObstaclesAndRobot(IplImage *world_camera_back_frame)
{
    drawSquareOnImage(world_camera_back_frame, detected->table, CYAN);
    drawSquareOnImage(world_camera_back_frame, detected->green_square, YELLOW);

    if(first_detection_happened && detected != NULL) {

        drawMarkerLocationOnImage(world_camera_back_frame, detected->robot);

        for(int i = 0; i < detected->number_of_obstacles; i++) {
            drawObstacleOnImage(world_camera_back_frame, detected->obstacles[i]);
        }

        drawSquareOnImage(world_camera_back_frame, detected->table, CYAN);
        drawSquareOnImage(world_camera_back_frame, detected->green_square, YELLOW);
    }

    WorldVision_applyWorldCameraBackFrame();
}

static void cleanExitIfMainLoopTerminated(CvMemStorage *opencv_storage)
{
    if(!StationInterface_isRunning()) {
        if(opencv_storage != NULL) {
            cvReleaseMemStorage(&opencv_storage);
        }

        free(detected);
        detected = NULL;
        g_thread_exit((gpointer) TRUE);
    }
}

#define INVALID_OBSTACLE_COORDINATE (-1)

static void packageWorldInformationsAndSendToRobot(struct Camera *input_camera, int environment_has_changed)
{
    struct Communication_Object robot;
    struct Communication_Object obstacles[MAXIMUM_OBSTACLE_NUMBER] = {};

    robot.radius = convertCMToRobot(ROBOT_RADIUS_CM);
    robot.zone.index = 0;
    robot.zone.pose.theta = convertAngleToRobotAngle(detected->robot.angle);
    struct Point3D point3d_robot = WorldVisionCalibration_convertImageCoordinatesToWorldCoordinates(
                                       PointTypes_createPoint2D(detected->robot.x, detected->robot.y), input_camera);
    robot.zone.pose.coordinates.x = convertMMToRobot(point3d_robot.x);
    robot.zone.pose.coordinates.y = convertMMToRobot(point3d_robot.y);

    unsigned int i;

    for(i = 0; i < detected->number_of_obstacles; i++) {
        obstacles[i].radius = convertCMToRobot(OBSTACLE_RADIUS_CM);
        obstacles[i].zone.index = i;

        if(detected->obstacles[i].type == OBSTACLE_CIRCLE) {
            obstacles[i].zone.pose.theta = 0;
        } else {
            obstacles[i].zone.pose.theta = convertObstacleAngleToRobotAngle(detected->obstacles[i].angle);
        }

        obstacles[i].zone.pose.coordinates.x = convertMMToRobot(detected->obstacles[i].x);
        obstacles[i].zone.pose.coordinates.y = convertMMToRobot(detected->obstacles[i].y);
    }

    for(; i < MAXIMUM_OBSTACLE_NUMBER; ++i) {
        obstacles[i].zone.pose.coordinates.x = INVALID_OBSTACLE_COORDINATE;
        obstacles[i].zone.pose.coordinates.y = INVALID_OBSTACLE_COORDINATE;
    }

    WorldVision_sendWorldInformationToRobot(robot, obstacles, environment_has_changed);
}

void WorldVisionDetection_initialize(void)
{
    detected = malloc(sizeof(struct DetectedThings));
    detected->has_changed = 0;
}

static struct Point2DSet *sortSquareCorners(CvPoint2D32f corners[NUM_SQUARE_CORNERS])
{
    struct Point2DSet *point_set = PointTypes_initializePoint2DSet(4);

    CvPoint2D32f *point_sort_x[4] = {};
    CvPoint2D32f *point_sort_y[4] = {};
    CvPoint2D32f *point_sort[4] = {};

    for(int i = 0; i < 4; i++) {

        point_sort_x[i] = (corners + i);
        point_sort_y[i] = (corners + i);
    }

    CvPoint2D32f *temporary_pointer_x;
    CvPoint2D32f *temporary_pointer_y;

    for(int j = 0; j < 4; j++) {
        for(int k = j + 1; k < 4; k++) {
            if(point_sort_x[k]->x < point_sort_x[j]->x) {

                temporary_pointer_x = point_sort_x[j];
                point_sort_x[j] = point_sort_x[k];
                point_sort_x[k] = temporary_pointer_x;
            }

            if(point_sort_y[k]->y < point_sort_y[j]->y) {

                temporary_pointer_y = point_sort_y[j];
                point_sort_y[j] = point_sort_y[k];
                point_sort_y[k] = temporary_pointer_y;
            }

        }
    }

    if(point_sort_x[2] == point_sort_y[0] || point_sort_x[2] == point_sort_y[1]) {

        point_sort[0] = point_sort_x[2];
    } else if(point_sort_x[3] == point_sort_y[0] || point_sort_x[3] == point_sort_y[1]) {

        point_sort[0] = point_sort_x[3];
    }

    if(point_sort_x[2] == point_sort_y[2] || point_sort_x[2] == point_sort_y[3]) {

        point_sort[1] = point_sort_x[2];
    } else if(point_sort_x[3] == point_sort_y[2] || point_sort_x[3] == point_sort_y[3]) {

        point_sort[1] = point_sort_x[3];
    }

    if(point_sort_x[0] == point_sort_y[2] || point_sort_x[0] == point_sort_y[3]) {

        point_sort[2] = point_sort_x[0];
    } else if(point_sort_x[1] == point_sort_y[2] || point_sort_x[1] == point_sort_y[3]) {

        point_sort[2] = point_sort_x[1];
    }

    if(point_sort_x[0] == point_sort_y[0] || point_sort_x[0] == point_sort_y[1]) {

        point_sort[3] = point_sort_x[0];
    } else if(point_sort_x[1] == point_sort_y[0] || point_sort_x[1] == point_sort_y[1]) {

        point_sort[3] = point_sort_x[1];
    }

    for(int l = 0; l < 4; l++) {

        PointTypes_addPointToPoint2DSet(point_set, PointTypes_createPoint2D(point_sort[l]->x,
                                        point_sort[l]->y));
    }

    return point_set;

}

struct Point2DSet *WorldVisionDetection_getDetectedTableCorners(void)
{
    if(detected->table_detected) {

        return sortSquareCorners(detected->table.corner);

    } else {

        return NULL;
    }
}

struct Point2DSet *WorldVisionDetection_getDetectedGreenSquareCorners(void)
{
    if(detected->green_square_detected) {

        return sortSquareCorners(detected->green_square.corner);

    } else {

        return NULL;
    }
}

int WorldVisionDetection_detectTableCorners(struct Camera *input_camera)
{
    WorldVision_createWorldCameraFrameSafeCopy();
    IplImage *image_yuv = cvCreateImage(cvGetSize(input_camera->camera_capture->current_safe_copy_frame), IPL_DEPTH_8U, 3);
    cvCvtColor(input_camera->camera_capture->current_safe_copy_frame, image_yuv, CV_RGB2YCrCb);
    cvSmooth(image_yuv, image_yuv, CV_GAUSSIAN, 3, 0, 0, 0);
    struct Square table_corners;
    _Bool table_detected = findTableCorners(image_yuv, &table_corners);

    if(table_detected) {

        detected->table_detected = 1;
        detected->table = table_corners;
    }

    cvReleaseImage(&image_yuv);

    return table_detected;
}

int WorldVisionDetection_detectGreenSquareCorners(struct Camera *input_camera)
{
    WorldVision_createWorldCameraFrameSafeCopy();
    IplImage *image_yuv = cvCreateImage(cvGetSize(input_camera->camera_capture->current_safe_copy_frame), IPL_DEPTH_8U, 3);
    cvCvtColor(input_camera->camera_capture->current_safe_copy_frame, image_yuv, CV_RGB2YCrCb);
    cvSmooth(image_yuv, image_yuv, CV_GAUSSIAN, 3, 0, 0, 0);
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);

    struct Square green_square;
    _Bool green_square_detected = findFirstGreenSquare(opencv_storage, image_yuv, &green_square);

    if(green_square_detected) {

        detected->green_square_detected = 1;
        detected->green_square = green_square;
    }

    cvReleaseImage(&image_yuv);
    cvReleaseMemStorage(&opencv_storage);

    return green_square_detected;
}

static void tryDetectGreenSquare(struct Camera *input_camera)
{
    if(!WorldVisionDetection_detectGreenSquareCorners(input_camera)) {

        detected->green_square_detected = 0;
    }
}

static int isRobotDrawing(void)
{
    g_mutex_lock(&robot_state_mutex);
    int status = (robot_state == DRAWING);
    g_mutex_unlock(&robot_state_mutex);

    return status;
}

gpointer WorldVisionDetection_detectObstaclesAndRobot(struct Camera *input_camera)
{
    CvMemStorage *opencv_storage = NULL;

    while(TRUE) {

        CvMemStorage *opencv_storage = cvCreateMemStorage(0);
        int environment_has_changed = 0;

        cleanExitIfMainLoopTerminated(opencv_storage);

        WorldVision_createWorldCameraFrameSafeCopy();
        IplImage *image_yuv = cvCreateImage(cvGetSize(input_camera->camera_capture->current_safe_copy_frame), IPL_DEPTH_8U, 3);
        cvCvtColor(input_camera->camera_capture->current_safe_copy_frame, image_yuv, CV_RGB2YCrCb);
        cvSmooth(image_yuv, image_yuv, CV_GAUSSIAN, 3, 0, 0, 0);

        if(!detected->green_square_detected) {
            tryDetectGreenSquare(input_camera);
        }

        struct Obstacle obstacles[MAXIMUM_OBSTACLE_NUMBER];

        int number_of_obstacles = findObstacles(opencv_storage, obstacles, MAXIMUM_OBSTACLE_NUMBER, image_yuv);

        struct Marker marker = detectMarker(input_camera->camera_capture->current_safe_copy_frame);

        if(detected->number_of_obstacles != number_of_obstacles) {

            detected->has_changed = 1;
        }

        detected->number_of_obstacles = number_of_obstacles;

        for(int i = 0; i < number_of_obstacles; ++i) {

            CvPoint obstacle_point = coordinateToTableCoordinate(cvPoint(obstacles[i].x, obstacles[i].y), OBSTACLE_HEIGHT_CM,
                                     cvPoint(cvGetSize(input_camera->camera_capture->current_safe_copy_frame).width / 2,
                                             cvGetSize(input_camera->camera_capture->current_safe_copy_frame).height / 2));

            obstacles[i].x = obstacle_point.x;
            obstacles[i].y = obstacle_point.y;

            if((detected->obstacles[i].x > obstacles[i].x + POSITION_TOLERANCE ||
                detected->obstacles[i].x < obstacles[i].x - POSITION_TOLERANCE ||
                detected->obstacles[i].y > obstacles[i].y + POSITION_TOLERANCE ||
                detected->obstacles[i].y < obstacles[i].y - POSITION_TOLERANCE ||
                detected->obstacles[i].angle > obstacles[i].angle + ANGLE_TOLERANCE
                || detected->obstacles[i].angle < obstacles[i].angle - ANGLE_TOLERANCE)) {

                detected->has_changed = 1;

                if(!environment_has_changed) {

                    while(!WorldVisionDetection_detectTableCorners(input_camera));

                    usleep(ONE_SECOND_IN_MICROSECONDS);

                    if(!isRobotDrawing()) {

                        WorldVisionCalibration_calibrateWithTableCorners(input_camera);
                    }

                    tryDetectGreenSquare(input_camera);

                    environment_has_changed = 1;
                }
            }

            detected->obstacles[i] = obstacles[i];
        }

        if(marker.valid) {

            CvPoint robot_point = coordinateToTableCoordinate(cvPoint(marker.x, marker.y), ROBOT_HEIGHT_CM,
                                  cvPoint(cvGetSize(input_camera->camera_capture->current_safe_copy_frame).width / 2,
                                          cvGetSize(input_camera->camera_capture->current_safe_copy_frame).height / 2));
            marker.x = robot_point.x;
            marker.y = robot_point.y;

            if((detected->robot.x != marker.x || detected->robot.y != marker.y ||
                detected->robot.angle != detected->robot.angle)) {

                detected->has_changed = 1;
            }

            detected->robot = marker;
        }

        if(!first_detection_happened && (number_of_obstacles > 0 || marker.valid)) {
            first_detection_happened = 1;
        }

        if(detected->has_changed == 1 && marker.valid) {
            packageWorldInformationsAndSendToRobot(input_camera, environment_has_changed);
            detected->has_changed = 0;
        }

        cvReleaseMemStorage(&opencv_storage);
        opencv_storage = NULL;
        cvReleaseImage(&image_yuv);
    }

    return NULL;
}

void WorldVisionDetection_setRobotStateToMoving(void)
{
    g_mutex_lock(&robot_state_mutex);

    robot_state = MOVING;

    g_mutex_unlock(&robot_state_mutex);
}

void WorldVisionDetection_setRobotStateToDrawing(void)
{
    g_mutex_lock(&robot_state_mutex);

    robot_state = DRAWING;

    g_mutex_unlock(&robot_state_mutex);
}

