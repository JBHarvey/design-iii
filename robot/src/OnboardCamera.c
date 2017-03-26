#include <math.h>
#include "OnboardCamera.h"
#include "vision.h"

#define CALIBRATION_FILE "utils/camera_calibration.xml"
#define CAMERA_INDEX 0

static CvCapture *cv_cap;
CvMat *camera_matrix = 0;
CvMat *distortion_coeffs = 0;

void OnboardCamera_init(void)
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    camera_matrix = (CvMat *)cvLoad(CALIBRATION_FILE, opencv_storage, "Camera_Matrix", 0);
    distortion_coeffs = (CvMat *)cvLoad(CALIBRATION_FILE, opencv_storage, "Distortion_Coefficients", 0);

    if(camera_matrix && distortion_coeffs) {
        #if (CV_MINOR_VERSION > 1)
        cv_cap = cvCreateCameraCapture(CAMERA_INDEX);
        #else
        cv_cap = cvCaptureFromCAM(CAMERA_INDEX);
        #endif
        cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_WIDTH, 1600);
        cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_HEIGHT, 1200);
    }
}

/* NOTE: returned image is in yuv color space and must be freed.
 */
static IplImage *getImage(void)
{
    IplImage *image = cvQueryFrame(cv_cap);

    if(camera_matrix && distortion_coeffs) {
        IplImage *image_temp = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
        cvUndistort2(image, image_temp, camera_matrix, distortion_coeffs, 0);
        IplImage *image_yuv = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
        cvCvtColor(image_temp, image_yuv, CV_BGR2YCrCb);
        cvReleaseImage(&image_temp);
        return image_yuv;
    } else {
        return image;
    }
}

#define SIZE_SIDE_IN 1000.0
#define SIZE_SIDE_OUT (5900.0 / 4.0)

static int convertToCartesian(double coord)
{
    return round((coord - (SIZE_SIDE_IN / 2.0)) * (SIZE_SIDE_OUT / SIZE_SIDE_IN));
}

#define LINE_SIZE 3

struct CoordinatesSequence *OnboardCamera_extractTrajectoryFromImage(IplImage **image_yuv_in_green_square)
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    IplImage *image = getImage();

    struct CoordinatesSequence *sequence = NULL;


    if(image != NULL) {

        CvSeq *opencv_sequence = findFirstFigure(opencv_storage, image, image_yuv_in_green_square);

        cvDrawContours(*image_yuv_in_green_square, opencv_sequence, CV_RGB(255, 0, 0), CV_RGB(255, 0, 0), 0, LINE_SIZE, 8,
                       cvPoint(0, 0));

        if(opencv_sequence) {
            unsigned int i;

            for(i = 0; i < opencv_sequence->total; ++i) {
                CvPoint *element_pointer = (CvPoint *)cvGetSeqElem(opencv_sequence, i);
                struct Coordinates *coordinates = Coordinates_new(convertToCartesian(element_pointer->x),
                                                  convertToCartesian(element_pointer->y) * (-1));

                if(!i) {
                    sequence = CoordinatesSequence_new(coordinates);
                } else {
                    CoordinatesSequence_append(sequence, coordinates);
                }

                Coordinates_delete(coordinates);
            }
        }

        cvReleaseMemStorage(&opencv_storage);
    }


    return sequence;
}

void OnboardCamera_deleteImageAndFreeCamera(IplImage **image)
{
    cvReleaseImage(image);
    cvReleaseCapture(&(cv_cap));
}

