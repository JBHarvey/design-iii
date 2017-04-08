#include <criterion/criterion.h>
#include <math.h>
#include <stdio.h>
#include "Pose.h"

const int POSE_X = 13256;
const int POSE_Y = 521651;
const int POSE_THETA = PI;
const int POSE_1_X = 500;
const int POSE_1_Y = 500;
const int POSE_1_THETA = 0;
const int POSE_2_X = 500;
const int POSE_2_Y = 200;
const int POSE_2_THETA = THREE_QUARTER_PI;
const int POSE_3_X = 300;
const int POSE_3_Y = 400;
const int POSE_3_THETA = MINUS_QUARTER_PI;
const int TRANSLATION_VALUE = 200;
const int ROTATION_VALUE = QUARTER_PI;

struct Pose *pose;
struct Pose *other_pose;
struct Pose *pose_1;
struct Pose *pose_2;
struct Pose *pose_3;

void setup_pose(void)
{
    pose = Pose_new(POSE_X, POSE_Y, POSE_THETA);
    other_pose = Pose_new(POSE_X, POSE_Y, POSE_THETA);
}

void teardown_pose(void)
{
    Pose_delete(pose);
    Pose_delete(other_pose);
}

void setup_pose_for_transformation(void)
{
    pose_1 = Pose_new(POSE_1_X, POSE_1_Y, POSE_1_THETA);
    pose_2 = Pose_new(POSE_2_X, POSE_2_Y, POSE_2_THETA);
    pose_3 = Pose_new(POSE_3_X, POSE_3_Y, POSE_3_THETA);
}

void teardown_pose_for_transformation(void)
{
    Pose_delete(pose_1);
    Pose_delete(pose_2);
    Pose_delete(pose_3);
}


Test(Pose, given__when_createsPoseZero_then_poseHasAllZeroValues)
{
    struct Pose *pose_zero = Pose_zero();
    cr_assert(
        pose_zero->coordinates->x == 0 &&
        pose_zero->coordinates->y == 0 &&
        pose_zero->angle->theta == 0);
    Pose_delete(pose_zero);
}
Test(Pose, creation_destruction
     , .init = setup_pose
     , .fini = teardown_pose)
{
    cr_assert(
        pose->coordinates->x == POSE_X &&
        pose->coordinates->y == POSE_Y &&
        pose->angle->theta == POSE_THETA);
}

Test(Pose, given_twoPoseWithSameValues_when_comparesThem_then_returnsTrue
     , .init = setup_pose
     , .fini = teardown_pose)
{
    int comparison = Pose_haveTheSameValues(pose, other_pose);
    cr_assert(comparison == 1);
}

Test(Pose, given_twoPoseWithDifferentValues_when_comparesThem_then_returnsFalse
     , .init = setup_pose
     , .fini = teardown_pose)
{

    other_pose->coordinates->x = POSE_Y;
    other_pose->coordinates->y = POSE_X;
    other_pose->angle->theta = MINUS_PI;
    int comparison = Pose_haveTheSameValues(pose, other_pose);
    cr_assert(comparison == 0);
}

Test(Pose, given_twoPoseWithDifferentValues_when_copied_then_thePoseHaveTheSameValues
     , .init = setup_pose
     , .fini = teardown_pose)
{
    other_pose->coordinates->x = POSE_Y;

    Pose_copyValuesFrom(pose, other_pose);

    int comparison = Pose_haveTheSameValues(pose, other_pose);
    cr_assert(comparison == 1);
}

const int ACCEPTED_ERROR = 10;
Test(Pose,
     given_aPoseWithAZeroAngleAndACoordinate_when_askedToComputeTheAngleBetween_then_theAngleBetweenTheTwoPointsIsReturnedInACartesianPlan
     , .init = setup_pose
     , .fini = teardown_pose)
{
    struct Coordinates *coordinates = Coordinates_new(POSE_X * 2, POSE_Y * 3);
    pose->angle->theta = 0;
    int delta_x = coordinates->x - pose->coordinates->x;
    int delta_y = coordinates->y - pose->coordinates->y;
    int expected_theta = (int)(atan2(delta_y, delta_x) / ANGLE_BASE_UNIT);
    int received_theta = Pose_computeAngleBetween(pose, coordinates);

    cr_assert(abs(expected_theta - received_theta) <= ACCEPTED_ERROR);
    Coordinates_delete(coordinates);
}

