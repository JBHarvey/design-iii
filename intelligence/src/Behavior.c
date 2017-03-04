#include <stdlib.h>
#include "Behavior.h"

static int Behavior_hasChild(struct Behavior *behavior)
{
    return behavior->firstChild != behavior;
}

static int Behavior_hasSibling(struct Behavior *behavior)
{
    return behavior->nextSibling != behavior;
}

struct Behavior *Behavior_new(struct Objective *new_entryConditions)
{
    struct Object *new_object = Object_new();
    struct Behavior *pointer = (struct Behavior *) malloc(sizeof(struct Behavior));

    pointer->object = new_object;
    pointer->entryConditions = new_entryConditions;
    pointer->firstChild = pointer;
    pointer->nextSibling = pointer;
    pointer->action = &Behavior_dummyAction;

    Object_addOneReference(new_entryConditions->object);

    return pointer;
}

void Behavior_delete(struct Behavior *behavior)
{
    Object_removeOneReference(behavior->object);

    if(Object_canBeDeleted(behavior->object)) {
        Object_delete(behavior->object);
        Objective_delete(behavior->entryConditions);

        if(Behavior_hasChild(behavior)) {
            Behavior_delete(behavior->firstChild);
        }

        if(Behavior_hasSibling(behavior)) {
            Behavior_delete(behavior->nextSibling);
        }

        free(behavior);
    }
}

int Behavior_areEntryConditionsReached(struct Behavior *behavior, struct State *currentState)
{
    int reached = Objective_isReached(behavior->entryConditions, currentState);

    return reached;
}

struct Behavior *Behavior_fetchFirstReachedChildOrReturnSelf(struct Behavior *self, struct State *currentState)
{

    if(Behavior_hasChild(self)) {

        struct Behavior *child = self->firstChild;

        if(Behavior_areEntryConditionsReached(child, currentState)) {
            return child;
        }

        while(Behavior_hasSibling(child)) {
            child = child->nextSibling;

            if(Behavior_areEntryConditionsReached(child, currentState)) {
                return child;
            }

        }
    }

    return self;
}

void Behavior_addChild(struct Behavior *behavior, struct Behavior *newChild)
{

    if(behavior == newChild) {
        return;
    }

    if(Behavior_hasChild(behavior)) {

        struct Behavior *lastChild = behavior->firstChild;

        while(Behavior_hasSibling(lastChild)) {
            lastChild = lastChild->nextSibling;
        }

        lastChild->nextSibling = newChild;

    } else {
        behavior->firstChild = newChild;
    }

    Object_addOneReference(newChild->object);
}

/* This here is present for safety purposes.
 * It prevents action of doing anything
 * if it isn't initialised.
 * */
void Behavior_dummyAction(struct Robot *robot) {}

void Behavior_changeAction(struct Behavior *behavior, void (*new_action)(struct Robot *))
{
    behavior->action = new_action;
}

void Behavior_act(struct Behavior *behavior, struct Robot *robot)
{
    (*behavior->action)(robot);
}
