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

/* Upper bound of YUV channel coordinates to detect the green square.
 */
#define GREEN_SQUARE_YUV_UPPER_BOUND (cvScalar(255, 120, 120, 255))

/* Lower bound of YUV channel coordinates to detect the green square.
 */
#define GREEN_SQUARE_YUV_LOWER_BOUND (cvScalar(10, 0, 0, 255))

/* Upper bound of Y channel coordinates to detect the figure.
 */
#define FIGURE_Y_UPPER_BOUND (150)

/* Min distance between UV coordinates and the center to detect figure.
 */
#define FIGURE_UV_MIN_DISTANCE (16)



#define GREEN_SQUARE_POLY_APPROX 100
#define FIGURE_POLY_APPROX 5

#define WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE 1000

#define MIN_FIGURE_AREA (WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE * WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE * 0.05)
#define MAX_FIGURE_AREA (WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE * WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE * 0.9)


typedef struct {
    CvPoint2D32f corner[4];
} Square;

/* helper function:
 * finds a cosine of angle between vectors
 * from pt0->pt1 and from pt0->pt2
 */
static double angle(CvPoint *pt1, CvPoint *pt2, CvPoint *pt0)
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

static _Bool detect_square(CvSeq *contours)
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

static _Bool detect_dual_square(CvSeq *contours)
{
    if(contours->v_next && detect_square(contours) && detect_square(contours->v_next)) {
        return 1;
    }

    return 0;
}

static IplImage *threshold_image(IplImage *image_yuv, CvScalar lower_bound, CvScalar upper_bound,
                                 unsigned int erode_dilate_pixels)
{
    IplImage *image_black_white = cvCreateImage(cvGetSize(image_yuv), IPL_DEPTH_8U, 1);
    cvInRangeS(image_yuv, lower_bound, upper_bound, image_black_white);

    cvDilate(image_black_white, image_black_white, NULL, erode_dilate_pixels);
    cvErode(image_black_white, image_black_white, NULL, erode_dilate_pixels * 2);
    cvDilate(image_black_white, image_black_white, NULL, erode_dilate_pixels);
    return image_black_white;
}


static IplImage *threshold_image_3d(IplImage *image_yuv, unsigned int erode_dilate_pixels)
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

            int px = (u - 128);
            int py = (v - 128);

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


static void find_green_squares_recursive(CvSeq *contours, Square *out_squares, unsigned int max_squares,
        unsigned int *num_squares)
{
    if(*num_squares >= max_squares) {
        return;
    }

    CvSeq *h_seq = contours;

    while(h_seq) {
        if(detect_dual_square(h_seq)) {
            Square square;
            square.corner[0] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(h_seq->v_next, 0));
            square.corner[1] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(h_seq->v_next, 1));
            square.corner[2] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(h_seq->v_next, 2));
            square.corner[3] = cvPointTo32f(*(CvPoint *)cvGetSeqElem(h_seq->v_next, 3));

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

        find_green_squares_recursive(h_seq->v_next, out_squares, max_squares, num_squares);
        h_seq = h_seq->h_next;
    }
}

static unsigned int find_green_squares(CvMemStorage *storage, IplImage *image_black_white, Square *out_squares,
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
        find_green_squares_recursive(contours, out_squares, max_squares, &num_squares);
    }

    return num_squares;
}

static IplImage *image_inside_square(IplImage *image_yuv, Square square)
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

static CvSeq *find_figure_contours(CvMemStorage *storage, IplImage *image_black_white)
{
    CvSeq *contours = NULL;

    if(cvFindContours(image_black_white, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE,
                      cvPoint(0, 0))) {
        contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, FIGURE_POLY_APPROX, 1);
    }

    return contours;
}

static _Bool detect_figure(CvSeq *contours)
{
    double countour_area = cvContourArea(contours, CV_WHOLE_SEQ, 0);

    if(countour_area < MAX_FIGURE_AREA && countour_area > MIN_FIGURE_AREA) {
        return 1;
    }

    return 0;
}


static CvSeq *find_figure(CvSeq *in)
{
    if(!in) {
        return 0;
    }

    CvSeq *h_seq = in;

    while(h_seq) {
        if(detect_figure(h_seq)) {
            return h_seq;
        }


        CvSeq *temp = find_figure(h_seq->v_next);

        if(temp) {
            return temp;
        }

        h_seq = h_seq->h_next;
    }

    return 0;
}

CvSeq *find_first_figure(CvMemStorage *storage, IplImage *image_yuv)
{
    CvSeq *figure_contours = NULL;

    IplImage *image_black_white = threshold_image(image_yuv, GREEN_SQUARE_YUV_LOWER_BOUND, GREEN_SQUARE_YUV_UPPER_BOUND,
                                  ERODE_DILATE_PIXELS);

    Square squares[1];

    if(find_green_squares(storage, image_black_white, squares, sizeof(squares) / sizeof(Square)) >= 1) {
        IplImage *square_image = image_inside_square(image_yuv, squares[0]);
        IplImage *square_image_black_white = threshold_image_3d(square_image, ERODE_DILATE_PIXELS);
        cvReleaseImage(&square_image);

        figure_contours = find_figure_contours(storage, square_image_black_white);
        cvReleaseImage(&square_image_black_white);
        figure_contours = find_figure(figure_contours);
    }

    cvReleaseImage(&image_black_white);
    return figure_contours;
}
