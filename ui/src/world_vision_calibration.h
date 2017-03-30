#ifndef __WORLD_VISION_CALIBRATION
#define __WORLD_VISION_CALIBRATION

#include <gtk/gtk.h>
#include "world_vision.h"
#include "point_types.h"

gboolean WorldVisionCalibration_initializeCameraMatrixAndDistortionCoefficientsFromFile(GtkWidget *widget,
        struct Camera *output_camera);

gboolean WorldVisionCalibration_calibrateWithTableCorners(struct Camera * input_camera);

gboolean WorldVisionCalibration_calibrateWithGreenSquareCorners(struct Camera * input_camera);

struct Point3D WorldVisionCalibration_convertImageCoordinatesToWorldCoordinates(struct Point2D image_coordinates,
        struct Camera *input_camera);

void WorldVisionCalibration_convertWorldCoordinatesSetToImageCoordinatesSet(struct Point3DSet *world_coordinates_set,
        struct Point2DSet *output_images_coordinates_set,  struct Camera *input_camera);

struct Point2D WorldVisionCalibration_convertWorldCoordinatesToImageCoordinates(
    struct Point3D world_coordinates, struct Camera *input_camera);

#endif // __WORLD_VISION_CALIBRATION
