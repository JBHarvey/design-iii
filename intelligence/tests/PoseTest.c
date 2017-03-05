#include <criterion/criterion.h>
#include <stdio.h>
#include "Pose.h"

const int POSE_X = 13256;
const int POSE_Y = 521651;
const int POSE_THETA = PI;

struct Pose *pose;
struct Pose *otherPose;

void setupPose(void)
{
    pose = Pose_new(POSE_X, POSE_Y, POSE_THETA);
    otherPose = Pose_new(POSE_X, POSE_Y, POSE_THETA);
}

void teardownPose(void)
{
    Pose_delete(pose);
    Pose_delete(otherPose);
}

Test(Pose, given__when_createsPoseZero_then_poseHasAllZeroValues)
{
    struct Pose *poseZero = Pose_zero();
    cr_assert(
        poseZero->x == 0 &&
        poseZero->y == 0 &&
        poseZero->angle->theta == 0);
    Pose_delete(poseZero);
}
Test(Pose, creation_destruction
     , .init = setupPose
     , .fini = teardownPose)
{
    cr_assert(
        pose->x == POSE_X &&
        pose->y == POSE_Y &&
        pose->angle->theta == POSE_THETA);
}

Test(Pose, given_twoPoseWithSameValues_when_comparesThem_then_returnsTrue
     , .init = setupPose
     , .fini = teardownPose)
{
    int comparison = Pose_haveTheSameValues(pose, otherPose);
    cr_assert(comparison == 1);
}

Test(Pose, given_twoPoseWithDifferentValues_when_comparesThem_then_returnsFalse
     , .init = setupPose
     , .fini = teardownPose)
{

    otherPose->x = POSE_Y;
    otherPose->y = POSE_X;
    otherPose->angle->theta = MINUS_PI;
    int comparison = Pose_haveTheSameValues(pose, otherPose);
    cr_assert(comparison == 0);
}

Test(Pose, given_twoPoseWithDifferentValues_when_copied_then_thePoseHaveTheSameValues
     , .init = setupPose
     , .fini = teardownPose)
{
    otherPose->x = POSE_Y;

    Pose_copyValuesFrom(pose, otherPose);

    int comparison = Pose_haveTheSameValues(pose, otherPose);
    cr_assert(comparison == 1);
}
