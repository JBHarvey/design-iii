#include <gtk/gtk.h>
#include "opencv2/imgproc/imgproc_c.h"
#include "world_vision.h"
#include "Defines.h"
#include "world_vision_calibration.h"
#include "vision.h"


struct DetectedThings {
    _Bool table_detected;
    struct Square table;
    _Bool green_square_detected;
    struct Square green_square;
    struct Marker robot;
    int number_of_obstacles;
    struct Obstacle obstacles[MAXIMUM_OBSTACLE_NUMBER];
    int has_changed;
};

gpointer WorldVisionDetection_detectObstaclesAndRobot(struct Camera *input_camera);

void WorldVisionDetection_drawObstaclesAndRobot(IplImage *world_camera_back_frame);
