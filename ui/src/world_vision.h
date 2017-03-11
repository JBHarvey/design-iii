#ifndef __WORLD_VISION
#define __WORLD_VISION

#include <gtk/gtk.h>
#include "opencv2/imgproc/imgproc_c.h"

/* Constants */

const int WORLD_CAMERA_WIDTH;
const int WORLD_CAMERA_HEIGHT;

/* Type definitions */

struct CameraIntrinsics {
    CvMat *camera_matrix;
    CvMat *distortion_coefficients;
};

struct CameraExtrinsics {
    CvMat *rotation_vector;
    CvMat *translation_vector;
};

struct Camera {
    struct CameraIntrinsics *camera_intrinsics;
    struct CameraExtrinsics *camera_extrinsics;
};

gpointer WorldVision_prepareImageFromWorldCameraForDrawing(gpointer data);

void WorldVision_setMainLoopStatusRunning(void);

void WorldVision_setMainLoopStatusTerminated(void);

void WorldVision_setWorldCameraStatusCalibrated(void);

#endif // __WORLD_VISION
