#include "opencv2/imgproc/imgproc_c.h"

#ifndef VISION_H
#define VISION_H

double angle(CvPoint *pt1, CvPoint *pt2, CvPoint *pt0);
CvPoint fixedCvPointFrom32f(CvPoint2D32f point);

_Bool isDualSquare(CvSeq *contours);
_Bool isFigure(CvSeq *contours);
double distancePoints(CvPoint point1, CvPoint point2);

#define NUM_SQUARE_CORNERS 4
struct Square {
    CvPoint2D32f corner[NUM_SQUARE_CORNERS];
};

_Bool findFirstGreenSquare(CvMemStorage *opencv_storage, IplImage *image_yuv, struct Square *square);

CvSeq *findFirstFigure(CvMemStorage *opencv_storage, IplImage *image_yuv, IplImage **image_yuv_in_green_square);

enum ObstacleType {
    OBSTACLE_NONE,
    OBSTACLE_CIRCLE,
    OBSTACLE_TRIANGLE
};

struct Vision_Obstacle {
    enum ObstacleType type;
    int x;
    int y;
    double angle; /* rads */
};

void sortObstacles(struct Vision_Obstacle *obstacles, unsigned int num_obstacles);

unsigned int findObstacles(CvMemStorage *opencv_storage, struct Vision_Obstacle *obstacles_out, unsigned int max_obstacles,
                           IplImage *image_yuv);

CvPoint coordinateToTableCoordinate(CvPoint point, double height_cm, CvPoint camera_midpoint);

_Bool findTableCorners(IplImage *image_yuv, struct Square *square);

#endif
