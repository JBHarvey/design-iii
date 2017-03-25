
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

void Camera_init();

/* NOTE: returned image is in yuv color space and must be freed.
 */
IplImage *Camera_get_image();
