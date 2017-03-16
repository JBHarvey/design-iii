#include <stdlib.h>
#include <stdio.h>
#include "Actuator.h"

struct Actuator *Actuator_new(void)
{
    struct Object *new_object = Object_new();
    struct Actuator *pointer = (struct Actuator *) malloc(sizeof(struct Actuator));

    pointer->object = new_object;
    pointer->has_prepared_new_command = 0;

    return pointer;
}

void Actuator_delete(struct Actuator *actuator)
{
    Object_removeOneReference(actuator->object);

    if(Object_canBeDeleted(actuator->object)) {
        Object_delete(actuator->object);
        free(actuator);
    }
}

void Actuator_preparesCommand(struct Actuator *actuator)
{
    actuator->has_prepared_new_command = 1;
}

void Actuator_sendsCommand(struct Actuator *actuator)
{
    actuator->has_prepared_new_command = 0;
}
