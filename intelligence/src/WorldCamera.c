#include <stdlib.h>
#include "WorldCamera.h"

struct WorldCamera *WorldCamera_new(void)
{
    struct Object *new_object = Object_new();
    struct Sensor *new_sensor = Sensor_new();
    struct WorldCamera *pointer = (struct WorldCamera *) malloc(sizeof(struct WorldCamera));

    pointer->object = new_object;
    pointer->sensor = new_sensor;

    return pointer;
}

void WorldCamera_delete(struct WorldCamera *worldCamera)
{
    Object_removeOneReference(worldCamera->object);

    if(Object_canBeDeleted(worldCamera->object)) {
        Object_delete(worldCamera->object);
        Sensor_delete(worldCamera->sensor);
        free(worldCamera);
    }
}
