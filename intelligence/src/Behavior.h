#ifndef BEHAVIOR_H_
#define BEHAVIOR_H_

#include "Objective.h"

struct Behavior {
    struct Object *object;
    struct Objective *entryConditions;
};

struct Behavior *Behavior_new(struct Objective *new_entryCondition);
void Behavior_delete(struct Behavior *behavior);
int Behavior_entryConditionsAreReached(struct Behavior *behavior, struct State *currentState);

#endif // BEHAVIOR_H_
