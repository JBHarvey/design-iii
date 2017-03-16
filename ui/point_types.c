#include <math.h>
#include "opencv2/calib3d/calib3d_c.h"
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

struct Point3D PointTypes_setPoint3DOrigin(struct Point3D origin, struct Point3D point)
{
    return PointTypes_createPoint3D(point.x - origin.x, point.y - origin.y, point.z - origin.z);
}

double PointTypes_getNormOfPoint3D(struct Point3D point)
{
    return sqrt(pow(point.x, 2) + pow(point.y, 2) + pow(point.z, 2));
}

struct Point3D PointTypes_point3DCrossProduct(struct Point3D point_a, struct Point3D point_b)
{
    CvMat *cv_vector_a = cvCreateMat(3, 1, CV_64FC1);
    CvMat *cv_vector_b = cvCreateMat(3, 1, CV_64FC1);
    CvMat *cv_vector_c = cvCreateMat(3, 1, CV_64FC1);
    cvmSet(cv_vector_a, 0, 0, point_a.x);
    cvmSet(cv_vector_a, 1, 0, point_a.y);
    cvmSet(cv_vector_a, 2, 0, point_a.z);
    cvmSet(cv_vector_b, 0, 0, point_b.x);
    cvmSet(cv_vector_b, 1, 0, point_b.y);
    cvmSet(cv_vector_b, 2, 0, point_b.z);
    cvCrossProduct((CvArr*) cv_vector_a, (CvArr*) cv_vector_b, (CvArr*) cv_vector_c);
    struct Point3D result = PointTypes_createPoint3D(cvmGet(cv_vector_c, 0, 0), cvmGet(cv_vector_c, 1, 0),
                            cvmGet(cv_vector_c, 2,
                                   0));
    cvReleaseMat(&cv_vector_a);
    cvReleaseMat(&cv_vector_b);
    cvReleaseMat(&cv_vector_c);
    return result;
}

struct Point3D PointTypes_transformPoint3D(struct Point3D point, CvMat *rotation_vector, CvMat *translation_vector)
{
    CvMat *rotation_matrix = cvCreateMat(3, 3, CV_64FC1);
    CvMat *transformation_matrix = cvCreateMat(4, 4, CV_64FC1);
    CvMat *homogenous_point = cvCreateMat(4, 1, CV_64FC1);
    CvMat *transformed_point = cvCreateMat(4, 1, CV_64FC1);

    cvRodrigues2(rotation_vector, rotation_matrix, 0);
    cvmSet(transformation_matrix, 0, 0, cvmGet(rotation_matrix, 0, 0));
    cvmSet(transformation_matrix, 0, 1, cvmGet(rotation_matrix, 0, 1));
    cvmSet(transformation_matrix, 0, 2, cvmGet(rotation_matrix, 0, 2));
    cvmSet(transformation_matrix, 0, 3, cvmGet(translation_vector, 0, 0));
    cvmSet(transformation_matrix, 1, 0, cvmGet(rotation_matrix, 1, 0));
    cvmSet(transformation_matrix, 1, 1, cvmGet(rotation_matrix, 1, 1));
    cvmSet(transformation_matrix, 1, 2, cvmGet(rotation_matrix, 1, 2));
    cvmSet(transformation_matrix, 1, 3, cvmGet(translation_vector, 1, 0));
    cvmSet(transformation_matrix, 2, 0, cvmGet(rotation_matrix, 2, 0));
    cvmSet(transformation_matrix, 2, 1, cvmGet(rotation_matrix, 2, 1));
    cvmSet(transformation_matrix, 2, 2, cvmGet(rotation_matrix, 2, 2));
    cvmSet(transformation_matrix, 2, 3, cvmGet(translation_vector, 2, 0));
    cvmSet(transformation_matrix, 3, 0, 0);
    cvmSet(transformation_matrix, 3, 1, 0);
    cvmSet(transformation_matrix, 3, 2, 0);
    cvmSet(transformation_matrix, 3, 3, 1);
    cvmSet(homogenous_point, 0, 0, point.x);
    cvmSet(homogenous_point, 1, 0, point.y);
    cvmSet(homogenous_point, 2, 0, point.z);
    cvmSet(homogenous_point, 3, 0, 1);
    cvMatMul(transformation_matrix, homogenous_point, transformed_point);

