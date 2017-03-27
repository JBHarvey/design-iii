#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/aruco.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include "markers.h"

#define MARKER_ID_TOP_RIGHT 101
#define MARKER_ID_BOTTOM_RIGHT 103
#define MARKER_ID_TOP_LEFT 102
#define MARKER_ID_BOTTOM_LEFT 104

#define MARKER_DISTANCE_RATIO (0.25)

static double distance_points(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

static struct Marker calculateMarker(std::vector<cv::Point2f> corners, unsigned int corner_number)
{
    struct Marker marker;
    marker.angle = atan2(corners[0].y - corners[2].y, corners[0].x - corners[2].x);
    double distance = distance_points(corners[0].x, corners[0].y, corners[2].x, corners[2].y);
    distance += distance_points(corners[1].x, corners[1].y, corners[3].x, corners[3].y);
    distance /= 2.0;
    distance *= MARKER_DISTANCE_RATIO;

    double x = corners[corner_number].x;
    x += distance * cos(marker.angle + ((double)corner_number) * (M_PI /  2.0));

    double y = corners[corner_number].y;
    y += distance * sin(marker.angle + ((double)corner_number) * (M_PI /  2.0));

    marker.angle += (M_PI / 4.0);
    marker.valid = 1;
    marker.x = x;
    marker.y = y;
    return marker;
}

struct Marker detectMarker(CvArr *image)
{
    cv::Mat src = cv::cvarrToMat(image);

    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f> > corners;
    #if (CV_MINOR_VERSION > 1)
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    #else
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    #endif
    cv::aruco::detectMarkers(src, dictionary, corners, ids);
    std::vector< cv::Mat > rvecs, tvecs;

    struct Marker out_marker = {};
    unsigned int num_markers = 0;

    unsigned int i;

    for(i = 0; i < ids.size(); ++i) {
        struct Marker marker;
        marker.valid = 0;

        if(ids[i] == MARKER_ID_TOP_RIGHT) {
            marker = calculateMarker(corners[i], 2);
        }

        if(ids[i] == MARKER_ID_TOP_LEFT) {
            marker = calculateMarker(corners[i], 3);
        }

        if(ids[i] == MARKER_ID_BOTTOM_RIGHT) {
            marker = calculateMarker(corners[i], 1);
        }

        if(ids[i] == MARKER_ID_BOTTOM_LEFT) {
            marker = calculateMarker(corners[i], 0);
        }

        if(marker.valid) {
            out_marker.x += marker.x;
            out_marker.y += marker.y;
            out_marker.angle += marker.angle;
            out_marker.valid = 1;
            ++num_markers;
        }
    }

    if(num_markers) {
        out_marker.x /= (double)num_markers;
        out_marker.y /= (double)num_markers;
        out_marker.angle /= (double)num_markers;
    }

    return out_marker;
}

#ifdef __cplusplus
}
#endif
