#include "opencv2/imgproc/imgproc_c.h"
#include <stdbool.h>

struct Marker {
    bool valid;
    double x;
    double y;
    double angle;
};

struct Marker detectMarker(CvArr *image);
