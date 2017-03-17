#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"

static struct Robot *robot;

Test(Robot, creation_destruction)
{
    struct Robot *robot = Robot_new();
    struct Pose *zero = Pose_zero();
    struct State *default_state = State_new(zero);

    cr_assert(Pose_haveTheSameValues(robot->current_state->pose, zero));
    cr_assert(Flags_haveTheSameValues(robot->current_state->flags, default_state->flags));


    Robot_delete(robot);
    Pose_delete(zero);
    State_delete(default_state);
}

void setup_robot(void)
{
    robot = Robot_new();
}

void teardown_robot(void)
{
    Robot_delete(robot);
}

Test(Robot, given_initialRobot_when_takesAPicture_then_thePicureTakenFlagValueIsOne
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_takePicture(robot);
    cr_assert(robot->current_state->flags->picture_taken);
}

// TODO: take the actual picture
