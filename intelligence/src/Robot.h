#ifndef ROBOT_H_
#define ROBOT_H_

#include "State.h"

struct DefaultValues;

struct Robot {
    struct Object *object;
    struct DefaultValues *defaultValues;
    struct State *currentState;
};

struct Robot *Robot_new();
void Robot_delete(struct Robot *robot);

void Robot_takePicture(struct Robot *robot);

#endif // ROBOT_H_
