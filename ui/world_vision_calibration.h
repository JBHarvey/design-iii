#ifndef __WORLD_VISION_CALIBRATION
#define __WORLD_VISION_CALIBRATION

#include <gtk/gtk.h>
#include "world_vision.h"

gboolean initializeCameraMatrixAndDistortionCoefficientsFromFile(GtkWidget *widget,
        struct CameraIntrinsics *output_camera_intrinsics);

gboolean gatherUserPointsForCameraPoseComputation(int input_index);

gboolean computeCameraPoseFromUserPoints(struct Camera *input_camera);

#endif // __WORLD_VISION_CALIBRATION
