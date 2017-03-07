#ifndef WORLD_CAMERA_H_
#define WORLD_CAMERA_H_

#include "Sensor.h"
#include "Map.h"

struct WorldCamera {
    struct Object *object;
    struct Sensor *mapSensor;
    struct Sensor *robotSensor;
    struct Map *map;
    struct Pose *robotPose;
    int robotRadius;
};

struct WorldCamera *WorldCamera_new(void);
void WorldCamera_delete(struct WorldCamera *worldCamera);

#endif // WORLD_CAMERA_H_
