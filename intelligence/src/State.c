#include <stdlib.h>
#include "State.h"


struct State *State_new(struct Pose *new_pose)
{
    struct State *pointer = (struct State *) malloc(sizeof(struct State));
    pointer->pose = new_pose;

    struct Flags *newFlags = Flags_new();
    pointer->flags = newFlags;

    return pointer;
}

void State_delete(struct State *state)
{
    Pose_delete(state->pose);
    Flags_delete(state->flags);
    free(state);
}

void State_updateFlagsValuesFrom(struct State *recipient, struct State *source)
{
    Flags_copyValuesFrom(recipient->flags, source->flags);
}
