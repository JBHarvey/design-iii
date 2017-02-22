#include <criterion/criterion.h>
#include <stdio.h>
#include "Pose.h"
#include "State.h"

Test(states, creation_destruction)
{
    const int SOME_X = 13256;
    const int SOME_Y = 521651;
    const int SOME_THETA = 11611;

    struct Pose *pose = Pose_new(SOME_X, SOME_Y, SOME_THETA);
    struct State *state = State_new(pose);
    cr_assert(state->pose == pose);
    State_delete(state);
}
