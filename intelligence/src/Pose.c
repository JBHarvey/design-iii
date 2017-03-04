#include <stdlib.h>
#include <stdio.h>
#include "Pose.h"


struct Pose *Pose_new(int new_x, int new_y, int new_theta)
{
    struct Object *new_object = Object_new();
    struct Angle *new_angle = Angle_new(new_theta);
    struct Pose *pointer = (struct Pose *) malloc(sizeof(struct Pose));

    pointer->object = new_object;
    pointer->angle = new_angle;

    pointer->x = new_x;
    pointer->y = new_y;

    return pointer;
}

void Pose_delete(struct Pose *pose)
{
    Object_removeOneReference(pose->object);

    if(Object_canBeDeleted(pose->object)) {
        Angle_delete(pose->angle);
        Object_delete(pose->object);
        free(pose);
    }
}

void Pose_copyValuesFrom(struct Pose *recipient, struct Pose *source)
{
    recipient->x = source->x;
    recipient->y = source->y;
    recipient->angle->theta = source->angle->theta;
}

int Pose_haveTheSameValues(struct Pose *pose, struct Pose *otherPose)
{
    return(
              pose->x == otherPose->x &&
              pose->y == otherPose->y &&
              Angle_smallestAngleBetween(pose->angle, otherPose->angle) == 0
          );
}
