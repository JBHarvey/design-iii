#include "opencv2/imgproc/imgproc_c.h"

/* helper function:
 * finds a cosine of angle between vectors
 * from pt0->pt1 and from pt0->pt2
 */
double angle(CvPoint *pt1, CvPoint *pt2, CvPoint *pt0);

_Bool isDualSquare(CvSeq *contours);
_Bool isFigure(CvSeq *contours);

struct Square {
    CvPoint2D32f corner[4];
};

_Bool findFirstGreenSquare(CvMemStorage *opencv_storage, IplImage *image_yuv, struct Square *square);

CvSeq *findFirstFigure(CvMemStorage *opencv_storage, IplImage *image_yuv);

enum ObstacleType {
    OBSTACLE_NONE,
    OBSTACLE_CIRCLE,
    OBSTACLE_TRIANGLE
};

struct Obstacle {
    enum ObstacleType type;
    int x;
    int y;
    double angle; /* rads */
};

unsigned int findObstacles(CvMemStorage *opencv_storage, struct Obstacle *obstacles_out, unsigned int max_obstacles,
                           IplImage *image_yuv);

CvPoint coordinateToTableCoordinate(CvPoint point, double height_cm, CvPoint camera_midpoint);
