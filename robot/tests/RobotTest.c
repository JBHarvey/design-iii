#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"

static struct Robot *robot;

Test(Robot, creation_destruction)
{
    struct Robot *robot = Robot_new();
    struct Pose *zero = Pose_zero();
    struct State *default_state = State_new(zero);
    struct ManchesterCode *manchester_code = ManchesterCode_new();
    struct Flags *flags = Flags_new();
    struct Behavior *behavior = BehaviorBuilder_build(
                                    BehaviorBuilder_withFlags(flags,
                                            BehaviorBuilder_end()));

    cr_assert(Pose_haveTheSameValues(robot->current_state->pose, zero));
    cr_assert(Flags_haveTheSameValues(robot->current_state->flags, default_state->flags));
    cr_assert_eq(robot->manchester_code->painting_number, manchester_code->painting_number);
    cr_assert_eq(robot->manchester_code->scale_factor, manchester_code->scale_factor);
    cr_assert_eq(robot->manchester_code->orientation, manchester_code->orientation);


    Behavior_delete(behavior);
    Flags_delete(flags);
    ManchesterCode_delete(manchester_code);
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
