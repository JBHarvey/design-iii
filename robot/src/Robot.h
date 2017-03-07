#ifndef ROBOT_H_
#define ROBOT_H_

#include "WorldCamera.h"

struct Robot {
    struct Object *object;
    struct DefaultValues *defaultValues;
    struct State *currentState;
    struct WorldCamera *worldCamera;
};

struct Robot *Robot_new();
void Robot_delete(struct Robot *robot);

void Robot_takePicture(struct Robot *robot);

#endif // ROBOT_H_
