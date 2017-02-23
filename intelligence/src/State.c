#include <stdlib.h>
#include "State.h"


struct State* State_new(struct Pose* new_pose)
{
    struct State* pointer = (struct State*) malloc(sizeof(struct State));
    pointer->pose = new_pose;

    struct Flag* newFlag = Flag_new();
    pointer->flag = newFlag;

    return pointer;
}

void State_delete(struct State* state)
{
    Pose_delete(state->pose);
    Flag_delete(state->flag);
    free(state);
}

void State_updateFlagValuesFrom(struct State* recipient, struct State* source)
{
    Flag_copyValuesFrom(recipient->flag, source->flag);
}
