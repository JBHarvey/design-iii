#include <stdio.h>
#include <math.h>
#include "vision.h"

/* Used by the square finding algorithm
 * If an angle has a cosine bigger than this value the shape is not considered a square
 */
#define MAX_SQUARE_COSINE 0.4

/* Number of pixels that the black and white images will be eroded then dilated to make
 * the edges cleaner.
 */
#define ERODE_DILATE_PIXELS 1

#define GREEN_SQUARE_YUV_UPPER_BOUND (cvScalar(255, 120, 120, 255))
#define GREEN_SQUARE_YUV_LOWER_BOUND (cvScalar(10, 0, 0, 255))

#define OBSTACLES_YUV_UPPER_BOUND (cvScalar(120, 148, 148, 255))
#define OBSTACLES_YUV_LOWER_BOUND (cvScalar(0, 108, 108, 255))

#define FIGURE_Y_UPPER_BOUND (150)

/* Min distance between UV coordinates and the center to detect figure.
 */
#define FIGURE_UV_MIN_DISTANCE (16)



#define GREEN_SQUARE_POLY_APPROX 100
#define FIGURE_POLY_APPROX 5

#define WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE 1000

#define MIN_FIGURE_AREA (WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE * WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE * 0.05)
#define MAX_FIGURE_AREA (WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE * WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE * 0.9)


struct Square {
    CvPoint2D32f corner[4];
};

double angle(CvPoint *pt1, CvPoint *pt2, CvPoint *pt0)
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

static _Bool isSquare(CvSeq *contours)
{
    if(contours->total == 4 &&
       fabs(cvContourArea(contours, CV_WHOLE_SEQ, 0)) > 1000 &&
       cvCheckContourConvexity(contours)) {
        double s = 0;

        unsigned int i;

        for(i = 0; i < 5; i++) {
            /* find minimum angle between joint
             * edges (maximum of cosine)
             */
            if(i >= 2) {
                double t = fabs(angle(
                                    (CvPoint *)cvGetSeqElem(contours, i),
                                    (CvPoint *)cvGetSeqElem(contours, i - 2),
                                    (CvPoint *)cvGetSeqElem(contours, i - 1)));
                s = s > t ? s : t;
            }
        }

        if(s < MAX_SQUARE_COSINE) {
            for(i = 0; i < 4; i++) {
                return 1;
            }
        }
    }

    return 0;
}

_Bool isDualSquare(CvSeq *contours)
{
    if(contours->v_next && isSquare(contours) && isSquare(contours->v_next)) {
        return 1;
    }

    return 0;
}

static IplImage *thresholdImage(IplImage *image_yuv, CvScalar lower_bound, CvScalar upper_bound,
                                unsigned int erode_dilate_pixels)
{
    IplImage *image_black_white = cvCreateImage(cvGetSize(image_yuv), IPL_DEPTH_8U, 1);
    cvInRangeS(image_yuv, lower_bound, upper_bound, image_black_white);

    cvDilate(image_black_white, image_black_white, NULL, erode_dilate_pixels);
    cvErode(image_black_white, image_black_white, NULL, erode_dilate_pixels * 2);
    cvDilate(image_black_white, image_black_white, NULL, erode_dilate_pixels);
    return image_black_white;
}

#define YUV_CENTER_OFFSET 128

static IplImage *thresholdImage3D(IplImage *image_yuv, unsigned int erode_dilate_pixels)
{
    IplImage *image_black_white = cvCreateImage(cvGetSize(image_yuv), IPL_DEPTH_8U, 1);
    IplImage *image_y = cvCreateImage(cvGetSize(image_yuv), IPL_DEPTH_8U, 1);
    cvSplit(image_yuv, image_y, 0, 0, 0);
    int thresh_y = cvThreshold(image_y, image_black_white, 0, 0, CV_THRESH_BINARY | CV_THRESH_OTSU);

    cvReleaseImage(&image_y);

    if(thresh_y > FIGURE_Y_UPPER_BOUND) {
        thresh_y = FIGURE_Y_UPPER_BOUND;
    }

    unsigned int width = image_yuv->width;
    unsigned int height = image_yuv->height;
    unsigned int source_width_step = image_yuv->widthStep;
    unsigned int dest_width_step = image_black_white->widthStep;

    unsigned int size = image_yuv->widthStep * image_yuv->height;
    unsigned int i, j;

    for(j = 0; j < height; j++) {
        for(i = 0; i < width; i++) {
            unsigned int source_row_index = j * source_width_step;
            int y = (uchar)image_yuv->imageData[i * 3 + 0 + source_row_index];
            int u = (uchar)image_yuv->imageData[i * 3 + 1 + source_row_index];
            int v = (uchar)image_yuv->imageData[i * 3 + 2 + source_row_index];

            int px = (u - YUV_CENTER_OFFSET);
            int py = (v - YUV_CENTER_OFFSET);

            unsigned int dest_row_index = j * dest_width_step;

            if(y > thresh_y && (px * px + py * py) < (FIGURE_UV_MIN_DISTANCE * FIGURE_UV_MIN_DISTANCE)) {
                image_black_white->imageData[i + dest_row_index] = 255;
            } else {
                image_black_white->imageData[i + dest_row_index] = 0;
            }
        }
    }

    cvDilate(image_black_white, image_black_white, NULL, erode_dilate_pixels);
    cvErode(image_black_white, image_black_white, NULL, erode_dilate_pixels * 2);
    cvDilate(image_black_white, image_black_white, NULL, erode_dilate_pixels);
    return image_black_white;
}


