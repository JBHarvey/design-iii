#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include "markers.h"

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

    if(ids.size()) {
        marker.x = (((corners[0][0].x + corners[0][2].x) / 2) + ((corners[0][1].x + corners[0][3].x) / 2)) / 2;
        marker.y = (((corners[0][0].y + corners[0][2].y) / 2) + ((corners[0][1].y + corners[0][3].y) / 2)) / 2;
        marker.angle = atan2(corners[0][0].y - corners[0][2].y, corners[0][0].x - corners[0][2].x) + (M_PI / 4.0);
        marker.valid = 1;
    }

    return marker;
}

#ifdef __cplusplus
}
#endif
