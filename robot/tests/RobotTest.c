#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"

struct Robot *robot;
struct DataSender_Callbacks validation_callbacks;
int validation_ready_to_start_is_sent;
const int SIGNAL_SENT = 1;
const int SIGNAL_NOT_SENT = 0;

Test(Robot, creation_destruction)
{
    struct Robot *robot = Robot_new();
    struct Pose *zero = Pose_zero();
    struct State *default_state = State_new(zero);
    struct ManchesterCode *manchester_code = ManchesterCode_new();
    void (*initial_behavior_action)(struct Robot *) = &Navigator_updateNavigableMap;

    cr_assert(Pose_haveTheSameValues(robot->current_state->pose, zero));
    cr_assert(Flags_haveTheSameValues(robot->current_state->flags, default_state->flags));
    cr_assert_eq(robot->manchester_code->painting_number, manchester_code->painting_number);
    cr_assert_eq(robot->manchester_code->scale_factor, manchester_code->scale_factor);
    cr_assert_eq(robot->manchester_code->orientation, manchester_code->orientation);
    cr_assert_eq(robot->behavior->action, initial_behavior_action);


    struct Behavior *second_behavior = robot->behavior->first_child;
    void (*second_behavior_action)(struct Robot *) = &Robot_sendReadyToStartSignal;
    struct Flags *second_flags = second_behavior->entry_conditions->goal_state->flags;
    struct Flags *map_is_ready_flags = Flags_new();
    Flags_setNavigableMapIsReady(map_is_ready_flags, 1);

    cr_assert_eq(second_behavior->action, second_behavior_action);
    cr_assert(Flags_haveTheSameValues(second_flags, map_is_ready_flags));


    struct Behavior *third_behavior = second_behavior->first_child;
    void (*idle_action)(struct Robot *) = &Behavior_idle;
    struct Flags *third_flags = third_behavior->entry_conditions->goal_state->flags;
    struct Flags *ready_to_start_received_flags = Flags_new();
    Flags_setReadyToStartReceivedByStation(ready_to_start_received_flags, 1);

    cr_assert_eq(third_behavior->action, idle_action);
    cr_assert(Flags_haveTheSameValues(third_flags, ready_to_start_received_flags));


    struct Behavior *fourth_behavior = third_behavior->first_child;
    void (*plan_navigation_to_antenna_start)(struct Robot *) = &Navigator_planTowardsAntennaStart;
    struct Flags *fourth_flags = fourth_behavior->entry_conditions->goal_state->flags;
    struct Flags *start_signal_received_flags = Flags_new();
    Flags_setStartCycleSignalReceived(start_signal_received_flags, 1);

    cr_assert_eq(fourth_behavior->action, plan_navigation_to_antenna_start);
    cr_assert(Flags_haveTheSameValues(fourth_flags, start_signal_received_flags));

    Flags_delete(map_is_ready_flags);
    Flags_delete(ready_to_start_received_flags);
    Flags_delete(start_signal_received_flags);
    ManchesterCode_delete(manchester_code);
    Robot_delete(robot);
    Pose_delete(zero);
    State_delete(default_state);
}

void validateReadyToStartIsSent(void)
{
    validation_ready_to_start_is_sent = SIGNAL_SENT;
}

void setup_robot(void)
{
    robot = Robot_new();
    validation_ready_to_start_is_sent = SIGNAL_NOT_SENT;
    validation_callbacks.sendSignalReadyToStart = &validateReadyToStartIsSent;
    DataSender_changeTarget(robot->data_sender, validation_callbacks);
}

void teardown_robot(void)
{
    Robot_delete(robot);
}

Test(Robot, given_aRobot_when_askedToSendAReadyToStartSignal_then_theSignalIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_sendReadyToStartSignal(robot);
    cr_assert(validation_ready_to_start_is_sent);
}

Test(Robot, given_initialRobot_when_takesAPicture_then_thePicureTakenFlagValueIsOne
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_takePicture(robot);
    cr_assert(robot->current_state->flags->picture_taken);
}

// TODO: take the actual picture
