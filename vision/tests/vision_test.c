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

Test(vision, isDualSquare_notDualSquare)
{
    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *contours_dual_square = (CvSeq *)cvLoad("figure.xml", storage, 0, 0);

    cr_assert(!isDualSquare(contours_dual_square));
    cvReleaseMemStorage(&storage);
}


Test(vision, isFigure)
{
    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *figure = (CvSeq *)cvLoad("figure.xml", storage, 0, 0);

    cr_assert(isFigure(figure));
    cvReleaseMemStorage(&storage);
}

Test(vision, isFigure_noFigure)
{
    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *figure = (CvSeq *)cvLoad("dual_square.xml", storage, 0, 0);

    cr_assert(!isFigure(figure));
    cvReleaseMemStorage(&storage);
}

_Bool in_range(double value, double expected, double error_percent)
{
    return (fabs(value) > fabs(expected) * (1.0 - error_percent)) && (fabs(value) < fabs(expected) * (1.0 + error_percent));
}

_Bool in_range_pixels(double value, double expected, double pixels)
{
    return (fabs(value) > fabs(expected) - pixels) && (fabs(value) < fabs(expected) + pixels);
}


void obstacle_exists(enum ObstacleType type, int x, int y, double angle, struct Vision_Obstacle *obstacles,
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
    IplImage *image = cvLoadImage("obstacles_robot.jpg", CV_LOAD_IMAGE_COLOR);
    IplImage *image_yuv = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvCvtColor(image, image_yuv, CV_BGR2YCrCb);

    struct Vision_Obstacle obstacles[20];
    int num_obstacles = findObstacles(storage, obstacles, 20, image_yuv);

    cr_assert(num_obstacles == 2);
    obstacle_exists(OBSTACLE_CIRCLE, 747, 376, 0, obstacles, num_obstacles);
    obstacle_exists(OBSTACLE_TRIANGLE, 393, 814, -1.43, obstacles, num_obstacles);

    cvReleaseMemStorage(&storage);
    cvReleaseImage(&image);
    cvReleaseImage(&image_yuv);
}

Test(vision, findObstacles_noObstacles)
{
    CvMemStorage *storage = cvCreateMemStorage(0);
    IplImage *image = cvLoadImage("random_image.jpg", CV_LOAD_IMAGE_COLOR);
    IplImage *image_yuv = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvCvtColor(image, image_yuv, CV_BGR2YCrCb);

    struct Vision_Obstacle obstacles[20];
    int num_obstacles = findObstacles(storage, obstacles, 20, image_yuv);

    cr_assert(num_obstacles == 0);

    cvReleaseMemStorage(&storage);
    cvReleaseImage(&image);
    cvReleaseImage(&image_yuv);
}

Test(vision, sortObstacles)
{
    struct Vision_Obstacle obstacles[4] = {{.type = OBSTACLE_TRIANGLE, .x = 2, .y = 2}, {.type = OBSTACLE_CIRCLE, .x = 200, .y = 200}, {.type = OBSTACLE_TRIANGLE, .x = 200, .y = 2}, {.type = OBSTACLE_CIRCLE, .x = 500, .y = 203}};

    sortObstacles(obstacles, 4);

    cr_assert(obstacles[0].type == OBSTACLE_CIRCLE);
    cr_assert(obstacles[1].type == OBSTACLE_CIRCLE);
    cr_assert(obstacles[2].type == OBSTACLE_TRIANGLE);
    cr_assert(obstacles[3].type == OBSTACLE_TRIANGLE);

    cr_assert(obstacles[0].x == 200);
    cr_assert(obstacles[2].x == 2);
}

Test(vision, detectMarker)
{
    IplImage *image = cvLoadImage("new_marker.jpg", CV_LOAD_IMAGE_COLOR);

    struct Marker marker = detectMarker(image);

    cr_assert(marker.valid);
    cr_assert(in_range_pixels(marker.x, 876, 1));
    cr_assert(in_range_pixels(marker.y, 290, 1));
    cr_assert(in_range(marker.angle, -0.22, 0.01));
    cvReleaseImage(&image);
}

Test(vision, detectMarker_noMarker)
{
    IplImage *image = cvLoadImage("random_image.jpg", CV_LOAD_IMAGE_COLOR);

    struct Marker marker = detectMarker(image);

    cr_assert(!marker.valid);
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

Test(vision, findFirstGreenSquare)
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    IplImage *image = cvLoadImage("green_square_table.jpg", CV_LOAD_IMAGE_COLOR);
    IplImage *image_yuv = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvCvtColor(image, image_yuv, CV_BGR2YCrCb);

    struct Square square;
    cr_assert(findFirstGreenSquare(opencv_storage, image_yuv, &square));
    cr_assert(in_range_pixels(square.corner[0].x, 196, 1));
    cr_assert(in_range_pixels(square.corner[0].y, 332, 1));
    cr_assert(in_range_pixels(square.corner[1].x, 596, 1));
    cr_assert(in_range_pixels(square.corner[1].y, 336, 1));
    cr_assert(in_range_pixels(square.corner[2].x, 596, 1));
    cr_assert(in_range_pixels(square.corner[2].y, 732, 1));
    cr_assert(in_range_pixels(square.corner[3].x, 199, 1));
    cr_assert(in_range_pixels(square.corner[3].y, 731, 1));

    cvReleaseMemStorage(&opencv_storage);
    cvReleaseImage(&image);
    cvReleaseImage(&image_yuv);
}

