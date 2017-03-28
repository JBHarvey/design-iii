#ifndef ONBOARDCAMERA_H
#define ONBOARDCAMERA_H

#include "opencv2/highgui/highgui_c.h"
#include "opencv2/videoio/videoio_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgcodecs/imgcodecs_c.h"

#include "CoordinatesSequence.h"

void OnboardCamera_init(void);
void OnboardCamera_freeCamera(void);

struct CoordinatesSequence *OnboardCamera_extractTrajectoryFromImage(IplImage **image_yuv_in_green_square);

void OnboardCamera_deleteImage(IplImage **image);

#endif //ONBOARDCAMERA_H
