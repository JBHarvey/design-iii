#include <math.h>
#include "vision.h"
#include "markers.h"
#include "world_vision_detection.h"

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

static void drawObstacleOnImage(IplImage *image_BGR, struct Obstacle obstacle)
{
    if(obstacle.type == OBSTACLE_CIRCLE) {
        cvCircle(image_BGR, cvPoint(obstacle.x, obstacle.y), RADIUS_DRAW_CIRCLE / 2, CV_RGB(255, 0, 0), SIZE_DRAW_LINES, 8, 0);
        cvCircle(image_BGR, cvPoint(obstacle.x, obstacle.y), RADIUS_DRAW_CIRCLE, CV_RGB(255, 0, 0), SIZE_DRAW_LINES, 8, 0);
    }

    if(obstacle.type == OBSTACLE_TRIANGLE) {
        cvLine(image_BGR, cvPoint(obstacle.x, obstacle.y), cvPoint(obstacle.x + RADIUS_DRAW_CIRCLE * cos(obstacle.angle),
                obstacle.y + RADIUS_DRAW_CIRCLE * sin(obstacle.angle)), CV_RGB(0, 255, 0), SIZE_DRAW_LINES, 8, 0);
        cvCircle(image_BGR, cvPoint(obstacle.x, obstacle.y), RADIUS_DRAW_CIRCLE, CV_RGB(0, 255, 0), SIZE_DRAW_LINES, 8, 0);
    }
}

static void drawSquareOnImage(IplImage *image_BGR, struct Square square)
{
    unsigned int i;

    for(i = 0; i < NUM_SQUARE_CORNERS; ++i) {
        cvLine(image_BGR, fixedCvPointFrom32f(square.corner[i]),
               fixedCvPointFrom32f(square.corner[(i + 1) % NUM_SQUARE_CORNERS]), CV_RGB(255, 255, 0), SIZE_DRAW_LINES, 8, 0);
    }
}

struct Detected_Things detectDrawObstaclesRobot(CvMemStorage *opencv_storage, IplImage *image_BGR,
        struct Camera *input_camera)
{
    IplImage *image_yuv = cvCreateImage(cvGetSize(image_BGR), IPL_DEPTH_8U, 3);
    cvCvtColor(image_BGR, image_yuv, CV_BGR2YCrCb);
    cvSmooth(image_yuv, image_yuv, CV_GAUSSIAN, 3, 0, 0, 0);

    struct Obstacle obstacles[MAX_OBSTACLES];
    int num_obstacles = findObstacles(opencv_storage, obstacles, MAX_OBSTACLES, image_yuv);

    struct Marker marker = detectMarker(image_BGR);


    struct Square table_corners;
    _Bool table_detected = findTableCorners(image_yuv, &table_corners);

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
        drawObstacleOnImage(image_BGR, obstacles[i]);
        detected.num_obstacles++;
    }

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

    if(table_detected) {
        drawSquareOnImage(image_BGR, table_corners);
    }

    cvReleaseImage(&image_yuv);
    return detected;
}
