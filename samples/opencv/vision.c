#include <stdio.h>
#include "vision.h"

/* Used by the square finding algorithm
 * If an angle has a cosine bigger than this value the shape is not considered a square
 */
#define MAX_SQUARE_COSINE 0.4

/* Number of pixels that the black and white images will be eroded then dilated to make
 * the edges cleaner.
 */
#define ERODE_DILATE_PIXELS 4

/* Upper bound of YUV channel coordinates to detect the green square.
 */
#define GREEN_SQUARE_YUV_UPPER_BOUND (cvScalar(255, 120, 120, 255))

/* Lower bound of YUV channel coordinates to detect the green square.
 */
#define GREEN_SQUARE_YUV_LOWER_BOUND (cvScalar(10, 0, 0, 255))

/* Upper bound of YUV channel coordinates to detect the white around the figure.
 */
#define FIGURE_YUV_UPPER_BOUND (cvScalar(256, 139, 139, 255))

/* Lower bound of YUV channel coordinates to detect the white around the figure.
 */
#define FIGURE_YUV_LOWER_BOUND (cvScalar(110, 115, 115, 255))


#define GREEN_SQUARE_POLY_APPROX 200
#define FIGURE_POLY_APPROX 20

#define WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE 1000

typedef struct {
    CvPoint2D32f corner[4];
} Square;

/* helper function:
 * finds a cosine of angle between vectors
 * from pt0->pt1 and from pt0->pt2
 */
static double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0)
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

static _Bool detect_square(CvSeq* contours)
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
                                    (CvPoint*)cvGetSeqElem(contours, i),
                                    (CvPoint*)cvGetSeqElem(contours, i - 2),
                                    (CvPoint*)cvGetSeqElem(contours, i - 1)));
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

static _Bool detect_dual_square(CvSeq* contours)
{
    if(contours->v_next && detect_square(contours) && detect_square(contours->v_next)) {
        return 1;
    }

    return 0;
}

static IplImage* threshold_image(IplImage* img_yuv, CvScalar lower_bound, CvScalar upper_bound,
                                 unsigned int erode_dilate_pixels)
{
    IplImage* img_bw = cvCreateImage(cvGetSize(img_yuv), IPL_DEPTH_8U, 1);
    cvInRangeS(img_yuv, lower_bound, upper_bound, img_bw);

    cvDilate(img_bw, img_bw, NULL, erode_dilate_pixels);
    cvErode(img_bw, img_bw, NULL, erode_dilate_pixels);
    cvErode(img_bw, img_bw, NULL, erode_dilate_pixels);
    cvDilate(img_bw, img_bw, NULL, erode_dilate_pixels);
    return img_bw;
}


static unsigned int find_green_squares(CvMemStorage* storage, IplImage* img_bw, Square *out_squares,
                                       unsigned int max_squares)
{
    unsigned int num_squares = 0;

    if(max_squares == 0) {
        return 0;
    }

    CvSeq* contours = 0;

    if(cvFindContours(img_bw, storage, &contours, sizeof(CvContour),
                      CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0))) {

        contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, GREEN_SQUARE_POLY_APPROX, 1);

        CvSeq* h_seq = contours;

        while(h_seq) {
            CvSeq* v_seq = h_seq;

            while(v_seq) {
                if(detect_dual_square(v_seq)) {
                    Square square;
                    square.corner[0] = cvPointTo32f(*(CvPoint*)cvGetSeqElem(v_seq->v_next, 0));
                    square.corner[1] = cvPointTo32f(*(CvPoint*)cvGetSeqElem(v_seq->v_next, 1));
                    square.corner[2] = cvPointTo32f(*(CvPoint*)cvGetSeqElem(v_seq->v_next, 2));
                    square.corner[3] = cvPointTo32f(*(CvPoint*)cvGetSeqElem(v_seq->v_next, 3));

                    /* fix rotation */
                    if(square.corner[0].x * square.corner[0].y > square.corner[3].x * square.corner[3].y) {
                        CvPoint2D32f temp = square.corner[0];
                        square.corner[0] = square.corner[3];
                        square.corner[3] = square.corner[2];
                        square.corner[2] = square.corner[1];
                        square.corner[1] = temp;
                    }

                    out_squares[num_squares] = square;
                    ++num_squares;

                    if(num_squares >= max_squares) {
                        return num_squares;
                    }
                }

                v_seq = v_seq->v_next;
            }

            h_seq = h_seq->h_next;
        }
    }

    return num_squares;
}

static IplImage* image_inside_square(IplImage* img_yuv, Square square)
{
    CvPoint2D32f dst[4];
    dst[0] = cvPoint2D32f(0, 0);
    dst[1] = cvPoint2D32f(WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE, 0);
    dst[2] = cvPoint2D32f(WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE, WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE);
    dst[3] = cvPoint2D32f(0, WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE);

    CvMat *perspective_matrix = cvCreateMat(3, 3, CV_32F);
    perspective_matrix = cvGetPerspectiveTransform(square.corner, dst, perspective_matrix);

    IplImage* corrected_image = cvCreateImage(cvSize(WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE,
                                WIDTH_HEIGHT_EXTRACTED_SQUARE_IMAGE), IPL_DEPTH_8U, 3);

    cvWarpPerspective(img_yuv, corrected_image, perspective_matrix, CV_INTER_LINEAR, cvScalarAll(0));
    cvReleaseMat(&perspective_matrix);
    return corrected_image;
}

static CvSeq *find_figure_contours(CvMemStorage* storage, IplImage* img_bw)
{
    CvSeq* contours = NULL;

    if(cvFindContours(img_bw, storage, &contours, sizeof(CvContour), CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0))) {
        contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, FIGURE_POLY_APPROX, 1);
    }

    return contours;
}

CvSeq *find_first_figure(CvMemStorage* storage, IplImage* img_yuv)
{
    CvSeq *figure_contours = NULL;

    IplImage *img_bw = threshold_image(img_yuv, GREEN_SQUARE_YUV_LOWER_BOUND, GREEN_SQUARE_YUV_UPPER_BOUND,
                                       ERODE_DILATE_PIXELS);

    Square squares[1];

    if(find_green_squares(storage, img_bw, squares, sizeof(squares) / sizeof(Square)) >= 1) {
        IplImage *square_image = image_inside_square(img_yuv, squares[0]);
        IplImage *square_image_bw = threshold_image(square_image, FIGURE_YUV_LOWER_BOUND, FIGURE_YUV_UPPER_BOUND,
                                    ERODE_DILATE_PIXELS);
        cvReleaseImage(&square_image);

        figure_contours = find_figure_contours(storage, square_image_bw);
        cvReleaseImage(&square_image_bw);
    }

    cvReleaseImage(&img_bw);
    return figure_contours;
}
