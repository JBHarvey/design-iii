#ifndef __WORLD_VISION_CALIBRATION
#define __WORLD_VISION_CALIBRATION

#include <gtk/gtk.h>
#include "world_vision.h"

/* Saves into passed "camera_intrinsics" structure the camera matrix and distortion coefficients loaded from file. */
gboolean initialize_camera_matrix_and_distortion_coefficients_from_file(GtkWidget *widget,
        struct camera_intrinsics *output_camera_intrinsics);

#endif // __WORLD_VISION_CALIBRATION
