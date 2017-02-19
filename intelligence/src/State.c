#include <stdlib.h>
#include "State.h"


struct State * State_new(struct Pose * new_pose)
{
    struct State * pointer = (struct State *) malloc(sizeof(struct State));
    pointer->pose = new_pose;
    return pointer;
}

void State_delete(struct State * state)
{
    Pose_delete(state->pose);
    free(state);
}
