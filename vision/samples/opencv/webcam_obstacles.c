
#include "opencv2/highgui/highgui_c.h"
#include "vision.h"
#include "markers.h"
#include <stdio.h>
#include <math.h>

#define SIZE_DRAW_LINES 3

int main(int argc, char *argv[])
{
    int c;
    IplImage *img;
    CvCapture *cv_cap = cvCaptureFromCAM(1);
    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_WIDTH, 1600);
    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_HEIGHT, 1200);

    CvMat *camera_matrix = 0;
    CvMat *distortion_coeffs = 0;
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);

    if(argc > 1) {
        camera_matrix = (CvMat *)cvLoad(argv[1], opencv_storage, "Camera_Matrix", 0);
        distortion_coeffs = (CvMat *)cvLoad(argv[1], opencv_storage, "Distortion_Coefficients", 0);
    }

    //cvNamedWindow("Video", 0); // create window
    //cvNamedWindow("Video-orig", 0); // create window
    cvNamedWindow("Video-lines", 0); // create window
    //cvNamedWindow("Video-innersquare", 0); // create window

    for(;;) {
        img = cvQueryFrame(cv_cap); // get frame
        //img = cvLoadImage(argv[1],CV_LOAD_IMAGE_COLOR);

        if(img != 0) {
            //cvShowImage("Video-orig", img);

            struct Marker marker = detectMarker(img);

            if(camera_matrix) {
                IplImage *image_temp = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
                cvUndistort2(img, image_temp, camera_matrix, distortion_coeffs, 0);
                img = image_temp;
            }

            IplImage *img_yuv = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
            cvCvtColor(img, img_yuv, CV_BGR2YCrCb);
            struct Obstacle obstacles[20];
            int num_obstacles = findObstacles(opencv_storage, obstacles, 20, img_yuv);
            cvReleaseImage(&img_yuv);

            IplImage *im_square_image = img;//cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);

            if(num_obstacles) {
                printf("num_obstacles %i\n", num_obstacles);
                unsigned int i;

                for(i = 0; i < num_obstacles; ++i) {
                    CvPoint obstacle_point = coordinateToTableCoordinate(cvPoint(obstacles[i].x, obstacles[i].y), 34.0 + 7.6, cvPoint(800,
                                             600));

                    if(obstacles[i].type == OBSTACLE_CIRCLE) {
                        cvCircle(im_square_image, obstacle_point, 22, CV_RGB(255, 0, 0), SIZE_DRAW_LINES, 8, 0);
                        cvCircle(im_square_image, obstacle_point, 40, CV_RGB(255, 0, 0), SIZE_DRAW_LINES, 8, 0);
                    }

                    if(obstacles[i].type == OBSTACLE_TRIANGLE) {
                        cvLine(im_square_image, obstacle_point, cvPoint(obstacle_point.x + 40 * cos(obstacles[i].angle),
                                obstacle_point.y + 40 * sin(obstacles[i].angle)), CV_RGB(0, 255, 0), SIZE_DRAW_LINES, 8, 0);
                        cvCircle(im_square_image, obstacle_point, 40, CV_RGB(0, 255, 0), SIZE_DRAW_LINES, 8, 0);
                    }
                }

                //cvDrawContours(im_square_image, contour, CV_RGB(255, 0, 0), CV_RGB(0, 255, 0), 0, 3, CV_AA, cvPoint(0, 0));
            }

            if(marker.valid) {
                cvLine(im_square_image, cvPoint(marker.x, marker.y), cvPoint(marker.x + 40 * cos(marker.angle),
                        marker.y + 40 * sin(marker.angle)), CV_RGB(255, 0, 255), SIZE_DRAW_LINES, 8, 0);
                cvCircle(im_square_image, cvPoint(marker.x, marker.y), 40, CV_RGB(255, 0, 255), SIZE_DRAW_LINES, 8, 0);
            }

            cvShowImage("Video-lines", im_square_image);
            //cvReleaseImage(&im_square_image);
            //cvReleaseImage(&img);

            if(camera_matrix) {
                cvReleaseImage(&img);
            }
        }

        c = cvWaitKey(10); // wait 10 ms or for key stroke

        if(c == 27) {
            break;    // if ESC, break and quit
        }
    }

    /* clean up */
    cvReleaseCapture(&cv_cap);
    cvReleaseMemStorage(&opencv_storage);
    cvDestroyWindow("Video-orig");
    cvDestroyWindow("Video-lines");
}