Test(vision, findFirstGreenSquare_noSquare)
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    IplImage *image = cvLoadImage("random_image.jpg", CV_LOAD_IMAGE_COLOR);
    IplImage *image_yuv = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvCvtColor(image, image_yuv, CV_BGR2YCrCb);

    struct Square square[1];
    cr_assert(!findFirstGreenSquare(opencv_storage, image_yuv, square));

    cvReleaseMemStorage(&opencv_storage);
    cvReleaseImage(&image);
    cvReleaseImage(&image_yuv);
}

Test(vision, findTableCorners_noTable)
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    IplImage *image = cvLoadImage("white.jpg", CV_LOAD_IMAGE_COLOR);
    IplImage *image_yuv = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvCvtColor(image, image_yuv, CV_BGR2YCrCb);

    struct Square square[1];
    cr_assert(!findTableCorners(image_yuv, square));

    cvReleaseMemStorage(&opencv_storage);
    cvReleaseImage(&image);
    cvReleaseImage(&image_yuv);
}

Test(vision, findTableCorners)
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    IplImage *image = cvLoadImage("table_ref.png", CV_LOAD_IMAGE_COLOR);
    IplImage *image_yuv = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvCvtColor(image, image_yuv, CV_BGR2YCrCb);

    struct Square square;
    cr_assert(findTableCorners(image_yuv, &square));

    cvReleaseMemStorage(&opencv_storage);
    cvReleaseImage(&image);
    cvReleaseImage(&image_yuv);

    cr_assert(in_range_pixels(square.corner[0].x, 41, 20));
    cr_assert(in_range_pixels(square.corner[0].y, 225, 20));
    cr_assert(in_range_pixels(square.corner[1].x, 1551, 5));
    cr_assert(in_range_pixels(square.corner[1].y, 223, 5));
    cr_assert(in_range_pixels(square.corner[2].x, 1553, 5));
    cr_assert(in_range_pixels(square.corner[2].y, 954, 5));
    cr_assert(in_range_pixels(square.corner[3].x, 45, 20));
    cr_assert(in_range_pixels(square.corner[3].y, 956, 20));
}

void assertTable3(struct Square square)
{
    cr_assert(in_range_pixels(square.corner[1].x, 1577, 10));
    cr_assert(in_range_pixels(square.corner[1].y, 258, 10));
    cr_assert(in_range_pixels(square.corner[2].x, 1554, 10));
    cr_assert(in_range_pixels(square.corner[2].y, 1001, 10));
}

void assertTable2(struct Square square)
{
    cr_assert(in_range_pixels(square.corner[1].x, 1556, 10));
    cr_assert(in_range_pixels(square.corner[1].y, 167, 10));
    cr_assert(in_range_pixels(square.corner[2].x, 1566, 10));
    cr_assert(in_range_pixels(square.corner[2].y, 913, 10));
}

void assertTable1(struct Square square)
{
    cr_assert(in_range_pixels(square.corner[1].x, 1578, 10));
    cr_assert(in_range_pixels(square.corner[1].y, 194, 10));
    cr_assert(in_range_pixels(square.corner[2].x, 1572, 10));
    cr_assert(in_range_pixels(square.corner[2].y, 934, 10));
}

void tableTest(char *filename, void (*assertFunction)(struct Square))
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    IplImage *image = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);
    IplImage *image_yuv = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
    cvCvtColor(image, image_yuv, CV_BGR2YCrCb);

    struct Square square;
    cr_assert(findTableCorners(image_yuv, &square));

    cvReleaseMemStorage(&opencv_storage);
    cvReleaseImage(&image);
    cvReleaseImage(&image_yuv);

    assertFunction(square);
}

Test(vision, findTableCorners_table3)
{
    tableTest("table3.jpg", assertTable3);
    tableTest("table3-1.jpg", assertTable3);
    tableTest("table3-2.jpg", assertTable3);
    tableTest("table3-3.jpg", assertTable3);
}

Test(vision, findTableCorners_table2)
{
    tableTest("table2.jpg", assertTable2);
    tableTest("table2-1.jpg", assertTable2);
    tableTest("table2-2.jpg", assertTable2);
    tableTest("table2-3.jpg", assertTable2);
}

Test(vision, findTableCorners_table1)
{
    tableTest("table1.jpg", assertTable1);
    tableTest("table1-1.jpg", assertTable1);
    tableTest("table1-2.jpg", assertTable1);
    tableTest("table1-3.jpg", assertTable1);
}
