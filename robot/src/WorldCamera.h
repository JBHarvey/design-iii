#ifndef WORLD_CAMERA_H_
#define WORLD_CAMERA_H_

#include "Sensor.h"
#include "Map.h"

struct WorldCamera {
    struct Object *object;
    struct Sensor *map_sensor;
    struct Sensor *robot_sensor;
    struct Map *map;
    struct Pose *robot_pose;
    int robot_radius;
};

struct WorldCamera *WorldCamera_new(void);
void WorldCamera_delete(struct WorldCamera *world_camera);
void WorldCamera_readPoseData(struct WorldCamera *world_camera, struct Pose *robot_pose);

#endif // WORLD_CAMERA_H_
