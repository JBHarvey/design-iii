
#include "opencv2/highgui/highgui_c.h"
#include "vision.h"
#include <stdio.h>


int main(int argc, char *argv[])
{
    int c;
    IplImage *img = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);

    //cvNamedWindow("Video", 0); // create window
    cvNamedWindow("Video-orig", 0); // create window
    cvNamedWindow("Video-lines", 0); // create window
    //cvNamedWindow("Video-innersquare", 0); // create window
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);


    if(img != 0) {
        cvShowImage("Video-orig", img);
        cvSmooth(img, img, CV_GAUSSIAN, 5, 0, 0, 0);
        IplImage *img_yuv = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
        cvCvtColor(img, img_yuv, CV_BGR2YCrCb);
        CvSeq *contour = findFirstFigure(opencv_storage, img_yuv, NULL);
        cvReleaseImage(&img_yuv);

        IplImage *im_square_image = cvCreateImage(cvSize(1000, 1000), IPL_DEPTH_8U, 3);

        if(contour) {
            cvDrawContours(im_square_image, contour, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), 8, 3, CV_AA, cvPoint(0, 0));
        }

        cvShowImage("Video-lines", im_square_image);
        cvReleaseImage(&im_square_image);
        cvReleaseImage(&img);
    }

    c = cvWaitKey(0);

    /* clean up */
    cvReleaseMemStorage(&opencv_storage);
    cvDestroyWindow("Video-orig");
    cvDestroyWindow("Video-lines");
}