Test(Pose,
     given_aPoseWithANonZeroAngleAndACoordinate_when_askedToComputeTheAngleBetween_then_theAngleBetweenTheTwoPointsIsReturnedInACartesianPlan
     , .init = setup_pose
     , .fini = teardown_pose)
{
    struct Coordinates *coordinates = Coordinates_new(POSE_X * 2, POSE_Y * 3);
    int delta_x = coordinates->x - pose->coordinates->x;
    int delta_y = coordinates->y - pose->coordinates->y;
    int expected_theta = ((int)(atan2(delta_y, delta_x) / ANGLE_BASE_UNIT)) - pose->angle->theta;
    int received_theta = Pose_computeAngleBetween(pose, coordinates);

    cr_assert(abs(expected_theta - received_theta) <= ACCEPTED_ERROR);
    Coordinates_delete(coordinates);
}

Test(Pose, given_aPose_when_translateOnTheEastSide_then_thePoseIsTranslatedAccordingly
     , .init = setup_pose_for_transformation
     , .fini = teardown_pose_for_transformation)
{
    struct Pose *pose_1_translated = Pose_zero();
    pose_1_translated->coordinates->x = POSE_1_X + TRANSLATION_VALUE * cos(POSE_1_THETA * ANGLE_BASE_UNIT);
    pose_1_translated->coordinates->y = POSE_1_Y + TRANSLATION_VALUE * sin(POSE_1_THETA * ANGLE_BASE_UNIT);
    pose_1_translated->angle->theta = POSE_1_THETA;
    struct Pose *pose_2_translated = Pose_zero();
    pose_2_translated->coordinates->x = POSE_2_X + TRANSLATION_VALUE * cos(POSE_2_THETA * ANGLE_BASE_UNIT);
    pose_2_translated->coordinates->y = POSE_2_Y + TRANSLATION_VALUE * sin(POSE_2_THETA * ANGLE_BASE_UNIT);
    pose_2_translated->angle->theta = POSE_2_THETA;
    struct Pose *pose_3_translated = Pose_zero();
    pose_3_translated->coordinates->x = POSE_3_X + TRANSLATION_VALUE * cos(POSE_3_THETA * ANGLE_BASE_UNIT);
    pose_3_translated->coordinates->y = POSE_3_Y + TRANSLATION_VALUE * sin(POSE_3_THETA * ANGLE_BASE_UNIT);
    pose_3_translated->angle->theta = POSE_3_THETA;

    Pose_translate(pose_1, TRANSLATION_VALUE, 0);
    Pose_translate(pose_2, TRANSLATION_VALUE, 0);
    Pose_translate(pose_3, TRANSLATION_VALUE, 0);

    cr_assert(Pose_haveTheSameValues(pose_1, pose_1_translated), "Pose translated : (%d,%d,%d)",
              pose_1_translated->coordinates->x,
              pose_1_translated->coordinates->y, pose_1_translated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_2, pose_2_translated), "Pose translated : (%d,%d,%d)",
              pose_2_translated->coordinates->x,
              pose_2_translated->coordinates->y, pose_2_translated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_3, pose_3_translated), "Pose translated : (%d,%d,%d)",
              pose_3_translated->coordinates->x,
              pose_3_translated->coordinates->y, pose_3_translated->angle->theta);

    Pose_delete(pose_1_translated);
    Pose_delete(pose_2_translated);
    Pose_delete(pose_3_translated);
}

