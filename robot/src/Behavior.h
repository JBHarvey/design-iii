#ifndef BEHAVIOR_H_
#define BEHAVIOR_H_

#include "Objective.h"

struct Robot;
struct Behavior {
    struct Object *object;
    struct Objective *entry_conditions;
    struct Behavior *first_child;
    struct Behavior *next_sibling;
    void (*action)(struct Robot *);
};

struct Behavior *Behavior_new(struct Objective *new_entry_condition);
void Behavior_delete(struct Behavior *behavior);

int Behavior_areEntryConditionsReached(struct Behavior *behavior, struct State *current_state);
struct Behavior *Behavior_fetchFirstReachedChildOrReturnSelf(struct Behavior *self, struct State *current_state);
void Behavior_addChild(struct Behavior *behavior, struct Behavior *new_child);
void Behavior_changeAction(struct Behavior *behavior, void (*new_action)(struct Robot *));
void Behavior_act(struct Behavior *behavior, struct Robot *robot);

void Behavior_idle(struct Robot *robot);

#endif // BEHAVIOR_H_
