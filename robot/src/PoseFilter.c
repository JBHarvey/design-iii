#include <time.h>
#include <stdlib.h>
#include "PoseFilter.h"

const int NUMBER_OF_PARTICLES = 50;

struct PoseFilter *PoseFilter_new(struct Robot *new_robot)
{
    struct Object *new_object = Object_new();
    struct Pose **new_particles = malloc(NUMBER_OF_PARTICLES * sizeof(struct Pose *));
    int *new_particles_status = calloc(NUMBER_OF_PARTICLES, sizeof(int));
    struct PoseFilter *pointer =  malloc(sizeof(struct PoseFilter));

    pointer->object = new_object;
    pointer->robot = new_robot;

    pointer->particles = new_particles;
    pointer->particles_status = new_particles_status;

    Object_addOneReference(new_robot->object);
    return pointer;
}

void PoseFilter_delete(struct PoseFilter *pose_filter)
{
    Object_removeOneReference(pose_filter->object);

    if(Object_canBeDeleted(pose_filter->object)) {
        Object_delete(pose_filter->object);
        Robot_delete(pose_filter->robot);

        for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
            if(pose_filter->particles_status[i]) {
                Pose_delete(pose_filter->particles[i]);
            }
        }

        free(pose_filter->particles_status);
        free(pose_filter->particles);
        free(pose_filter);
    }
}

struct PoseFilter_Callbacks PoseFilter_fetchCallbacks(void)
{
    struct PoseFilter_Callbacks callbacks = {
        .updateFromCameraOnly = &PoseFilter_updateFromCameraOnly,
        .updateFromCameraAndWheels = &PoseFilter_updateFromCameraAndWheels
    };
    return callbacks;
}

void PoseFilter_executeFilter(struct PoseFilter *pose_filter, void (*filter)(struct PoseFilter *))
{
    (*(filter))(pose_filter);
}

void PoseFilter_updateFromCameraOnly(struct PoseFilter *pose_filter)
{
    struct Robot *robot = pose_filter->robot;
    int new_data_from_world = robot->world_camera->robot_sensor->has_received_new_data;

    if(new_data_from_world) {
        struct Pose *current_robot_pose = robot->current_state->pose;
        struct Pose *world_camera_pose = robot->world_camera->robot_pose;
        Pose_copyValuesFrom(current_robot_pose, world_camera_pose);
        Sensor_readsData(robot->world_camera->robot_sensor);
    }
}

static void populateParticles(struct Map *map, struct Pose **particles, int *particles_status)
{
    int table_length = map->south_eastern_table_corner->x - map->south_western_table_corner->x;
    int table_width = map->north_western_table_corner->y - map->south_western_table_corner->y;

    srand((unsigned int) time(NULL));

    for(int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        if(!particles_status[i]) {
            particles[i] = Pose_new((int)(rand() % table_length),
                                    (int)(rand() % table_width),
                                    (int)(rand() % 2 * PI) - PI);
        }
    }
}

static void predict(struct Pose **particles, struct Pose *robot_current_pose, struct Wheels *wheels,
                    struct WorldCamera *world_camera)
{
    int new_command_for_wheels = wheels->translation_actuator->has_prepared_new_command;

}

void PoseFilter_updateFromCameraAndWheels(struct PoseFilter * pose_filter)
{
    struct Robot *robot = pose_filter->robot;
    populateParticles(robot->world_camera->map, pose_filter->particles, pose_filter->particles_status);

}
