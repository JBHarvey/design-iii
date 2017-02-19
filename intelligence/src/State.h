#ifndef STATE_H_
#define STATE_H_

#include "Pose.h"

struct State {
    struct Pose * pose;
};

struct State * State_new(struct Pose * new_pose);
void State_delete(struct State * state);

#endif // STATE_H_
