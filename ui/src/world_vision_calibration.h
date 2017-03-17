#ifndef __WORLD_VISION_CALIBRATION
#define __WORLD_VISION_CALIBRATION

#include <gtk/gtk.h>
#include "world_vision.h"
#include "point_types.h"

gboolean WorldVisionCalibration_initializeCameraMatrixAndDistortionCoefficientsFromFile(GtkWidget *widget,
        struct Camera *output_camera);

gboolean WorldVisionCalibration_calibrate(struct Camera *input_camera);

struct Point3D WorldVisionCalibration_convertImageCoordinatesToWorldCoordinates(struct Point2D image_coordinates,
        struct Camera *input_camera);

#endif // __WORLD_VISION_CALIBRATION
