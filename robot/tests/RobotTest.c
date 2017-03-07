#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"

static struct Robot *robot;

Test(Robot, creation_destruction)
{
    struct Robot *robot = Robot_new();
    struct Pose *zero = Pose_zero();
    cr_assert(Pose_haveTheSameValues(robot->currentState->pose, zero));

    Robot_delete(robot);
    Pose_delete(zero);
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
