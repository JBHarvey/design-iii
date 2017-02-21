#ifndef OBJECTIVE_H_
#define OBJECTIVE_H_

#include "State.h"

struct Objective {
    struct State * objectiveValues;
    struct State * tolerances;
};

struct Objective * Objective_new(struct State * new_objectiveValues, struct State * new_tolerances);
void Objective_delete(struct Objective * objective);
int Objective_isReached(struct Objective * objective, int tolerance);

#endif // OBJECTIVE_H_
