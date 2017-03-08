#include <stdlib.h>
#include "State.h"


struct State *State_new(struct Pose *new_pose)
{
    struct Object *new_object = Object_new();
    struct Flags *new_flags = Flags_new();
    struct State *pointer = (struct State *) malloc(sizeof(struct State));

    pointer->object = new_object;
    pointer->flags = new_flags;
    pointer->pose = new_pose;

    Object_addOneReference(new_pose->object);

    return pointer;
}

void State_delete(struct State *state)
{
    Object_removeOneReference(state->object);

    if(Object_canBeDeleted(state->object)) {
        Object_delete(state->object);
        Pose_delete(state->pose);
        Flags_delete(state->flags);
        free(state);
    }
}

void State_updateFlagsValuesFrom(struct State *recipient, struct State *source)
{
    Flags_copyValuesFrom(recipient->flags, source->flags);
}
