#include <stdlib.h>
#include "Behavior.h"

struct Behavior *Behavior_new(struct Objective *new_entryConditions)
{
    struct Object *new_object = Object_new();
    struct Behavior *pointer = (struct Behavior *) malloc(sizeof(struct Behavior));

    pointer->object = new_object;
    pointer->entryConditions = new_entryConditions;

    Object_addOneReference(new_entryConditions->object);

    return pointer;
}

void Behavior_delete(struct Behavior *behavior)
{
    Object_removeOneReference(behavior->object);

    if(Object_canBeDeleted(behavior->object)) {
        Object_delete(behavior->object);
        Objective_delete(behavior->entryConditions);

        free(behavior);
    }
}

int Behavior_entryConditionsAreReached(struct Behavior *behavior, struct State *currentState)
{
    int reached = Objective_isReached(behavior->entryConditions, currentState);

    return reached;
}
