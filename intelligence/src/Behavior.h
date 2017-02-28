#ifndef BEHAVIOR_H_
#define BEHAVIOR_H_

#include "Objective.h"

struct Behavior {
    struct Object *object;
    struct Objective *entryConditions;
    struct Behavior *firstChild;
    struct Behavior *nextSibling;
};

struct Behavior *Behavior_new(struct Objective *new_entryCondition);
void Behavior_delete(struct Behavior *behavior);

int Behavior_areEntryConditionsReached(struct Behavior *behavior, struct State *currentState);
struct Behavior *Behavior_fetchFirstReachedChildOrReturnSelf(struct Behavior *self, struct State *currentState);
void Behavior_addChild(struct Behavior *behavior, struct Behavior *newChild);

#endif // BEHAVIOR_H_