static void findGreenSquaresRecursive(CvSeq *contours, struct Square *out_squares, unsigned int max_squares,
                                      unsigned int *num_squares)
{
    if(*num_squares >= max_squares) {
        return;
    }

    CvSeq *horizontal_sequence = contours;

    while(horizontal_sequence) {
        if(isDualSquare(horizontal_sequence)) {
            struct Square square;
            square.corner[0] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(horizontal_sequence->v_next, 0));
            square.corner[1] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(horizontal_sequence->v_next, 1));
            square.corner[2] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(horizontal_sequence->v_next, 2));
            square.corner[3] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(horizontal_sequence->v_next, 3));

            /* fix rotation */
            if(square.corner[0].x * square.corner[0].y > square.corner[3].x * square.corner[3].y) {
                CvPoint2D32f temp = square.corner[0];
                square.corner[0] = square.corner[3];
                square.corner[3] = square.corner[2];
                square.corner[2] = square.corner[1];
                square.corner[1] = temp;
            }

            out_squares[*num_squares] = square;
            ++*num_squares;

            if(*num_squares >= max_squares) {
                return;
            }
        }

        findGreenSquaresRecursive(horizontal_sequence->v_next, out_squares, max_squares, num_squares);
        horizontal_sequence = horizontal_sequence->h_next;
    }
}

static unsigned int findGreenSquares(CvMemStorage *storage, IplImage *image_black_white, struct Square *out_squares,
                                     unsigned int max_squares)
{
    unsigned int num_squares = 0;

    if(max_squares == 0) {
        return 0;
    }

    CvSeq *contours = 0;

    if(cvFindContours(image_black_white, storage, &contours, sizeof(CvContour),
                      CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0))) {

        contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, GREEN_SQUARE_POLY_APPROX, 1);
        findGreenSquaresRecursive(contours, out_squares, max_squares, &num_squares);
    }

    return num_squares;
}

static IplImage *imageInsideSquare(IplImage *image_yuv, struct Square square)
{
    CvPoint2D32f dst[4];
    dst[0] = cvPoint2D32f(0, 0);
    dst[1] = cvPoint2D32f(WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE, 0);
    dst[2] = cvPoint2D32f(WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE, WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE);
    dst[3] = cvPoint2D32f(0, WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE);

    CvMat *perspective_matrix = cvCreateMat(3, 3, CV_32F);
    perspective_matrix = cvGetPerspectiveTransform(square.corner, dst, perspective_matrix);

    IplImage *corrected_image = cvCreateImage(cvSize(WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE,
                                WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE), IPL_DEPTH_8U, 3);

    cvWarpPerspective(image_yuv, corrected_image, perspective_matrix, CV_INTER_LINEAR, cvScalarAll(0));
    cvReleaseMat(&perspective_matrix);
    return corrected_image;
}

static CvSeq *findFigureContours(CvMemStorage *storage, IplImage *image_black_white)
{
    CvSeq *contours = NULL;

    if(cvFindContours(image_black_white, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE,
                      cvPoint(0, 0))) {
        contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, FIGURE_POLY_APPROX, 1);
    }

    return contours;
}

