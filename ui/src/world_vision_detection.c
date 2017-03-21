#include <math.h>
#include "world_vision_detection.h"
#include "station_interface.h"

#define ROBOT_HEIGHT_CM 24.0
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

extern enum ThreadStatus main_loop_status;

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

static void drawMarkerLocationOnImage(IplImage *image_BGR, struct Marker marker)
{
    cvLine(image_BGR, cvPoint(marker.x, marker.y), cvPoint(marker.x + RADIUS_DRAW_CIRCLE * cos(marker.angle),
            marker.y + RADIUS_DRAW_CIRCLE * sin(marker.angle)), CV_RGB(255, 0, 255), SIZE_DRAW_LINES, 8, 0);
    cvCircle(image_BGR, cvPoint(marker.x, marker.y), RADIUS_DRAW_CIRCLE, CV_RGB(255, 0, 255), SIZE_DRAW_LINES, 8, 0);
}

static void drawObstaclesOnImage(IplImage *image_BGR, struct Obstacle obstacles[MAXIMUM_OBSTACLE_NUMBER],
                                 int number_of_obstacles)
{
    for(int i = 0; i < number_of_obstacles; i++) {
        if(obstacles[i].type == OBSTACLE_CIRCLE) {
            cvCircle(image_BGR, cvPoint(obstacles[i].x, obstacles[i].y), RADIUS_DRAW_CIRCLE / 2, CV_RGB(255, 0, 0), SIZE_DRAW_LINES,
                     8, 0);
            cvCircle(image_BGR, cvPoint(obstacles[i].x, obstacles[i].y), RADIUS_DRAW_CIRCLE, CV_RGB(255, 0, 0), SIZE_DRAW_LINES, 8,
                     0);
        }

        if(obstacles[i].type == OBSTACLE_TRIANGLE) {
            cvLine(image_BGR, cvPoint(obstacles[i].x, obstacles[i].y),
                   cvPoint(obstacles[i].x + RADIUS_DRAW_CIRCLE * cos(obstacles[i].angle),
                           obstacles[i].y + RADIUS_DRAW_CIRCLE * sin(obstacles[i].angle)), CV_RGB(0, 255, 0), SIZE_DRAW_LINES, 8, 0);
            cvCircle(image_BGR, cvPoint(obstacles[i].x, obstacles[i].y), RADIUS_DRAW_CIRCLE, CV_RGB(0, 255, 0), SIZE_DRAW_LINES, 8,
                     0);
        }
    }
}

static void cleanExitIfMainLoopTerminated(CvMemStorage *opencv_storage)
{
    if(main_loop_status == TERMINATED) {
        cvReleaseMemStorage(&opencv_storage);
        g_thread_exit((gpointer) TRUE);
    }
}

gpointer WorldVisionDetection_detectObstaclesAndRobot(struct Camera *input_camera)
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);

    while(TRUE) {

        cleanExitIfMainLoopTerminated(opencv_storage);

        IplImage *world_camera_back_frame = WorldVision_getWorldCameraBackFrame();
        IplImage *image_yuv = cvCreateImage(cvGetSize(world_camera_back_frame), IPL_DEPTH_8U, 3);
        cvCvtColor(world_camera_back_frame, image_yuv, CV_RGB2YCrCb);
        cvSmooth(image_yuv, image_yuv, CV_GAUSSIAN, 3, 0, 0, 0);

        struct Obstacle obstacles[MAXIMUM_OBSTACLE_NUMBER];
        int number_of_obstacles = findObstacles(opencv_storage, obstacles, MAXIMUM_OBSTACLE_NUMBER, image_yuv);

        struct Marker marker = detectMarker(world_camera_back_frame);
        struct Detected_Things detected = {};

        detected.number_of_obstacles = number_of_obstacles;

        for(int i = 0; i < number_of_obstacles; ++i) {

            CvPoint obstacle_point = coordinateToTableCoordinate(cvPoint(obstacles[i].x, obstacles[i].y), OBSTACLE_HEIGHT_CM,
                                     cvPoint(cvGetSize(world_camera_back_frame).width / 2, cvGetSize(world_camera_back_frame).height / 2));

            obstacles[i].x = obstacle_point.x;
            obstacles[i].y = obstacle_point.y;
            detected.obstacles[i] = obstacles[i];
        }

        if(marker.valid) {

            CvPoint robot_point = coordinateToTableCoordinate(cvPoint(marker.x, marker.y), ROBOT_HEIGHT_CM,
                                  cvPoint(cvGetSize(world_camera_back_frame).width / 2, cvGetSize(world_camera_back_frame).height / 2));
            marker.x = robot_point.x;
            marker.y = robot_point.y;
            detected.robot = marker;
        }

        WorldVision_applyWorldCameraBackFrame();

        cvReleaseImage(&image_yuv);
    }

    return NULL;
}

