#ifndef __WORLD_VISION
#define __WORLD_VISION

#include <gtk/gtk.h>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/videoio/videoio_c.h"
#include "CommunicationStructures.h"
#include "station_client.h"
#include "markers.h"
#include "vision.h"
#include "Defines.h"

#define WORLD_CAMERA_WIDTH 1600
#define WORLD_CAMERA_HEIGHT 1200

/* Type definitions */

enum CameraStatus {UNCALIBRATED, INTRINSICALLY_CALIBRATED, INTRINSICALLY_AND_EXTRINSICALLY_CALIBRATED, FULLY_CALIBRATED};

struct CameraCapture {
    CvCapture *camera_capture_feed;
    IplImage *current_raw_frame;
    IplImage *current_safe_copy_frame;
};

struct CameraIntrinsics {
    CvMat *camera_matrix;
    CvMat *distortion_coefficients;
};

struct CameraExtrinsics {
    CvMat *rotation_vector;
    CvMat *translation_vector;
    double a;
    double b;
    double c;
    double d;
};

struct Camera {
    struct CameraIntrinsics *camera_intrinsics;
    struct CameraExtrinsics *camera_extrinsics;
    struct CameraCapture *camera_capture;
    enum CameraStatus camera_status;
};

gpointer WorldVision_prepareImageFromWorldCameraForDrawing(gpointer data);

void WorldVision_applyWorldCameraBackFrame(void);

void WorldVision_createWorldCameraFrameSafeCopy(void);

void WorldVision_sendWorldInformationToRobot(struct Communication_Object robot,
        struct Communication_Object obstacles[MAXIMUM_OBSTACLE_NUMBER]);

#endif // __WORLD_VISION
