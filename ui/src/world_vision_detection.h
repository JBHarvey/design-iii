
#include "CommunicationStructures.h"
#include "Defines.h"
#include "markers.h"
#include "vision.h"
#include "world_vision_calibration.h"

/*
struct Detected_Things {
    _Bool robot_detected;
    struct Communication_Object robot;

    unsigned int num_obstacles;
    struct Communication_Object obstacles[MAX_OBSTACLES];
};
*/

struct Detected_Things {
    struct Marker robot;    
    int number_of_obstacles;
    struct Obstacle obstacles[MAXIMUM_OBSTACLE_NUMBER];
};

gpointer WorldVisionDetection_detectObstaclesAndRobot(struct Camera *input_camera);

/*
struct Detected_Things detectDrawObstaclesRobot(CvMemStorage *opencv_storage, IplImage *image_BGR,
        struct Camera *input_camera);
*/
