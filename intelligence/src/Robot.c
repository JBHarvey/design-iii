#include <stdlib.h>
#include "Robot.h"

struct DefaultValues {
    int x;
    int y;
    int theta;
    struct Pose *pose;
};

struct Robot *Robot_new()
{
    struct DefaultValues *defaultValues = (struct DefaultValues *) malloc(sizeof(struct DefaultValues));
    defaultValues->x = 0;
    defaultValues->y = 0;
    defaultValues->theta = 0;
    defaultValues->pose = Pose_new(defaultValues->x, defaultValues->y, defaultValues->theta);

    struct Object *new_object = Object_new();
    struct State *new_state = State_new(defaultValues->pose);
    struct Robot *pointer = (struct Robot *) malloc(sizeof(struct Robot));

    pointer->object = new_object;
    pointer->defaultValues = defaultValues;
    pointer->currentState = new_state;

    return pointer;
}

void Robot_delete(struct Robot *robot)
{
    Object_removeOneReference(robot->object);

    if(Object_canBeDeleted(robot->object)) {
        Object_delete(robot->object);
        State_delete(robot->currentState);

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
