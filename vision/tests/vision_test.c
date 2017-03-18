#include <criterion/criterion.h>

#include "vision.h"

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

void test_coordinateToTableCoordinate(CvPoint input, CvPoint expected)
{
    CvPoint point = coordinateToTableCoordinate(input, 41.6, cvPoint(800, 600));

    cr_assert(point.x > expected.x * 0.98);
    cr_assert(point.x < expected.x * 1.02);
    cr_assert(point.y > expected.y * 0.98);
    cr_assert(point.y < expected.y * 1.02);
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
