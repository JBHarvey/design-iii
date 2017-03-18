#include "opencv2/imgproc/imgproc_c.h"
#include <stdbool.h>

struct Marker {
    bool valid;
    int x;
    int y;
    double angle;
};

struct Marker detectMarker(CvArr *image);