_Bool isFigure(CvSeq *contours)
{
    if((contours->flags & CV_SEQ_FLAG_HOLE) == 0) {
        return 0;
    }

    double countour_area = cvContourArea(contours, CV_WHOLE_SEQ, 0);

    if(countour_area < MAX_FIGURE_AREA && countour_area > MIN_FIGURE_AREA) {
        return 1;
    }

    return 0;
}


static CvSeq *findFigure(CvSeq *in)
{
    if(!in) {
        return 0;
    }

    CvSeq *horizontal_sequence = in;

    while(horizontal_sequence) {
        if(isFigure(horizontal_sequence)) {
            return horizontal_sequence;
        }


        CvSeq *temp = findFigure(horizontal_sequence->v_next);

        if(temp) {
            return temp;
        }

        horizontal_sequence = horizontal_sequence->h_next;
    }

    return 0;
}

CvSeq *findFirstFigure(CvMemStorage *storage, IplImage *image_yuv)
{
    CvSeq *figure_contours = NULL;

    IplImage *image_black_white = thresholdImage(image_yuv, GREEN_SQUARE_YUV_LOWER_BOUND, GREEN_SQUARE_YUV_UPPER_BOUND,
                                  ERODE_DILATE_PIXELS);

    struct Square squares[1];

    if(findGreenSquares(storage, image_black_white, squares, sizeof(squares) / sizeof(struct Square)) >= 1) {
        IplImage *square_image = imageInsideSquare(image_yuv, squares[0]);
        IplImage *square_image_black_white = thresholdImage3D(square_image, ERODE_DILATE_PIXELS);
        cvReleaseImage(&square_image);

        figure_contours = findFigureContours(storage, square_image_black_white);
        cvReleaseImage(&square_image_black_white);
        figure_contours = findFigure(figure_contours);
    }

    cvReleaseImage(&image_black_white);
    return figure_contours;
}

static _Bool isCircle(CvSeq *in)
{
    CvPoint point = *(CvPoint *)cvGetSeqElem(in, 0);

    double contour_area = fabs(cvContourArea(in, CV_WHOLE_SEQ, 0));
    double contour_perimeter = cvArcLength(in, CV_WHOLE_SEQ, 1);

    if(contour_area < 100.0) {
        return 0;
    }

    double comp_area = contour_perimeter * contour_perimeter / (3.14159 * 4);

    if(contour_area * 0.96 < comp_area && comp_area < contour_area * 1.04) {
        return 1;
    }

    return 0;
}

#define OBSTACLE_CIRCLE_AREA 1465.0

static _Bool isCircleObstacle(CvSeq *in)
{
    if((in->flags & CV_SEQ_FLAG_HOLE) != 0) {
        return 0;
    }

    if(!isCircle(in)) {
        return 0;
    }

    double area = fabs(cvContourArea(in, CV_WHOLE_SEQ, 0));

    if(OBSTACLE_CIRCLE_AREA * 0.90 < area && area < OBSTACLE_CIRCLE_AREA * 1.10) {
        return 1;
    }

    return 0;
}

static CvPoint circleCenter(CvSeq *in)
{
    unsigned int max_x = 0, max_y = 0;
    unsigned int min_x = -1, min_y = -1;
    unsigned int i;

    for(i = 0; i < in->total; ++i) {
        CvPoint point = *(CvPoint *)cvGetSeqElem(in, i);

        if(point.x > max_x) {
            max_x = point.x;
        }

        if(point.x < min_x) {
            min_x = point.x;
        }

        if(point.y > max_y) {
            max_y = point.y;
        }

        if(point.y < min_y) {
            min_y = point.y;
        }
    }

    CvPoint point;
    point.x = min_x + ((max_x - min_x) / 2);
    point.y = min_y + ((max_y - min_y) / 2);
    return point;
}

#define OBSTACLE_TRIANGLE_AREA 1150.0

static _Bool isTriangleObstacle(CvSeq *in)
{
    if((in->flags & CV_SEQ_FLAG_HOLE) != 0) {
        return 0;
    }

    double area = fabs(cvContourArea(in, CV_WHOLE_SEQ, 0));

    if(OBSTACLE_TRIANGLE_AREA * 0.90 < area && area < OBSTACLE_TRIANGLE_AREA * 1.10) {
        return 1;
    }

    return 0;
}

#define OBSTACLE_TRIANGLE_POLY_APPROX 10

