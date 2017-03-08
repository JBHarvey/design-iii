#include <criterion/criterion.h>
#include <stdio.h>
#include "Pose.h"

const int POSE_X = 13256;
const int POSE_Y = 521651;
const int POSE_THETA = PI;

struct Pose *pose;
struct Pose *other_pose;

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
