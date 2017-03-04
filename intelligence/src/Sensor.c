#include <stdlib.h>
#include <stdio.h>
#include "Sensor.h"

struct Sensor *Sensor_new()
{
    struct Object *new_object = Object_new();
    struct Sensor *pointer = (struct Sensor *) malloc(sizeof(struct Sensor));

    pointer->object = new_object;
    pointer->hasReceivedNewData = 0;

    return pointer;
}

void Sensor_delete(struct Sensor *sensor)
{
    Object_removeOneReference(sensor->object);

    if(Object_canBeDeleted(sensor->object)) {
        Object_delete(sensor->object);
        free(sensor);
    }
}

/*
void Sensor_addOneReference(struct Sensor *sensor)
{
    ++sensor->hasReceivedNewData;
}

void Sensor_removeOneReference(struct Sensor *sensor)
{
    --sensor->hasReceivedNewData;
}
*/
