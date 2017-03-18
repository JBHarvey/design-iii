#ifndef OBJECTIVE_H_
#define OBJECTIVE_H_

#include "State.h"

struct Objective {
    struct Object *object;
    struct State *goal_state;
    struct State *tolerances;
};

struct Objective *Objective_new(struct State *new_goal_state, struct State *new_tolerances);
void Objective_delete(struct Objective *objective);
int Objective_isReached(struct Objective *objective, struct State *current_state);

#endif // OBJECTIVE_H_
