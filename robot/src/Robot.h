#ifndef ROBOT_H_
#define ROBOT_H_

#include "RobotBehaviors.h"

struct Robot {
    struct Object *object;
    struct DefaultValues *default_values;
    struct State *current_state;
    struct ManchesterCode *manchester_code;
    struct Wheels *wheels;
    struct WorldCamera *world_camera;
    struct Behavior *behavior;
};

struct Robot *Robot_new();
void Robot_delete(struct Robot *robot);

void Robot_takePicture(struct Robot *robot);

#endif // ROBOT_H_
