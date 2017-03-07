#include <stdlib.h>
#include "WorldCamera.h"

struct WorldCamera *WorldCamera_new(void)
{
    struct Object *new_object = Object_new();
    struct Sensor *new_mapSensor = Sensor_new();
    struct Sensor *new_robotSensor = Sensor_new();
    struct Map *new_map = Map_new();
    struct Pose *new_robotPose = Pose_zero();
    int new_robotRadius = 0;
    struct WorldCamera *pointer = (struct WorldCamera *) malloc(sizeof(struct WorldCamera));

    pointer->object = new_object;
    pointer->mapSensor = new_mapSensor;
    pointer->robotSensor = new_robotSensor;
    pointer->map = new_map;
    pointer->robotPose = new_robotPose;
    pointer->robotRadius = new_robotRadius;

    return pointer;
}

void WorldCamera_delete(struct WorldCamera *worldCamera)
{
    Object_removeOneReference(worldCamera->object);

    if(Object_canBeDeleted(worldCamera->object)) {
        Object_delete(worldCamera->object);
        Sensor_delete(worldCamera->mapSensor);
        Sensor_delete(worldCamera->robotSensor);
        Map_delete(worldCamera->map);
        Pose_delete(worldCamera->robotPose);
        free(worldCamera);
    }
}
