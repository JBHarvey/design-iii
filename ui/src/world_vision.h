#ifndef __WORLD_VISION
#define __WORLD_VISION

#include <gtk/gtk.h>
#include "opencv2/imgproc/imgproc_c.h"
#include "station_client.h"

#define WORLD_CAMERA_WIDTH 1600
#define WORLD_CAMERA_HEIGHT 1200

/* Type definitions */

enum CameraStatus {UNCALIBRATED, INTRINSICALLY_CALIBRATED, INTRINSICALLY_AND_EXTRINSICALLY_CALIBRATED, FULLY_CALIBRATED};

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
    enum CameraStatus camera_status;
};

gpointer WorldVision_prepareImageFromWorldCameraForDrawing(struct StationClient *station_client);

void WorldVision_applyWorldCameraBackFrame(void);

IplImage *WorldVision_getWorldCameraBackFrame(void);

#endif // __WORLD_VISION
