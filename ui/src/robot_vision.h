#ifndef __ROBOT_VISION
#define __ROBOT_VISION

#include <gtk/gtk.h>
#include "opencv2/imgproc/imgproc_c.h"

void RobotVision_initialize(GtkWidget *widget);

void RobotVision_setImage(IplImage *image);

void RobotVision_finalize(void);

#endif // __ROBOT_VISION
