#ifndef CAMERA_H
#define CAMERA_H

#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "CoordinatesSequence.h"

void Camera_init();

/* NOTE: returned image is in yuv color space and must be freed.
 */
IplImage *Camera_get_image();

struct CoordinatesSequence *Camera_cvSeqToCoordinatesSequence(CvSeq *opencv_sequence);

#endif
