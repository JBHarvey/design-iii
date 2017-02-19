#include <criterion/criterion.h>
#include <stdio.h>
#include "Pose.h"

Test(Pose, creation_destruction)
{
    const int SOME_X = 13256;
    const int SOME_Y = 521651;
    const int SOME_THETA = 11611;

    struct Pose *pose = Pose_new(SOME_X, SOME_Y, SOME_THETA);
    cr_assert(
        pose->x == SOME_X &&
        pose->y == SOME_Y &&
        pose->theta == SOME_THETA);
    Pose_delete(pose);
}
