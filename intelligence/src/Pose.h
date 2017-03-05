#ifndef POSE_H_
#define POSE_H_
#include "Angle.h"

struct Pose {
    struct Object *object;
    int x;        /* Unit: 0.1 mm */
    int y;        /* Unit: 0.1 mm */
    struct Angle *angle;
};

struct Pose *Pose_zero(void);
struct Pose *Pose_new(int new_x, int new_y, int new_theta);
void Pose_delete(struct Pose *pose);

void Pose_copyValuesFrom(struct Pose *recipient, struct Pose *source);
int Pose_haveTheSameValues(struct Pose *pose, struct Pose *otherPose);

#endif // POSE_H_
