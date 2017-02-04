 
 
#include "opencv2/highgui/highgui_c.h"
#include "vision.h"
#include <stdio.h>


#define OFF_AREA_TOLERANCE (60.0 / 1000.0)
#define OFF_PIXEL_TOLERANCE ((unsigned int)(1000.0 / 66.0))

static _Bool has_nonblack_pixels(IplImage* img)
{
    unsigned int size = img->imageSize;
    char *img_data = img->imageData;

    unsigned int i;
    for (i = 0; i < size; ++i) {
        if (img_data[i]) {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
        return 1;

    _Bool non_black_pixels = 1;

    CvMemStorage* storage = cvCreateMemStorage(0);

    IplImage* img = cvLoadImage(argv[1],CV_LOAD_IMAGE_COLOR);
    CvSeq *contours_comp = (CvSeq*)cvLoad(argv[2], storage, 0, 0);

    //cvNamedWindow("Video", 0); // create window
    //cvNamedWindow("Video-lines", 0); // create window
    //cvNamedWindow("Video-innersquare", 0); // create window

    if(img) {
        //cvShowImage("Video-orig", img);
        //cvSmooth(img, img, CV_GAUSSIAN, 3, 0, 0, 0);
        IplImage* img_yuv = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
        cvCvtColor(img, img_yuv, CV_BGR2YCrCb);
        CvSeq *contour = find_first_figure(storage, img_yuv);
        cvReleaseImage(&img_yuv);

        IplImage* im_square_image = cvCreateImage(cvSize(1000, 1000), IPL_DEPTH_8U, 3);

        memset(im_square_image->imageData, 0, im_square_image->imageSize);

        if (contours_comp && contour) {
#ifdef D_DEBUG
            cvDrawContours(im_square_image, contours_comp, CV_RGB(0, 255, 0), CV_RGB(0, 255, 0), 0, OFF_PIXEL_TOLERANCE, 8, cvPoint(0, 0));
            cvDrawContours(im_square_image, contour, CV_RGB(255, 0, 0), CV_RGB(255, 0, 0), 0, 1, 8, cvPoint(0, 0));
            {
#else
            double area = cvContourArea(contour, CV_WHOLE_SEQ, 0);
            double area_comp = cvContourArea(contours_comp, CV_WHOLE_SEQ, 0);

            if (area < area_comp * (1.0 + OFF_AREA_TOLERANCE) && area > area_comp * (1.0 - OFF_AREA_TOLERANCE)) {
                cvDrawContours(im_square_image, contour, CV_RGB(0, 255, 0), CV_RGB(0, 255, 0), 0, 1, 8, cvPoint(0, 0));
                cvDrawContours(im_square_image, contours_comp, CV_RGB(0, 0, 0), CV_RGB(0, 0, 0), 0, OFF_PIXEL_TOLERANCE, 8, cvPoint(0, 0));
#endif
                non_black_pixels = has_nonblack_pixels(im_square_image);
            }
        }
#ifdef D_DEBUG
        cvShowImage("Video-lines", im_square_image);
#endif
        cvReleaseImage(&im_square_image);
        cvReleaseImage(&img);
    }
#ifdef D_DEBUG
    int c = cvWaitKey(0);
#endif
    /* clean up */
    cvReleaseMemStorage(&storage);
    //cvDestroyWindow("Video-orig");
    //printf("%u\n", non_black_pixels);
    return non_black_pixels;
}
