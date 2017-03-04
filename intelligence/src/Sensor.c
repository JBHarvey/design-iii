#include <stdlib.h>
#include <stdio.h>
#include "Sensor.h"

struct Sensor *Sensor_new(void)
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

void Sensor_receivesData(struct Sensor *sensor)
{
    sensor->hasReceivedNewData = 1;
}

void Sensor_readsData(struct Sensor *sensor)
{
    sensor->hasReceivedNewData = 0;
}
