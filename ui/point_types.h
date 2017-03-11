#ifndef __POINT_TYPES
#define __POINT_TYPES

#include "opencv2/imgproc/imgproc_c.h"

/* Type definitions */

struct Point2D {
    double x;
    double y;
};

struct Point3D {
    double x;
    double y;
    double z;
};

struct Point2DSet {
    CvMat *vector_of_points;
    double *flat_points;
    int filled_up_to_index;
    int number_of_points;
};

struct Point3DSet {
    CvMat *vector_of_points;
    double *flat_points;
    int filled_up_to_index;
    int number_of_points;
};

struct Point2D PointTypes_createPoint2D(double x, double y);

struct Point3D PointTypes_createPoint3D(double x, double y, double z);

struct Point3D PointTypes_transformPoint3D(struct Point3D point, CvMat *rotation_vector, CvMat *translation_vector);

struct Point2DSet *PointTypes_initializePoint2DSet(int number_of_points);

void PointTypes_releasePoint2DSet(struct Point2DSet *point_set);

void PointTypes_addPointToPoint2DSet(struct Point2DSet *point_set, struct Point2D point);

struct Point2D PointTypes_getPointFromPoint2DSet(struct Point2DSet *point_set, int index);

int PointTypes_getNumberOfPointStoredInPoint2DSet(struct Point2DSet *point_set);

int PointTypes_isPoint2DSetFull(struct Point2DSet *point_set);

struct Point3DSet *PointTypes_initializePoint3DSet(int number_of_points);

void PointTypes_releasePoint3DSet(struct Point3DSet *point_set);

void PointTypes_addPointToPoint3DSet(struct Point3DSet *point_set, struct Point3D point);

struct Point3D PointTypes_getPointFromPoint3DSet(struct Point3DSet *point_set, int index);

int PointTypes_getNumberOfPointStoredInPoint3DSet(struct Point3DSet *point_set);

int PointTypes_isPoint3DSetFull(struct Point3DSet *point_set);

#endif // __POINT_TYPES