Test(Pose, given_aPose_when_translateOnTheNorthSide_then_thePoseIsTranslatedAccordingly
     , .init = setup_pose_for_transformation
     , .fini = teardown_pose_for_transformation)
{
    struct Pose *pose_1_translated = Pose_zero();
    pose_1_translated->coordinates->x = POSE_1_X + TRANSLATION_VALUE * cos((POSE_1_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_1_translated->coordinates->y = POSE_1_Y + TRANSLATION_VALUE * sin((POSE_1_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_1_translated->angle->theta = POSE_1_THETA;
    struct Pose *pose_2_translated = Pose_zero();
    pose_2_translated->coordinates->x = POSE_2_X + TRANSLATION_VALUE * cos((POSE_2_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_2_translated->coordinates->y = POSE_2_Y + TRANSLATION_VALUE * sin((POSE_2_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_2_translated->angle->theta = POSE_2_THETA;
    struct Pose *pose_3_translated = Pose_zero();
    pose_3_translated->coordinates->x = POSE_3_X + TRANSLATION_VALUE * cos((POSE_3_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_3_translated->coordinates->y = POSE_3_Y + TRANSLATION_VALUE * sin((POSE_3_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_3_translated->angle->theta = POSE_3_THETA;

    Pose_translate(pose_1, 0, TRANSLATION_VALUE);
    Pose_translate(pose_2, 0, TRANSLATION_VALUE);
    Pose_translate(pose_3, 0, TRANSLATION_VALUE);

    cr_assert(Pose_haveTheSameValues(pose_1, pose_1_translated), "Pose translated : (%d,%d,%d)",
              pose_1_translated->coordinates->x,
              pose_1_translated->coordinates->y, pose_1_translated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_2, pose_2_translated), "Pose translated : (%d,%d,%d)",
              pose_2_translated->coordinates->x,
              pose_2_translated->coordinates->y, pose_2_translated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_3, pose_3_translated), "Pose translated : (%d,%d,%d)",
              pose_3_translated->coordinates->x,
              pose_3_translated->coordinates->y, pose_3_translated->angle->theta);

    Pose_delete(pose_1_translated);
    Pose_delete(pose_2_translated);
    Pose_delete(pose_3_translated);
}

Test(Pose, given_aPose_when_translateOnTheWestSide_then_thePoseIsTranslatedAccordingly
     , .init = setup_pose_for_transformation
     , .fini = teardown_pose_for_transformation)
{
    struct Pose *pose_1_translated = Pose_zero();
    pose_1_translated->coordinates->x = POSE_1_X - TRANSLATION_VALUE * cos((POSE_1_THETA) * ANGLE_BASE_UNIT);
    pose_1_translated->coordinates->y = POSE_1_Y - TRANSLATION_VALUE * sin((POSE_1_THETA) * ANGLE_BASE_UNIT);
    pose_1_translated->angle->theta = POSE_1_THETA;
    struct Pose *pose_2_translated = Pose_zero();
    pose_2_translated->coordinates->x = POSE_2_X - TRANSLATION_VALUE * cos((POSE_2_THETA) * ANGLE_BASE_UNIT);
    pose_2_translated->coordinates->y = POSE_2_Y - TRANSLATION_VALUE * sin((POSE_2_THETA) * ANGLE_BASE_UNIT);
    pose_2_translated->angle->theta = POSE_2_THETA;
    struct Pose *pose_3_translated = Pose_zero();
    pose_3_translated->coordinates->x = POSE_3_X - TRANSLATION_VALUE * cos((POSE_3_THETA) * ANGLE_BASE_UNIT);
    pose_3_translated->coordinates->y = POSE_3_Y - TRANSLATION_VALUE * sin((POSE_3_THETA) * ANGLE_BASE_UNIT);
    pose_3_translated->angle->theta = POSE_3_THETA;

    Pose_translate(pose_1, -TRANSLATION_VALUE, 0);
    Pose_translate(pose_2, -TRANSLATION_VALUE, 0);
    Pose_translate(pose_3, -TRANSLATION_VALUE, 0);

    cr_assert(Pose_haveTheSameValues(pose_1, pose_1_translated), "Pose translated : (%d,%d,%d)",
              pose_1_translated->coordinates->x,
              pose_1_translated->coordinates->y, pose_1_translated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_2, pose_2_translated), "Pose translated : (%d,%d,%d)",
              pose_2_translated->coordinates->x,
              pose_2_translated->coordinates->y, pose_2_translated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_3, pose_3_translated), "Pose translated : (%d,%d,%d)",
              pose_3_translated->coordinates->x,
              pose_3_translated->coordinates->y, pose_3_translated->angle->theta);

    Pose_delete(pose_1_translated);
    Pose_delete(pose_2_translated);
    Pose_delete(pose_3_translated);
}

Test(Pose, given_aPose_when_translateOnTheSouthSide_then_thePoseIsTranslatedAccordingly
     , .init = setup_pose_for_transformation
     , .fini = teardown_pose_for_transformation)
{
    struct Pose *pose_1_translated = Pose_zero();
    pose_1_translated->coordinates->x = POSE_1_X - TRANSLATION_VALUE * cos((POSE_1_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_1_translated->coordinates->y = POSE_1_Y - TRANSLATION_VALUE * sin((POSE_1_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_1_translated->angle->theta = POSE_1_THETA;
    struct Pose *pose_2_translated = Pose_zero();
    pose_2_translated->coordinates->x = POSE_2_X - TRANSLATION_VALUE * cos((POSE_2_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_2_translated->coordinates->y = POSE_2_Y - TRANSLATION_VALUE * sin((POSE_2_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_2_translated->angle->theta = POSE_2_THETA;
    struct Pose *pose_3_translated = Pose_zero();
    pose_3_translated->coordinates->x = POSE_3_X - TRANSLATION_VALUE * cos((POSE_3_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_3_translated->coordinates->y = POSE_3_Y - TRANSLATION_VALUE * sin((POSE_3_THETA + HALF_PI) * ANGLE_BASE_UNIT);
    pose_3_translated->angle->theta = POSE_3_THETA;

    Pose_translate(pose_1, 0, -TRANSLATION_VALUE);
    Pose_translate(pose_2, 0, -TRANSLATION_VALUE);
    Pose_translate(pose_3, 0, -TRANSLATION_VALUE);

    cr_assert(Pose_haveTheSameValues(pose_1, pose_1_translated), "Pose translated : (%d,%d,%d)",
              pose_1_translated->coordinates->x,
              pose_1_translated->coordinates->y, pose_1_translated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_2, pose_2_translated), "Pose translated : (%d,%d,%d)",
              pose_2_translated->coordinates->x,
              pose_2_translated->coordinates->y, pose_2_translated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_3, pose_3_translated), "Pose translated : (%d,%d,%d)",
              pose_3_translated->coordinates->x,
              pose_3_translated->coordinates->y, pose_3_translated->angle->theta);

    Pose_delete(pose_1_translated);
    Pose_delete(pose_2_translated);
    Pose_delete(pose_3_translated);
}

Test(Pose, given_aPose_when_rotate_then_thePoseAngleIsRotatedAccordingly
     , .init = setup_pose_for_transformation
     , .fini = teardown_pose_for_transformation)
{
    struct Pose *pose_1_rotated = Pose_zero();
    struct Angle *new_theta_1 = Angle_new(POSE_1_THETA + ROTATION_VALUE);
    pose_1_rotated->coordinates->x = POSE_1_X;
    pose_1_rotated->coordinates->y = POSE_1_Y;
    pose_1_rotated->angle->theta = new_theta_1->theta;
    struct Pose *pose_2_rotated = Pose_zero();
    struct Angle *new_theta_2 = Angle_new(POSE_2_THETA + ROTATION_VALUE);
    pose_2_rotated->coordinates->x = POSE_2_X;
    pose_2_rotated->coordinates->y = POSE_2_Y;
    pose_2_rotated->angle->theta = new_theta_2->theta;
    struct Pose *pose_3_rotated = Pose_zero();
    struct Angle *new_theta_3 = Angle_new(POSE_3_THETA + ROTATION_VALUE);
    pose_3_rotated->coordinates->x = POSE_3_X;
    pose_3_rotated->coordinates->y = POSE_3_Y;
    pose_3_rotated->angle->theta = new_theta_3->theta;

    Pose_rotate(pose_1, ROTATION_VALUE);
    Pose_rotate(pose_2, ROTATION_VALUE);
    Pose_rotate(pose_3, ROTATION_VALUE);

    cr_assert(Pose_haveTheSameValues(pose_1, pose_1_rotated), "Pose rotated : (%d,%d,%d)",
              pose_1_rotated->coordinates->x,
              pose_1_rotated->coordinates->y, pose_1_rotated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_2, pose_2_rotated), "Pose rotated : (%d,%d,%d)",
              pose_2_rotated->coordinates->x,
              pose_2_rotated->coordinates->y, pose_2_rotated->angle->theta);
    cr_assert(Pose_haveTheSameValues(pose_3, pose_3_rotated), "Pose rotated : (%d,%d,%d)",
              pose_3_rotated->coordinates->x,
              pose_3_rotated->coordinates->y, pose_3_rotated->angle->theta);

    Pose_delete(pose_1_rotated);
    Pose_delete(pose_2_rotated);
    Pose_delete(pose_3_rotated);
    Angle_delete(new_theta_1);
    Angle_delete(new_theta_2);
    Angle_delete(new_theta_3);

}
