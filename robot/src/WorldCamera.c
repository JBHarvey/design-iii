#include <stdlib.h>
#include "WorldCamera.h"

struct WorldCamera *WorldCamera_new(void)
{
    struct Object *new_object = Object_new();
    struct Sensor *new_map_sensor = Sensor_new();
    struct Sensor *new_robot_sensor = Sensor_new();
    struct Map *new_map = Map_new();
    struct Pose *new_robot_pose = Pose_zero();
    int new_robot_radius = 0;
    struct WorldCamera *pointer = (struct WorldCamera *) malloc(sizeof(struct WorldCamera));

    pointer->object = new_object;
    pointer->map_sensor = new_map_sensor;
    pointer->robot_sensor = new_robot_sensor;
    pointer->map = new_map;
    pointer->robot_pose = new_robot_pose;
    pointer->robot_radius = new_robot_radius;

    return pointer;
}

void WorldCamera_delete(struct WorldCamera *world_camera)
{
    Object_removeOneReference(world_camera->object);

    if(Object_canBeDeleted(world_camera->object)) {
        Object_delete(world_camera->object);
        Sensor_delete(world_camera->map_sensor);
        Sensor_delete(world_camera->robot_sensor);
        Map_delete(world_camera->map);
        Pose_delete(world_camera->robot_pose);
        free(world_camera);
    }
}
