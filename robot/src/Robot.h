#ifndef ROBOT_H_
#define ROBOT_H_

#include "WorldCamera.h"
#include "Wheels.h"
#include "ManchesterCode.h"
#include "OnboardCamera.h"

struct Robot {
    struct Object *object;
    struct DefaultValues *default_values;
    struct State *current_state;
    struct WorldCamera *world_camera;
    struct Wheels *wheels;
    struct ManchesterCode *manchester_code;
};

struct Robot *Robot_new();
void Robot_delete(struct Robot *robot);

void Robot_takePicture(struct Robot *robot);

#endif // ROBOT_H_
