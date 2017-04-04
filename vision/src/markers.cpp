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
#define ANGLE_CORNER ((M_PI /  4.0) * 0.7486681672439952)
#define MARKER_CORNER_DISTANCE_RATION (0.9013878188659974)

#define MARKER_CORNERS 4

static double distance_points(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

static struct Marker calculateMarker(std::vector<cv::Point2f> corners, unsigned int corner_number)
{
    double angle = atan2(corners[0].y - corners[2].y, corners[0].x - corners[2].x);
    double angle_x = cos(angle);
    double angle_y = sin(angle);
    angle = atan2(corners[1].y - corners[3].y, corners[1].x - corners[3].x);
    angle -= (M_PI /  2.0);
    angle_x += cos(angle);
    angle_y += sin(angle);

    struct Marker marker;
    marker.angle = atan2(angle_y, angle_x);
    double distance = distance_points(corners[0].x, corners[0].y, corners[2].x, corners[2].y);
    distance += distance_points(corners[1].x, corners[1].y, corners[3].x, corners[3].y);
    distance /= 2.0;

    double corner_angle = marker.angle + ((double)corner_number) * (M_PI /  2.0);

    double x = 0.0;
    x += corners[corner_number].x;
    x += (distance * MARKER_DISTANCE_RATIO) * cos(corner_angle);

    x += corners[(corner_number + 2) % MARKER_CORNERS].x;
    x += (distance + distance * MARKER_DISTANCE_RATIO) * cos(corner_angle);

    x += corners[(corner_number + 1) % MARKER_CORNERS].x;
    x += (distance * MARKER_CORNER_DISTANCE_RATION) * cos(corner_angle - ANGLE_CORNER);

    x += corners[(corner_number + 3) % MARKER_CORNERS].x;
    x += (distance * MARKER_CORNER_DISTANCE_RATION) * cos(corner_angle + ANGLE_CORNER);

    double y = 0.0;
    y += corners[corner_number].y;
    y += (distance * MARKER_DISTANCE_RATIO) * sin(corner_angle);

    y += corners[(corner_number + 2) % MARKER_CORNERS].y;
    y += (distance + distance * MARKER_DISTANCE_RATIO) * sin(corner_angle);

    y += corners[(corner_number + 1) % MARKER_CORNERS].y;
    y += (distance * MARKER_CORNER_DISTANCE_RATION) * sin(corner_angle - ANGLE_CORNER);

    y += corners[(corner_number + 3) % MARKER_CORNERS].y;
    y += (distance * MARKER_CORNER_DISTANCE_RATION) * sin(corner_angle + ANGLE_CORNER);

    marker.angle += (M_PI / 4.0);
    marker.valid = 1;
    marker.x = x / 4.0;
    marker.y = y / 4.0;
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

    double angle_x = 0.0, angle_y = 0.0;

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
            angle_x += cos(marker.angle);
            angle_y += sin(marker.angle);

            out_marker.valid = 1;
            ++num_markers;
        }
    }

    if(num_markers) {
        out_marker.x /= (double)num_markers;
        out_marker.y /= (double)num_markers;
        out_marker.angle = atan2(angle_y, angle_x);
    }

    return out_marker;
}

#ifdef __cplusplus
}
#endif
