#ifndef POSE_H_
#define POSE_H_

struct Pose {
    int x;        /* Unit: 0.1 mm */
    int y;        /* Unit: 0.1 mm */
    int theta;    /* Unit: 0.0001 rad */
};

struct Pose * Pose_new(int new_x, int new_y, int new_theta);
void Pose_delete(struct Pose * pose);
void Pose_print(struct Pose * pose);

#endif // POSE_H_
