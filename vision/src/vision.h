#include "opencv2/imgproc/imgproc_c.h"

/* helper function:
 * finds a cosine of angle between vectors
 * from pt0->pt1 and from pt0->pt2
 */
double angle(CvPoint *pt1, CvPoint *pt2, CvPoint *pt0);

_Bool detect_dual_square(CvSeq *contours);
_Bool detect_figure(CvSeq *contours);

CvSeq *find_first_figure(CvMemStorage *storage, IplImage *image_yuv);

typedef enum {
    OBSTACLE_NONE,
    OBSTACLE_CIRCLE,
    OBSTACLE_TRIANGLE
} Obstacle_Type;

typedef struct {
    Obstacle_Type type;
    int x;
    int y;
    double angle; /* rads */
}
Obstacle;

unsigned int find_obstacles(CvMemStorage *storage, Obstacle *obstacles_out, unsigned int max_obstacles,
                            IplImage *image_yuv);
