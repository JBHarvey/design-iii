
#include "Camera.h"
#define CALIBRATION_FILE "camera_calibration.xml"
#define CAMERA_INDEX 0

static CvCapture *cv_cap;
CvMat *camera_matrix = 0;
CvMat *distortion_coeffs = 0;

void Camera_init()
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    camera_matrix = (CvMat *)cvLoad(CALIBRATION_FILE, opencv_storage, "Camera_Matrix", 0);
    distortion_coeffs = (CvMat *)cvLoad(CALIBRATION_FILE, opencv_storage, "Distortion_Coefficients", 0);

    if(camera_matrix && distortion_coeffs) {
        cv_cap = cvCaptureFromCAM(CAMERA_INDEX);
        cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_WIDTH, 1600);
        cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_HEIGHT, 1200);
    }
}

/* NOTE: returned image is in yuv color space and must be freed.
 */
IplImage *Camera_get_image()
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
        return 0;
    }
}
