

#include "opencv2/highgui/highgui_c.h"
#include "vision.h"
#include <stdio.h>

#ifndef D_TOLERANCE
    #define D_TOLERANCE (66.0)
#endif

#define OFF_AREA_TOLERANCE (60.0 / 1000.0)
#define OFF_PIXEL_TOLERANCE ((unsigned int)(1000.0 / D_TOLERANCE))

static _Bool hasNonblackPixels(IplImage *image)
{
    unsigned int size = image->imageSize;
    char *image_data = image->imageData;

    unsigned int i;

    for(i = 0; i < size; ++i) {
        if(image_data[i]) {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if(argc < 4) {
        return 1;
    }

    _Bool non_black_pixels = 1;

    CvMemStorage *opencv_storage = cvCreateMemStorage(0);

    IplImage *image = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);
    CvSeq *contours_compare = (CvSeq *)cvLoad(argv[2], opencv_storage, 0, 0);

    CvMat *camera_matrix = (CvMat *)cvLoad(argv[3], opencv_storage, "Camera_Matrix", 0);
    CvMat *distortion_coeffs = (CvMat *)cvLoad(argv[3], opencv_storage, "Distortion_Coefficients", 0);

    //cvNamedWindow("Video", 0); // create window
    //cvNamedWindow("Video-lines", 0); // create window
    //cvNamedWindow("Video-innersquare", 0); // create window

    if(image && camera_matrix && distortion_coeffs) {
        //cvShowImage("Video-orig", image);
        //cvSmooth(image, image, CV_GAUSSIAN, 3, 0, 0, 0);

        if(image->width == 1600) {
            IplImage *image_temp = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
            cvUndistort2(image, image_temp, camera_matrix, distortion_coeffs, 0);
            cvReleaseImage(&image);
            image = image_temp;
        }

        cvReleaseMat(&camera_matrix);
        cvReleaseMat(&distortion_coeffs);

        //cvShowImage("Video-orig", image);
        IplImage *image_yuv = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
        cvCvtColor(image, image_yuv, CV_BGR2YCrCb);
        CvSeq *contour = findFirstFigure(opencv_storage, image_yuv, NULL);
        cvReleaseImage(&image_yuv);

        IplImage *square_image = cvCreateImage(cvSize(1000, 1000), IPL_DEPTH_8U, 3);

        memset(square_image->imageData, 0, square_image->imageSize);

        if(contours_compare && contour) {
            #ifdef D_DEBUG
            cvDrawContours(square_image, contours_compare, CV_RGB(0, 255, 0), CV_RGB(0, 255, 0), 0, OFF_PIXEL_TOLERANCE, 8,
                           cvPoint(0, 0));
            cvDrawContours(square_image, contour, CV_RGB(255, 0, 0), CV_RGB(255, 0, 0), 0, 1, 8, cvPoint(0, 0));
            {
            #else
            double area = cvContourArea(contour, CV_WHOLE_SEQ, 0);
            double area_compare = cvContourArea(contours_compare, CV_WHOLE_SEQ, 0);

            if(area < area_compare * (1.0 + OFF_AREA_TOLERANCE) && area > area_compare * (1.0 - OFF_AREA_TOLERANCE)) {
                cvDrawContours(square_image, contour, CV_RGB(0, 255, 0), CV_RGB(0, 255, 0), 0, 1, 8, cvPoint(0, 0));
                cvDrawContours(square_image, contours_compare, CV_RGB(0, 0, 0), CV_RGB(0, 0, 0), 0, OFF_PIXEL_TOLERANCE, 8, cvPoint(0,
                               0));
            #endif
                non_black_pixels = hasNonblackPixels(square_image);
            }
        }
        #ifdef D_DEBUG
        cvShowImage("Video-lines", square_image);
        #endif
        cvReleaseImage(&square_image);
        cvReleaseImage(&image);
    }

    #ifdef D_DEBUG
    int c = cvWaitKey(0);
    #endif
    /* clean up */
    cvReleaseMemStorage(&opencv_storage);
    //cvDestroyWindow("Video-orig");
    //printf("%u\n", non_black_pixels);
    return non_black_pixels;
}
