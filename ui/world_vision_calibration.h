#ifndef __WORLD_VISION_CALIBRATION
#define __WORLD_VISION_CALIBRATION

#include <gtk/gtk.h>
#include "world_vision.h"

gboolean initialize_camera_matrix_and_distortion_coefficients_from_file(GtkWidget *widget,
        struct camera_intrinsics *output_camera_intrinsics);

gboolean gather_user_points_for_camera_pose_computation(int input_index);

gboolean compute_camera_pose_from_user_points(struct camera *input_camera);

#endif // __WORLD_VISION_CALIBRATION
