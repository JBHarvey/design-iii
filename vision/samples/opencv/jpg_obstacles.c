
#include "opencv2/highgui/highgui_c.h"
#include "vision.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[])
{
    int c;
    IplImage *img;
    CvMat *camera_matrix = 0;
    CvMat *distortion_coeffs = 0;
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);

    if(argc > 2) {
        camera_matrix = (CvMat *)cvLoad(argv[2], opencv_storage, "Camera_Matrix", 0);
        distortion_coeffs = (CvMat *)cvLoad(argv[2], opencv_storage, "Distortion_Coefficients", 0);
    }

    //cvNamedWindow("Video", 0); // create window
    cvNamedWindow("Video-orig", 0); // create window
    cvNamedWindow("Video-lines", 0); // create window
    //cvNamedWindow("Video-cont", 0); // create window
    //cvNamedWindow("Video-bw", 0); // create window
    {
        img = cvLoadImage(argv[1], CV_LOAD_IMAGE_COLOR);

        if(img != 0) {
            if(camera_matrix) {
                IplImage *image_temp = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
                cvUndistort2(img, image_temp, camera_matrix, distortion_coeffs, 0);
                cvReleaseImage(&img);
                img = image_temp;
            }

            IplImage *img_yuv = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
            cvCvtColor(img, img_yuv, CV_BGR2YCrCb);
            struct Vision_Obstacle obstacles[20];
            int num_obstacles = findObstacles(opencv_storage, obstacles, 20, img_yuv);

            struct Square square;
            if (findTableCorners(img_yuv, &square)) {
                unsigned int i;

                for(i = 0; i < 4; ++i) {
                    cvLine(img, fixedCvPointFrom32f(square.corner[i]), fixedCvPointFrom32f(square.corner[(i + 1) % 4]), CV_RGB(255, 0, 255), 1, 8, 0);
                }
            }

            cvReleaseImage(&img_yuv);

            IplImage *im_square_image = img;
            if(num_obstacles) {
                printf("num_obstacles %i\n", num_obstacles);
                unsigned int i;

                for(i = 0; i < num_obstacles; ++i) {
                    if(obstacles[i].type == OBSTACLE_CIRCLE) {
                        cvCircle(im_square_image, cvPoint(obstacles[i].x, obstacles[i].y), 22, CV_RGB(255, 0, 0), 1, 8, 0);
                        cvCircle(im_square_image, cvPoint(obstacles[i].x, obstacles[i].y), 40, CV_RGB(255, 0, 0), 1, 8, 0);
                    }

                    if(obstacles[i].type == OBSTACLE_TRIANGLE) {
                        printf("%i %i\n", obstacles[i].x, obstacles[i].y);
                        cvLine(im_square_image, cvPoint(obstacles[i].x, obstacles[i].y), cvPoint(obstacles[i].x + 40 * cos(obstacles[i].angle),
                                obstacles[i].y + 40 * sin(obstacles[i].angle)), CV_RGB(0, 255, 0), 1, 8, 0);
                        cvCircle(im_square_image, cvPoint(obstacles[i].x, obstacles[i].y), 40, CV_RGB(0, 255, 0), 1, 8, 0);
                    }
                }
            }

            cvShowImage("Video-lines", im_square_image);
            cvReleaseImage(&im_square_image);
            //cvReleaseImage(&img);

            //if(camera_matrix)
            //cvReleaseImage(&img);
        }

        c = cvWaitKey(0); // wait inf ms or for key stroke

    }

    /* clean up */
    cvReleaseMemStorage(&opencv_storage);
    cvDestroyWindow("Video-orig");
    cvDestroyWindow("Video-lines");
}
