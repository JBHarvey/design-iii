#ifndef POSE_H_
#define POSE_H_
#include "Coordinates.h"
#include "Angle.h"

struct Pose {
    struct Object *object;
    struct Coordinates *coordinates;
    struct Angle *angle;
};

struct Pose *Pose_zero(void);
struct Pose *Pose_new(int newX, int newY, int new_theta);
void Pose_delete(struct Pose *pose);

void Pose_copyValuesFrom(struct Pose *recipient, struct Pose *source);
int Pose_haveTheSameValues(struct Pose *pose, struct Pose *other_pose);

#endif // POSE_H_
