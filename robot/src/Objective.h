#ifndef OBJECTIVE_H_
#define OBJECTIVE_H_

#include "State.h"

#define X_TOLERANCE_MIN 0                   // 0 mm
#define X_TOLERANCE_DEFAULT 1500            // 15 cm
#define X_TOLERANCE_MAX 50000               // 5 m

#define Y_TOLERANCE_MIN 0                   // 0 mm
#define Y_TOLERANCE_DEFAULT 1500            // 15 cm
#define Y_TOLERANCE_MAX 50000               // 5 m

#define THETA_TOLERANCE_MIN 0               // 0 rad
#define THETA_TOLERANCE_DEFAULT 31416       // Pi/10 rad
#define THETA_TOLERANCE_MAX 314160          // Pi rad

struct Objective {
    struct Object *object;
    struct State *goal_state;
    struct State *tolerances;
};

struct Objective *Objective_new(struct State *new_goal_state, struct State *new_tolerances);
void Objective_delete(struct Objective *objective);
int Objective_isReached(struct Objective *objective, struct State *current_state);

#endif // OBJECTIVE_H_