#include <criterion/criterion.h>
#include <stdio.h>
#include <math.h>
#include "point_types.h"

Test(Point2D, creation)
{
    double x = 1.0;
    double y = 1.5;
    struct Point2D point = PointTypes_createPoint2D(x, y);

    cr_assert_eq(point.x, x);
    cr_assert_eq(point.y, y);
}

Test(Point3D, creation)
{
    double x = 1.0;
    double y = 1.5;
    double z = 2.0;
    struct Point3D point = PointTypes_createPoint3D(x, y, z);

    cr_assert_eq(point.x, x);
    cr_assert_eq(point.y, y);
    cr_assert_eq(point.z, z);
}

Test(Point3D, given_aPoint_when_settingANewOrigin_then_thePointIsSetAsIfTheNewOriginIsZero)
{
    struct Point3D point = PointTypes_createPoint3D(10.0, 15.5, 20.0);
    struct Point3D origin = PointTypes_createPoint3D(1.5, -2.0, 10.0);

    struct Point3D new_point = PointTypes_setPoint3DOrigin(origin, point);

    cr_assert_eq(new_point.x, 8.5);
    cr_assert_eq(new_point.y, 17.5);
    cr_assert_eq(new_point.z, 10.0);
}

Test(Point3D, given_aPoint_when_gettingItsNorm_then_theNormIsValid)
{
    struct Point3D point = PointTypes_createPoint3D(10.0, 15.5, 20.0);
    double wanted_norm = sqrt(pow(point.x, 2) + pow(point.y, 2) + pow(point.z, 2));

    double norm = PointTypes_getNormOfPoint3D(point);

    cr_assert_eq(norm, wanted_norm);
}
