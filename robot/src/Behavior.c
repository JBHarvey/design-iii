#include <stdlib.h>
#include "Behavior.h"

static int hasChild(struct Behavior *behavior)
{
    return behavior->first_child != behavior;
}

static int hasSibling(struct Behavior *behavior)
{
    return behavior->next_sibling != behavior;
}

struct Behavior *Behavior_new(struct Objective *new_entry_conditions)
{
    struct Object *new_object = Object_new();
    struct Behavior *pointer = malloc(sizeof(struct Behavior));

    pointer->object = new_object;
    pointer->entry_conditions = new_entry_conditions;
    pointer->first_child = pointer;
    pointer->next_sibling = pointer;
    pointer->action = &Behavior_idle;

    Object_addOneReference(new_entry_conditions->object);

    return pointer;
}

void Behavior_delete(struct Behavior *behavior)
{
    Object_removeOneReference(behavior->object);

    if(Object_canBeDeleted(behavior->object)) {
        Object_delete(behavior->object);
        Objective_delete(behavior->entry_conditions);

        if(hasChild(behavior)) {
            Behavior_delete(behavior->first_child);
        }

        if(hasSibling(behavior)) {
            Behavior_delete(behavior->next_sibling);
        }

        free(behavior);
    }
}

int Behavior_areEntryConditionsReached(struct Behavior *behavior, struct State *current_state)
{
    int reached = Objective_isReached(behavior->entry_conditions, current_state);

    return reached;
}

struct Behavior *Behavior_fetchFirstReachedChildOrReturnSelf(struct Behavior *self, struct State *current_state)
{

    if(hasChild(self)) {

        struct Behavior *child = self->first_child;

        if(Behavior_areEntryConditionsReached(child, current_state)) {
            return child;
        }

        while(hasSibling(child)) {
            child = child->next_sibling;

            if(Behavior_areEntryConditionsReached(child, current_state)) {
                return child;
            }

        }
    }

    return self;
}

void Behavior_addChild(struct Behavior *behavior, struct Behavior *new_child)
{

    if(behavior == new_child) {
        return;
    }

    if(hasChild(behavior)) {

        struct Behavior *last_child = behavior->first_child;

        while(hasSibling(last_child)) {
            last_child = last_child->next_sibling;
        }

        last_child->next_sibling = new_child;

    } else {
        behavior->first_child = new_child;
    }

    Object_addOneReference(new_child->object);
}

/* This here is present for safety & waiting times purposes.
 * It prevents action of doing anything.
 * */
void Behavior_idle(struct Robot *robot) {}

void Behavior_changeAction(struct Behavior *behavior, void (*new_action)(struct Robot *))
{
    behavior->action = new_action;
}

void Behavior_act(struct Behavior *behavior, struct Robot *robot)
{
    (*behavior->action)(robot);
}
