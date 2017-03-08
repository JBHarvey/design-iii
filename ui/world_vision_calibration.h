#ifndef __WORLD_VISION_CALIBRATION
#define __WORLD_VISION_CALIBRATION

#include <gtk/gtk.h>
#include "world_vision.h"

/* Saves into passed "camera_intrinsics" structure the camera matrix and distortion coefficients loaded from file. */
gboolean initialize_camera_matrix_and_distortion_coefficients_from_file(GtkWidget *widget,
        struct camera_intrinsics *output_camera_intrinsics);

/* Gathers user clicked points that are going to be associated to real world 3D coordinates in order to compute
 * the camera pose. */
gboolean gather_user_inputs_for_camera_pose_computation(int input_index);

/* Computes the camera pose from user inputs points associated to real world 3D coordinates and using the camera
 * intrinsics. */
gboolean compute_camera_pose_from_user_inputs(struct camera *input_camera);

#endif // __WORLD_VISION_CALIBRATION