    struct Point3D result = PointTypes_createPoint3D(cvmGet(transformed_point, 0, 0), cvmGet(transformed_point, 1, 0),
                            cvmGet(transformed_point, 2, 0));

    cvReleaseMat(&rotation_matrix);
    cvReleaseMat(&transformation_matrix);
    cvReleaseMat(&homogenous_point);
    cvReleaseMat(&transformed_point);

    return result;
}

struct Point3D PointTypes_transformInversePoint3D(struct Point3D point, CvMat *rotation_vector,
        CvMat *translation_vector)
{
    CvMat *rotation_matrix = cvCreateMat(3, 3, CV_64FC1);
    CvMat *transposed_rotation_matrix = cvCreateMat(3, 3, CV_64FC1);
    CvMat *opposite_transposed_rotation_matrix = cvCreateMat(3, 3, CV_64FC1);
    CvMat *translation_partial_matrix = cvCreateMat(3, 1, CV_64FC1);
    CvMat *inverse_transformation_matrix = cvCreateMat(4, 4, CV_64FC1);
    CvMat *homogenous_point = cvCreateMat(4, 1, CV_64FC1);
    CvMat *transformed_point = cvCreateMat(4, 1, CV_64FC1);
    cvRodrigues2(rotation_vector, rotation_matrix, 0);
    cvTranspose((CvArr*) rotation_matrix, (CvArr*) transposed_rotation_matrix);

    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            cvmSet(opposite_transposed_rotation_matrix, i, j, -1.0 * cvmGet(transposed_rotation_matrix, i, j));
        }
    }

    cvMatMul(opposite_transposed_rotation_matrix, translation_vector, translation_partial_matrix);
    cvmSet(inverse_transformation_matrix, 0, 0, cvmGet(transposed_rotation_matrix, 0, 0));
    cvmSet(inverse_transformation_matrix, 0, 1, cvmGet(transposed_rotation_matrix, 0, 1));
    cvmSet(inverse_transformation_matrix, 0, 2, cvmGet(transposed_rotation_matrix, 0, 2));
    cvmSet(inverse_transformation_matrix, 0, 3, cvmGet(translation_partial_matrix, 0, 0));
    cvmSet(inverse_transformation_matrix, 1, 0, cvmGet(transposed_rotation_matrix, 1, 0));
    cvmSet(inverse_transformation_matrix, 1, 1, cvmGet(transposed_rotation_matrix, 1, 1));
    cvmSet(inverse_transformation_matrix, 1, 2, cvmGet(transposed_rotation_matrix, 1, 2));
    cvmSet(inverse_transformation_matrix, 1, 3, cvmGet(translation_partial_matrix, 1, 0));
    cvmSet(inverse_transformation_matrix, 2, 0, cvmGet(transposed_rotation_matrix, 2, 0));
    cvmSet(inverse_transformation_matrix, 2, 1, cvmGet(transposed_rotation_matrix, 2, 1));
    cvmSet(inverse_transformation_matrix, 2, 2, cvmGet(transposed_rotation_matrix, 2, 2));
    cvmSet(inverse_transformation_matrix, 2, 3, cvmGet(translation_partial_matrix, 2, 0));
    cvmSet(inverse_transformation_matrix, 3, 0, 0);
    cvmSet(inverse_transformation_matrix, 3, 1, 0);
    cvmSet(inverse_transformation_matrix, 3, 2, 0);
    cvmSet(inverse_transformation_matrix, 3, 3, 1);
    cvmSet(homogenous_point, 0, 0, point.x);
    cvmSet(homogenous_point, 1, 0, point.y);
    cvmSet(homogenous_point, 2, 0, point.z);
    cvmSet(homogenous_point, 3, 0, 1);
    cvMatMul(inverse_transformation_matrix, homogenous_point, transformed_point);

    struct Point3D result = PointTypes_createPoint3D(cvmGet(transformed_point, 0, 0), cvmGet(transformed_point, 1, 0),
                            cvmGet(transformed_point, 2, 0));


    cvReleaseMat(&rotation_matrix);
    cvReleaseMat(&transposed_rotation_matrix);
    cvReleaseMat(&opposite_transposed_rotation_matrix);
    cvReleaseMat(&translation_partial_matrix);
    cvReleaseMat(&inverse_transformation_matrix);
    cvReleaseMat(&homogenous_point);
    cvReleaseMat(&transformed_point);

    return result;
}

