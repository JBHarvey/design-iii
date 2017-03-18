
#include "CommunicationStructures.h"
#include "world_vision_calibration.h"

#define MAX_OBSTACLES 20

struct Detected_Things {
    _Bool robot_detected;
    struct Communication_Object robot;

    unsigned int num_obstacles;
    struct Communication_Object obstacles[MAX_OBSTACLES];
};

struct Detected_Things detectDrawObstaclesRobot(CvMemStorage *opencv_storage, IplImage *image_BGR,
        struct Camera *input_camera);
