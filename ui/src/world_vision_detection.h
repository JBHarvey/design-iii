
#include "CommunicationStructures.h"
#include "world_vision_calibration.h"
#include "Defines.h"

/*
struct DetectedThings {
    _Bool robot_detected;
    struct Communication_Object robot;

    unsigned int num_obstacles;
    struct Communication_Object obstacles[MAX_OBSTACLES];
};
*/

struct DetectedThings {
    struct Marker robot;    
    int number_of_obstacles;
    struct Obstacle obstacles[MAXIMUM_OBSTACLE_NUMBER];
    int has_changed;
};

gpointer WorldVisionDetection_detectObstaclesAndRobot(struct Camera *input_camera);

void WorldVisionDetection_drawObstaclesAndRobot(IplImage *world_camera_back_frame);

/*
struct DetectedThings detectDrawObstaclesRobot(CvMemStorage *opencv_storage, IplImage *image_BGR,
        struct Camera *input_camera);
*/
