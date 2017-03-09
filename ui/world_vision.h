#ifndef __WORLD_VISION
#define __WORLD_VISION

#include <gtk/gtk.h>
#include "opencv2/imgproc/imgproc_c.h"

/* Type definitions */

struct camera_intrinsics {
    CvMat *camera_matrix;
    CvMat *distortion_coefficients;
};

struct camera_extrinsics {
    CvMat *rotation_vector;
    CvMat *translation_vector;
};

struct camera {
    struct camera_intrinsics *camera_intrinsics;
    struct camera_extrinsics *camera_extrinsics;
};

gpointer prepare_image_from_world_camera_for_drawing(gpointer data);

gboolean world_camera_draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
 
gboolean world_camera_calibration_clicked_event_callback(GtkWidget *widget, gpointer data);

void set_main_loop_status_running(void);

void set_main_loop_status_terminated(void);

void set_world_camera_status_calibrated(void);

#endif // __WORLD_VISION
