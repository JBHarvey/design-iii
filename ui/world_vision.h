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

/* Intended to be a worker thread that takes camera capture continuously and feed a pixel buffer with them. */
gpointer world_camera_feeder(gpointer data);

/* Callback used by draw event on the world camera's GtkDrawingArea that process a redraw of the area with 
 * the world camera feed's pixel buffer. */ 
gboolean world_camera_draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
 
/* Callback used by the "calibrate" button that loads the intrinsics parameters of the world camera from file and
 * computes its extrinsics parameters. Then, it undistorts the world camera feed. */
gboolean world_camera_calibration_clicked_event_callback(GtkWidget *widget, gpointer data);

/* Advises the world camera feeder thread that the main loop is running.*/
void set_main_loop_status_running(void);

/* Advises the world camera feeder thread that the main loop has ended and that the thread should stop. */
void set_main_loop_status_terminated(void);

/* Advises the world camera feeder thread that the world camera intrinsics and extrinsics have been computed and that
 * the feeder should now apply the undistortion to the feed. */
void set_world_camera_status_calibrated(void);

#endif // __WORLD_VISION