static _Bool trianglePoints(CvPoint points[3], CvSeq *in)
{
    CvSeq *approxed = cvApproxPoly(in, sizeof(CvContour), in->storage, CV_POLY_APPROX_DP, OBSTACLE_TRIANGLE_POLY_APPROX, 1);

    if(approxed->total != 3) {
        return 0;
    }

    double smallest_dist = 1 << 30;
    unsigned int index_smallest_dist = -1;

    unsigned int i;

    for(i = 0; i < 3; ++i) {
        CvPoint point1 = *(CvPoint *)cvGetSeqElem(approxed, i);
        CvPoint point2 = *(CvPoint *)cvGetSeqElem(approxed, (i + 1) % 3);

        double distance = ((point1.y - point2.y) * (point1.y - point2.y)) + ((point1.x - point2.x) * (point1.x - point2.x));

        if(distance < smallest_dist) {
            smallest_dist = distance;
            index_smallest_dist = i;
        }
    }

    for(i = 0; i < 3; ++i) {
        points[i] = *(CvPoint *)cvGetSeqElem(approxed, (i + index_smallest_dist + 2) % 3);
    }

    return 1;
}

static CvPoint midpoint(CvPoint point1, CvPoint point2)
{
    CvPoint midpoint;

    midpoint.x = point1.x + ((point2.x - point1.x) / 2);
    midpoint.y = point1.y + ((point2.y - point1.y) / 2);
    return midpoint;
}

static CvPoint triangleCenter(CvPoint points[3])
{
    CvPoint center;

    center = midpoint(points[1], points[2]);
    center = midpoint(points[0], center);
    return center;
}

static double triangleAngle(CvPoint points[3])
{
    CvPoint mid;

    mid = midpoint(points[1], points[2]);

    return atan2(points[0].y - mid.y, points[0].x - mid.x);
}


static _Bool isPossibleObstacle(CvSeq *in)
{
    if((in->flags & CV_SEQ_FLAG_HOLE) == 0) {
        return 0;
    }

    return isCircle(in);
}

static void findObstaclesRecursive(CvSeq *contours, struct Obstacle *out_obstacles, unsigned int max_obstacles,
                                   unsigned int *num_obstacles)
{
    if(*num_obstacles >= max_obstacles) {
        return;
    }

    CvSeq *horizontal_sequence = contours;

    while(horizontal_sequence) {
        if(isPossibleObstacle(horizontal_sequence)) {
            if(horizontal_sequence->v_next) {
                struct Obstacle obstacle;
                obstacle.type = OBSTACLE_NONE;
                CvSeq *obstacle_seq = horizontal_sequence->v_next;

                if(isCircleObstacle(obstacle_seq)) {
                    obstacle.type = OBSTACLE_CIRCLE;
                    CvPoint point = circleCenter(obstacle_seq);
                    obstacle.x = point.x;
                    obstacle.y = point.y;
                    obstacle.angle = 0;
                } else if(isTriangleObstacle(obstacle_seq)) {
                    CvPoint points[3];

                    if(trianglePoints(points, obstacle_seq)) {
                        obstacle.type = OBSTACLE_TRIANGLE;
                        CvPoint point = triangleCenter(points);
                        obstacle.x = point.x;
                        obstacle.y = point.y;
                        obstacle.angle = triangleAngle(points);
                    }
                }

                if(obstacle.type != OBSTACLE_NONE) {
                    out_obstacles[*num_obstacles] = obstacle;
                    ++*num_obstacles;

                    if(*num_obstacles >= max_obstacles) {
                        return;
                    }
                }
            }
        }

        findObstaclesRecursive(horizontal_sequence->v_next, out_obstacles, max_obstacles, num_obstacles);
        horizontal_sequence = horizontal_sequence->h_next;
    }
}

#define OBSTACLE_POLY_APPROX 1

unsigned int findObstacles(CvMemStorage *storage, struct Obstacle *obstacles_out, unsigned int max_obstacles,
                           IplImage *image_yuv)
{
    IplImage *image_black_white = thresholdImage(image_yuv, OBSTACLES_YUV_LOWER_BOUND, OBSTACLES_YUV_UPPER_BOUND,
                                  ERODE_DILATE_PIXELS);

    CvSeq *contours = 0;
    unsigned int num_obstacles = 0;

    if(cvFindContours(image_black_white, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE,
                      cvPoint(0, 0))) {
        contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, OBSTACLE_POLY_APPROX, 1);

        findObstaclesRecursive(contours, obstacles_out, max_obstacles, &num_obstacles);
    }

    cvReleaseImage(&image_black_white);
    return num_obstacles;
}
