#include <stdlib.h>
#include "Robot.h"

struct DefaultValues {
    struct Pose *pose;
};

struct Robot *Robot_new()
{
    struct DefaultValues *default_values = (struct DefaultValues *) malloc(sizeof(struct DefaultValues));
    default_values->pose = Pose_zero();

    struct Object *new_object = Object_new();
    struct State *new_state = State_new(default_values->pose);
    struct WorldCamera *new_world_camera = WorldCamera_new();
    struct Robot *pointer = (struct Robot *) malloc(sizeof(struct Robot));

    pointer->object = new_object;
    pointer->default_values = default_values;
    pointer->current_state = new_state;
    pointer->world_camera = new_world_camera;

    return pointer;
}

void Robot_delete(struct Robot *robot)
{
    Object_removeOneReference(robot->object);

    if(Object_canBeDeleted(robot->object)) {
        Object_delete(robot->object);
        State_delete(robot->current_state);
        WorldCamera_delete(robot->world_camera);

        /* DefaultValues destruction */
        Pose_delete(robot->default_values->pose);
        free(robot->default_values);

        free(robot);
    }
}

void Robot_takePicture(struct Robot *robot)
{
    //TODO : Take the actual picture with the on-board camera;
    Flags_setPictureTaken(robot->current_state->flags, TRUE);
}
