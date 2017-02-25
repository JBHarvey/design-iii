#include "opencv2/imgproc/imgproc_c.h"

/* helper function:
 * finds a cosine of angle between vectors
 * from pt0->pt1 and from pt0->pt2
 */
double angle(CvPoint *pt1, CvPoint *pt2, CvPoint *pt0);

_Bool detect_dual_square(CvSeq *contours);

CvSeq *find_first_figure(CvMemStorage *storage, IplImage *img_yuv);
