#ifndef STATE_H_
#define STATE_H_

#include "Pose.h"
#include "Flags.h"

struct State {
    struct Pose *pose;
    struct Flags *flags;
};

struct State *State_new(struct Pose *new_pose);
void State_delete(struct State *state);

void State_updateFlagsValuesFrom(struct State *recipient, struct State *source);

#endif // STATE_H_