/*
struct Detected_Things detectDrawObstaclesRobot(CvMemStorage *opencv_storage, IplImage *image_BGR,
        struct Camera *input_camera)
{
    IplImage *image_yuv = cvCreateImage(cvGetSize(image_BGR), IPL_DEPTH_8U, 3);
    cvCvtColor(image_BGR, image_yuv, CV_BGR2YCrCb);
    cvSmooth(image_yuv, image_yuv, CV_GAUSSIAN, 3, 0, 0, 0);

    struct Obstacle obstacles[MAX_OBSTACLES];
    int num_obstacles = findObstacles(opencv_storage, obstacles, MAX_OBSTACLES, image_yuv);

    struct Marker marker = detectMarker(image_BGR);

    struct Detected_Things detected = {};

    unsigned int i;

    for(i = 0; i < num_obstacles; ++i) {
        CvPoint obstacle_point = coordinateToTableCoordinate(cvPoint(obstacles[i].x, obstacles[i].y), OBSTACLE_HEIGHT_CM,
                                 cvPoint(cvGetSize(image_BGR).width / 2, cvGetSize(image_BGR).height / 2));

        obstacles[i].x = obstacle_point.x;
        obstacles[i].y = obstacle_point.y;

        detected.obstacles[i].radius = convertCMToRobot(OBSTACLE_RADIUS_CM);
        detected.obstacles[i].zone.index = i;

        if(obstacles[i].type == OBSTACLE_CIRCLE) {
            detected.obstacles[i].zone.pose.theta = 0;
        } else {
            detected.obstacles[i].zone.pose.theta = convertObstacleAngleToRobotAngle(obstacles[i].angle);
        }

        detected.obstacles[i].zone.pose.coordinates.x = convertMMToRobot(obstacles[i].x);
        detected.obstacles[i].zone.pose.coordinates.y = convertMMToRobot(obstacles[i].y);
        detected.num_obstacles++;
    }

    drawObstaclesOnImage(image_BGR, obstacles, num_obstacles);

    if(marker.valid) {
        detected.robot_detected = 1;
        CvPoint robot_point = coordinateToTableCoordinate(cvPoint(marker.x, marker.y), ROBOT_HEIGHT_CM,
                              cvPoint(cvGetSize(image_BGR).width / 2, cvGetSize(image_BGR).height / 2));

        marker.x = robot_point.x;
        marker.y = robot_point.y;
        detected.robot.radius = convertCMToRobot(ROBOT_RADIUS_CM);
        detected.robot.zone.index = 0;
        detected.robot.zone.pose.theta = convertAngleToRobotAngle(marker.angle);
        struct Point3D point3d_robot = WorldVisionCalibration_convertImageCoordinatesToWorldCoordinates(
                                           PointTypes_createPoint2D(marker.x, marker.y), input_camera);
        detected.robot.zone.pose.coordinates.x = convertMMToRobot(point3d_robot.x);
        detected.robot.zone.pose.coordinates.y = convertMMToRobot(point3d_robot.y);

        drawMarkerLocationOnImage(image_BGR, marker);
    }

    cvReleaseImage(&image_yuv);
    return detected;
}*/
