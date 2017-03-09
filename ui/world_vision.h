#ifndef __WORLD_VISION
#define __WORLD_VISION

#include <gtk/gtk.h>
#include "opencv2/imgproc/imgproc_c.h"

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

gpointer prepareImageFromWorldCameraForDrawing(gpointer data);

gboolean worldCameraDrawEventCallback(GtkWidget *widget, GdkEventExpose *event, gpointer data);
 
gboolean worldCameraCalibrationClickedEventCallback(GtkWidget *widget, gpointer data);

void setMainLoopStatusRunning(void);

void setMainLoopStatusTerminated(void);

void setWorldCameraStatusCalibrated(void);

#endif // __WORLD_VISION
