#include <stdlib.h>
#include "Wheels.h"

struct Wheels *Wheels_new(void)
{
    struct Object *new_object = Object_new();
    struct Sensor *new_sensor = Sensor_new();
    struct Wheels *pointer = (struct Wheels *) malloc(sizeof(struct Wheels));

    pointer->object = new_object;
    pointer->sensor = new_sensor;

    return pointer;
}

void Wheels_delete(struct Wheels *wheels)
{
    Object_removeOneReference(wheels->object);

    if(Object_canBeDeleted(wheels->object)) {
        Object_delete(wheels->object);
        Sensor_delete(wheels->sensor);
        free(wheels);
    }
}
