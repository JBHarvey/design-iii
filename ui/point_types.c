#include "point_types.h"

struct Point2D PointTypes_createPoint2D(double x, double y)
{
    struct Point2D point2D;
    point2D.x = x;
    point2D.y = y;
    return point2D;
}

struct Point3D PointTypes_createPoint3D(double x, double y, double z)
{
    struct Point3D point3D;
    point3D.x = x;
    point3D.y = y;
    point3D.z = z;
    return point3D;
}

void PointTypes_initializePoint2DSet(struct Point2DSet *point_set, int number_of_points)
{

    point_set->vector_of_points = cvCreateMatHeader(number_of_points, 2, CV_64FC1);
    point_set->flat_points = calloc(number_of_points * 2, sizeof(double));
    cvSetData(point_set->vector_of_points, point_set->flat_points, CV_AUTOSTEP);
    point_set->filled_up_to_index = 0;
    point_set->number_of_points = number_of_points;
}

void PointTypes_releasePoint2DSet(struct Point2DSet *point_set)
{
    cvReleaseMat(&(point_set->vector_of_points));
    free(point_set->flat_points);
}

void PointTypes_addPointToPoint2DSet(struct Point2DSet *point_set, struct Point2D point)
{
    if(point_set->filled_up_to_index <= point_set->number_of_points - 2) {
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.x;
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.y;
    }
}

struct Point2D PointTypes_getPointFromPoint2DSet(struct Point2DSet *point_set, int index)
{
    struct Point2D point;
    point.x = point_set->flat_points[2 * index];
    point.y = point_set->flat_points[2 * index + 1];
    return point;
}

int PointTypes_getNumberOfPointStoredInPoint2DSet(struct Point2DSet *point_set)
{
    return point_set->filled_up_to_index / 2;
}

void PointTypes_initializePoint3DSet(struct Point3DSet *point_set, int number_of_points)
{
    point_set->vector_of_points = cvCreateMatHeader(number_of_points, 3, CV_64FC1);
    point_set->flat_points = calloc(number_of_points * 3, sizeof(double));
    cvSetData(point_set->vector_of_points, point_set->flat_points, CV_AUTOSTEP);
    point_set->filled_up_to_index = 0;
    point_set->number_of_points = number_of_points;
}

void PointTypes_releasePoint3DSet(struct Point3DSet *point_set)
{
    cvReleaseMat(&(point_set->vector_of_points));
    free(point_set->flat_points);
}

void PointTypes_addPointToPoint3DSet(struct Point3DSet *point_set, struct Point3D point)
{
    if(point_set->filled_up_to_index <= point_set->number_of_points - 3) {
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.x;
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.y;
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.z;
    }
}

struct Point3D PointTypes_getPointFromPoint3DSet(struct Point3DSet *point_set, int index)
{
    struct Point3D point;
    point.x = point_set->flat_points[3 * index];
    point.y = point_set->flat_points[3 * index + 1];
    point.z = point_set->flat_points[3 * index + 2];
    return point;
}

int PointTypes_getNumberOfPointStoredInPoint3DSet(struct Point3DSet *point_set)
{
    return point_set->filled_up_to_index / 3;
}

