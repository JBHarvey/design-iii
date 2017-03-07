#include <stdlib.h>
#include "Robot.h"

struct DefaultValues {
    struct Pose *pose;
};

struct Robot *Robot_new()
{
    struct DefaultValues *defaultValues = (struct DefaultValues *) malloc(sizeof(struct DefaultValues));
    defaultValues->pose = Pose_zero();

    struct Object *new_object = Object_new();
    struct State *new_state = State_new(defaultValues->pose);
    struct WorldCamera *new_worldCamera = WorldCamera_new();
    struct Robot *pointer = (struct Robot *) malloc(sizeof(struct Robot));

    pointer->object = new_object;
    pointer->defaultValues = defaultValues;
    pointer->currentState = new_state;
    pointer->worldCamera = new_worldCamera;

    return pointer;
}

void Robot_delete(struct Robot *robot)
{
    Object_removeOneReference(robot->object);

    if(Object_canBeDeleted(robot->object)) {
        Object_delete(robot->object);
        State_delete(robot->currentState);
        WorldCamera_delete(robot->worldCamera);

        /* DefaultValues destruction */
        Pose_delete(robot->defaultValues->pose);
        free(robot->defaultValues);

        free(robot);
    }
}

void Robot_takePicture(struct Robot *robot)
{
    //TODO : Take the actual picture with the on-board camera;
    Flags_setPictureTaken(robot->currentState->flags, TRUE);
}
