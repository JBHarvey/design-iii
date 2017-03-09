#include <stdlib.h>
#include <stdio.h>
#include "Pose.h"


struct Pose *Pose_zero(void)
{
    return Pose_new(0, 0, 0);
}

struct Pose *Pose_new(int newX, int newY, int new_theta)
{
    struct Object *new_object = Object_new();
    struct Coordinates *new_coordinates = Coordinates_new(newX, newY);
    struct Angle *new_angle = Angle_new(new_theta);
    struct Pose *pointer = (struct Pose *) malloc(sizeof(struct Pose));

    pointer->object = new_object;
    pointer->angle = new_angle;
    pointer->coordinates = new_coordinates;

    return pointer;
}

void Pose_delete(struct Pose *pose)
{
    Object_removeOneReference(pose->object);

    if(Object_canBeDeleted(pose->object)) {
        Angle_delete(pose->angle);
        Coordinates_delete(pose->coordinates);
        Object_delete(pose->object);
        free(pose);
    }
}

void Pose_copyValuesFrom(struct Pose *recipient, struct Pose *source)
{
    Coordinates_copyValuesFrom(recipient->coordinates, source->coordinates);
    recipient->angle->theta = source->angle->theta;
}

int Pose_haveTheSameValues(struct Pose *pose, struct Pose *other_pose)
{
    return(
              Coordinates_haveTheSameValues(pose->coordinates, other_pose->coordinates)
              && Angle_smallestAngleBetween(pose->angle, other_pose->angle) == 0
          );
}