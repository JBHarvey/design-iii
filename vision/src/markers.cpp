#include <opencv2/core/core.hpp>
#include <opencv2/core/highgui.hpp>
#include <opencv2/aruco.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include "markers.h"

#define MARKER_ID 123

struct Marker detectMarker(CvArr *image)
{
    cv::Mat src = cv::cvarrToMat(image);

    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f> > corners;
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::detectMarkers(src, dictionary, corners, ids);
    std::vector< cv::Mat > rvecs, tvecs;

    struct Marker marker;
    marker.valid = 0;

    unsigned int i;

    for(i = 0; i < ids.size(); ++i) {
        if(ids[i] == MARKER_ID) {
            marker.x = (((corners[i][0].x + corners[i][2].x) / 2) + ((corners[i][1].x + corners[i][3].x) / 2)) / 2;
            marker.y = (((corners[i][0].y + corners[i][2].y) / 2) + ((corners[i][1].y + corners[i][3].y) / 2)) / 2;
            marker.angle = atan2(corners[i][0].y - corners[i][2].y, corners[i][0].x - corners[i][2].x) + (M_PI / 4.0);
            marker.valid = 1;
            break;
        }
    }

    return marker;
}

#ifdef __cplusplus
}
#endif
