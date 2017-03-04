#ifndef BEHAVIOR_H_
#define BEHAVIOR_H_

#include "Objective.h"

struct Robot;

struct Behavior {
    struct Object *object;
    struct Objective *entryConditions;
    struct Behavior *firstChild;
    struct Behavior *nextSibling;
    void (*action)(struct Robot *);
};

struct Behavior *Behavior_new(struct Objective *new_entryCondition);
void Behavior_delete(struct Behavior *behavior);

int Behavior_areEntryConditionsReached(struct Behavior *behavior, struct State *currentState);
struct Behavior *Behavior_fetchFirstReachedChildOrReturnSelf(struct Behavior *self, struct State *currentState);
void Behavior_addChild(struct Behavior *behavior, struct Behavior *newChild);
void Behavior_changeAction(struct Behavior *behavior, void (*new_action)(struct Robot *));
void Behavior_act(struct Behavior *behavior, struct Robot *robot);

void Behavior_dummyAction(struct Robot *robot);

#endif // BEHAVIOR_H_
