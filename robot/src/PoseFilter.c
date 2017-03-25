#include <stdlib.h>
#include "PoseFilter.h"

struct PoseFilter *PoseFilter_new(struct Robot *new_robot)
{
    struct Object *new_object = Object_new();
    struct PoseFilter *pointer =  malloc(sizeof(struct PoseFilter));

    pointer->object = new_object;
    pointer->robot = new_robot;

    Object_addOneReference(new_robot->object);
    return pointer;
}

void PoseFilter_delete(struct PoseFilter *pose_filter)
{
    Object_removeOneReference(pose_filter->object);

    if(Object_canBeDeleted(pose_filter->object)) {
        Object_delete(pose_filter->object);
        Robot_delete(pose_filter->robot);

        free(pose_filter);
    }
}

struct PoseFilter_Callbacks PoseFilter_fetchCallbacks(void)
{
    struct PoseFilter_Callbacks callbacks = {
        .updateFromCameraOnly = &PoseFilter_updateFromCameraOnly
    };
    return callbacks;
}

void PoseFilter_executeFilter(struct PoseFilter *pose_filter, void (*filter)(struct PoseFilter *))
{
    (*(filter))(pose_filter);
}

void PoseFilter_updateFromCameraOnly(struct PoseFilter *pose_filter)
{
}