struct Point2DSet *PointTypes_initializePoint2DSet(int number_of_points)
{
    struct Point2DSet *point_set = malloc(sizeof(struct Point2DSet));
    point_set->vector_of_points = cvCreateMatHeader(number_of_points, 2, CV_64FC1);
    point_set->flat_points = calloc(number_of_points * 2, sizeof(double));
    cvSetData(point_set->vector_of_points, point_set->flat_points, CV_AUTOSTEP);
    point_set->filled_up_to_index = 0;
    point_set->number_of_points = number_of_points;

    return point_set;
}

void PointTypes_releasePoint2DSet(struct Point2DSet *point_set)
{
    cvReleaseMat(&(point_set->vector_of_points));
    free(point_set->flat_points);
    free(point_set);
    point_set = NULL;
}

void PointTypes_addPointToPoint2DSet(struct Point2DSet *point_set, struct Point2D point)
{
    if(point_set->filled_up_to_index / 2 < point_set->number_of_points) {
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.x;
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.y;
    }
}

struct Point2D PointTypes_getPointFromPoint2DSet(struct Point2DSet *point_set, int index)
{
    return PointTypes_createPoint2D(point_set->flat_points[2 * index], point_set->flat_points[2 * index + 1]);
}

int PointTypes_getNumberOfPointStoredInPoint2DSet(struct Point2DSet *point_set)
{
    return point_set->filled_up_to_index / 2;
}

int PointTypes_isPoint2DSetFull(struct Point2DSet *point_set)
{
    return point_set->filled_up_to_index / 2 == point_set->number_of_points;
}

struct Point3DSet *PointTypes_initializePoint3DSet(int number_of_points)
{
    struct Point3DSet *point_set = malloc(sizeof(struct Point3DSet));
    point_set->vector_of_points = cvCreateMatHeader(number_of_points, 3, CV_64FC1);
    point_set->flat_points = calloc(number_of_points * 3, sizeof(double));
    cvSetData(point_set->vector_of_points, point_set->flat_points, CV_AUTOSTEP);
    point_set->filled_up_to_index = 0;
    point_set->number_of_points = number_of_points;

    return point_set;
}

void PointTypes_releasePoint3DSet(struct Point3DSet *point_set)
{
    cvReleaseMat(&(point_set->vector_of_points));
    free(point_set->flat_points);
    free(point_set);
    point_set = NULL;
}

void PointTypes_addPointToPoint3DSet(struct Point3DSet *point_set, struct Point3D point)
{
    if(point_set->filled_up_to_index / 3 < point_set->number_of_points) {
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.x;
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.y;
        point_set->flat_points[(point_set->filled_up_to_index)++] = point.z;
    }
}

struct Point3D PointTypes_getPointFromPoint3DSet(struct Point3DSet *point_set, int index)
{
    return PointTypes_createPoint3D(point_set->flat_points[3 * index], point_set->flat_points[3 * index + 1],
                                    point_set->flat_points[3 * index + 2]);
}

int PointTypes_getNumberOfPointStoredInPoint3DSet(struct Point3DSet *point_set)
{
    return point_set->filled_up_to_index / 3;
}

int PointTypes_isPoint3DSetFull(struct Point3DSet *point_set)
{
    return point_set->filled_up_to_index / 3 == point_set->number_of_points;
}

