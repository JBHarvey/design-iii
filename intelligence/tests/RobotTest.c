#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"

const int INITIAL_X = 0;
const int INITIAL_Y = 0;
const int INITIAL_THETA = 0;

static struct Robot *robot;

Test(Robot, creation_destruction)
{
    struct Robot *robot = Robot_new();
    cr_assert(
        robot->currentState->pose->x == INITIAL_X &&
        robot->currentState->pose->y == INITIAL_Y &&
        robot->currentState->pose->angle->theta == INITIAL_THETA);

    Robot_delete(robot);
}

void setupRobot(void)
{
    robot = Robot_new();
}

void teardownRobot(void)
{
    Robot_delete(robot);
}

Test(Robot, given_initialRobot_when_takesAPicture_then_thePicureTakenFlagValueIsOne
     , .init = setupRobot
     , .fini = teardownRobot)
{
    Robot_takePicture(robot);
    cr_assert(robot->currentState->flags->pictureTaken);
}

// TODO: take the actual picture
