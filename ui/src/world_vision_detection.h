
#include "CommunicationStructures.h"
#include "Defines.h"
#include "world_vision_calibration.h"
#include "vision.h"

#define MAX_OBSTACLES (MAXIMIN_NUMBER_OF_OBSTACLE)

struct Detected_Things {
    _Bool table_detected;
    struct Square table;

    _Bool green_square_detected;
    struct Square green_square;

    _Bool robot_detected;
    struct Communication_Object robot;

    unsigned int num_obstacles;
    struct Communication_Object obstacles[MAX_OBSTACLES];
};

struct Detected_Things detectDrawThings(CvMemStorage *opencv_storage, IplImage *image_BGR, struct Camera *input_camera);
