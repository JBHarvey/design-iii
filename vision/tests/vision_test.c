#include <criterion/criterion.h>
#include <math.h>

#include "vision.h"
#include "markers.h"
#include "opencv2/highgui/highgui_c.h"

Test(vision, angle)
{
    CvPoint point[3];

    point[0].x = point[1].x = 0;
    point[0].y = point[2].y = 0;
    point[1].y = point[2].x = 100;

    double result = angle(point + 1, point + 2, point);
    cr_assert(result == 0);
}

Test(vision, isDualSquare)
{
    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *contours_dual_square = (CvSeq *)cvLoad("dual_square.xml", storage, 0, 0);

    cr_assert(isDualSquare(contours_dual_square));
    cvReleaseMemStorage(&storage);
}

Test(vision, isFigure)
{
    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *figure = (CvSeq *)cvLoad("figure.xml", storage, 0, 0);

    cr_assert(isFigure(figure));
    cvReleaseMemStorage(&storage);
}

_Bool in_range(double value, double expected, double error_percent)
{
    return (fabs(value) > fabs(expected) * (1.0 - error_percent)) && (fabs(value) < fabs(expected) * (1.0 + error_percent));
}

void obstacle_exists(enum ObstacleType type, int x, int y, double angle, struct Obstacle *obstacles,
                     unsigned int num_obstacles)
{
    unsigned int i, count = 0;

    for(i = 0; i < num_obstacles; ++i) {
        if(obstacles[i].type == type && in_range(obstacles[i].x, x, 0.02) && in_range(obstacles[i].y, y, 0.02)) {
            if(type == OBSTACLE_TRIANGLE && in_range(obstacles[i].angle, angle, 0.02)) {
                ++count;
            }

            if(type == OBSTACLE_CIRCLE) {
                ++count;
            }
        }
    }

    cr_assert(count == 1);
}

Test(vision, findObstacles)
{
    CvMemStorage *storage = cvCreateMemStorage(0);
    IplImage *img = cvLoadImage("obstacles_robot.jpg", CV_LOAD_IMAGE_COLOR);
    IplImage *img_yuv = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
    cvCvtColor(img, img_yuv, CV_BGR2YCrCb);

    struct Obstacle obstacles[20];
    int num_obstacles = findObstacles(storage, obstacles, 20, img_yuv);

    cr_assert(num_obstacles == 2);
    obstacle_exists(OBSTACLE_CIRCLE, 747, 376, 0, obstacles, num_obstacles);
    obstacle_exists(OBSTACLE_TRIANGLE, 393, 814, -1.43, obstacles, num_obstacles);

    cvReleaseMemStorage(&storage);
    cvReleaseImage(&img);
    cvReleaseImage(&img_yuv);
}

Test(vision, detectMarker)
{
    IplImage *image = cvLoadImage("obstacles_robot.jpg", CV_LOAD_IMAGE_COLOR);

    struct Marker marker = detectMarker(image);

    cr_assert(marker.valid);
    cr_assert(in_range(marker.x, 1106, 0.02));
    cr_assert(in_range(marker.y, 660, 0.02));
    cr_assert(in_range(marker.angle, 1.46, 0.02));
    cvReleaseImage(&image);
}

void test_coordinateToTableCoordinate(CvPoint input, CvPoint expected)
{
    CvPoint point = coordinateToTableCoordinate(input, 41.6, cvPoint(800, 600));

    cr_assert(in_range(point.x, expected.x, 0.02));
    cr_assert(in_range(point.y, expected.y, 0.02));
}

Test(vision, coordinateToTableCoordinate_1)
{
    CvPoint input = cvPoint(800, 600);
    CvPoint expected = cvPoint(800, 600);

    test_coordinateToTableCoordinate(input, expected);
}

Test(vision, coordinateToTableCoordinate_2)
{
    CvPoint input = cvPoint(393, 815);
    CvPoint expected = cvPoint(477, 765);

    test_coordinateToTableCoordinate(input, expected);
}
