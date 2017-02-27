#include <stdlib.h>
#include "Behavior.h"

struct Behavior *Behavior_new(struct Objective *new_entryCondition)
{
    struct Object *new_object = Object_new();
    struct Behavior *pointer = (struct Behavior *) malloc(sizeof(struct Behavior));

    pointer->object = new_object;
    pointer->entryCondition = new_entryCondition;

    Object_addOneReference(new_entryCondition->object);

    return pointer;
}

void Behavior_delete(struct Behavior *behavior)
{
    Object_removeOneReference(behavior->object);

    if(Object_canBeDeleted(behavior->object)) {
        Object_delete(behavior->object);
        Objective_delete(behavior->entryCondition);

        free(behavior);
    }
}
/*
int Behavior_isReached(struct Behavior *behavior, struct State *currentState)
{
    int reached = xIsWithinToleranceOfGoal(behavior, currentState)
                  && yIsWithinToleranceOfGoal(behavior, currentState)
                  && thetaIsWithinToleranceOfGoal(behavior, currentState)
                  && flagsAreTheSame(behavior, currentState);

    return reached;
}
*/
