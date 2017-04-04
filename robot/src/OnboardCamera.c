#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "OnboardCamera.h"
#include "vision.h"
#include "Robot.h"

#define CALIBRATION_FILE "utils/camera_calibration.xml"
#define CAMERA_INDEX 0

static CvCapture *cv_cap;
CvMat *camera_matrix = 0;
CvMat *distortion_coeffs = 0;
static int counter = 0;

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

void OnboardCamera_freeCamera(void)
{
    cvReleaseCapture(&(cv_cap));
}

#define OPENCV_IMAGE_BUFFER_LENGTH (30)

/* NOTE: returned image is in yuv color space and must be freed.
 */
static IplImage *getImage(void)
{

    int buffer = 0;
    IplImage *image;

    do {
        image = cvQueryFrame(cv_cap);
        ++buffer;
    } while(buffer <= OPENCV_IMAGE_BUFFER_LENGTH);

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

#define LINE_SIZE  30

struct CoordinatesSequence *OnboardCamera_extractTrajectoryFromImage(IplImage **painting_image)
{
    CvMemStorage *opencv_storage = cvCreateMemStorage(0);
    IplImage *image = getImage();

    struct CoordinatesSequence *sequence = NULL;


    if(image != NULL) {

        CvSeq *opencv_sequence = findFirstFigure(opencv_storage, image, painting_image);

        if(opencv_sequence) {
            if(painting_image && *painting_image) {
                cvDrawContours(*painting_image, opencv_sequence, CV_RGB(255, 0, 0), CV_RGB(255, 0, 0), 0, LINE_SIZE, 8,
                               cvPoint(0, 0));

                IplImage *image_temp = cvCloneImage(*painting_image);
                cvCvtColor(*painting_image, image_temp, CV_YCrCb2RGB);
                cvCopy(image_temp, *painting_image, NULL);
                cvReleaseImage(&image_temp);
            }

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

            // Loops the trajectory
            struct Coordinates *first_coordinates = Coordinates_zero();
            Coordinates_copyValuesFrom(first_coordinates, sequence->coordinates);
            CoordinatesSequence_append(sequence, first_coordinates);
            Coordinates_delete(first_coordinates);
        }

        cvReleaseMemStorage(&opencv_storage);
    }

    return sequence;
}

void OnboardCamera_deleteImage(IplImage **image)
{
    cvReleaseImage(image);
}

void OnboardCamera_takePictureAndIfValidSendAndUpdateDrawingBaseTrajectory(struct Robot *robot)
{
    IplImage *image;
    struct CoordinatesSequence *image_trajectory;
    image_trajectory = OnboardCamera_extractTrajectoryFromImage(&image);

    if(image_trajectory != NULL) {

        Flags_setPictureTaken(robot->current_state->flags, TRUE);
        DataSender_sendImage(robot->data_sender, image);
        robot->drawing_trajectory = image_trajectory;
    } else {
        char name[20];
        sprintf(name, "image-%d.jpg", counter);
        cvSaveImage(name, image, 0);
        counter++;
    }
}
