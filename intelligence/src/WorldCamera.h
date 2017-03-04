#ifndef WORLD_CAMERA_H_
#define WORLD_CAMERA_H_

#include "Sensor.h"
#include "Pose.h"

struct WorldCamera {
    struct Object *object;
    struct Sensor *sensor;
};

struct WorldCamera *WorldCamera_new(void);
void WorldCamera_delete(struct WorldCamera *worldCamera);

#endif // WORLD_CAMERA_H_
