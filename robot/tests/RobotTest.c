#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"
#include "Timer.h"

struct Robot *robot;
struct DataSender_Callbacks validation_data_sender_callbacks;
struct CommandSender_Callbacks validation_command_sender_callbacks;
int validation_ready_to_start_is_sent;
int validation_planned_trajectory_is_sent;
int validation_pose_estimate_is_sent;
int validation_lower_pen_command_is_sent;
int validation_rise_pen_command_is_sent;
const int SIGNAL_SENT = 1;
const int SIGNAL_NOT_SENT = 0;
const int PEN_DOWN_COMMAND_SENT = 1;
const int PEN_DOWN_COMMAND_NOT_SENT = 0;
const int PEN_UP_COMMAND_SENT = 1;
const int PEN_UP_COMMAND_NOT_SENT = 0;
int manchester_code_command_count;

void assertBehaviorHasFreeFlagsEntry(struct Behavior *behavior)
{
    struct Flags *irrelevant_flags = Flags_irrelevant();
    cr_assert(Flags_haveTheSameValues(irrelevant_flags, behavior->entry_conditions->goal_state->flags));
    Flags_delete(irrelevant_flags);
}

void assertBehaviorHasOrientationEntry(struct Behavior *behavior, enum CardinalDirection orientation)
{
    struct Pose *angle_tolerance = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_DEFAULT);
    cr_assert(Pose_haveTheSameValues(angle_tolerance, behavior->entry_conditions->tolerances->pose));
    Pose_delete(angle_tolerance);
    int angle;

    switch(orientation) {
        case EAST:
            angle = 0;
            break;

        case NORTH:
            angle = HALF_PI;
            break;

        case WEST:
            angle = PI;
            break;

        default:
            angle = 0;
            break;
    }

    cr_assert_eq(angle, behavior->entry_conditions->goal_state->pose->angle->theta);

    assertBehaviorHasFreeFlagsEntry(behavior);
}

void assertBehaviorHasFreePoseEntry(struct Behavior *behavior)
{
    struct Pose *max_pose_tolerance = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_MAX);
    cr_assert(Pose_haveTheSameValues(max_pose_tolerance, behavior->entry_conditions->tolerances->pose));
    Pose_delete(max_pose_tolerance);
}

void assertBehaviorHasFreeEntry(struct Behavior *behavior)
{
    assertBehaviorHasFreeFlagsEntry(behavior);
    assertBehaviorHasFreePoseEntry(behavior);
}

void assertBehaviorHasFreeTrajectoryEntry(struct Behavior *behavior)
{
    struct Pose *free_coordinates_tolerance = Pose_new(X_TOLERANCE_DEFAULT, Y_TOLERANCE_DEFAULT, THETA_TOLERANCE_MAX);
    cr_assert(Pose_haveTheSameValues(free_coordinates_tolerance, behavior->entry_conditions->tolerances->pose));
    Pose_delete(free_coordinates_tolerance);
    assertBehaviorHasFreeFlagsEntry(behavior);
}

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
    cr_assert_eq(robot->first_behavior->action, initial_behavior_action);


    struct Behavior *second_behavior = robot->first_behavior->first_child;
    void (*second_behavior_action)(struct Robot *) = &Robot_sendReadyToStartSignal;
    struct Flags *second_flags = second_behavior->entry_conditions->goal_state->flags;
    struct Flags *map_is_ready_flags = Flags_new();
    Flags_setNavigableMapIsReady(map_is_ready_flags, 1);

    cr_assert_eq(second_behavior->action, second_behavior_action);
    cr_assert(Flags_haveTheSameValues(second_flags, map_is_ready_flags));
    assertBehaviorHasFreePoseEntry(second_behavior);


    struct Behavior *third_behavior = second_behavior->first_child;
    void (*idle_action)(struct Robot *) = &Behavior_idle;
    struct Flags *third_flags = third_behavior->entry_conditions->goal_state->flags;
    struct Flags *ready_to_start_received_flags = Flags_new();
    Flags_setReadyToStartReceivedByStation(ready_to_start_received_flags, 1);

    cr_assert_eq(third_behavior->action, idle_action);
    cr_assert(Flags_haveTheSameValues(third_flags, ready_to_start_received_flags));
    assertBehaviorHasFreePoseEntry(third_behavior);


    struct Behavior *fourth_behavior = third_behavior->first_child;
    void (*plan_navigation_to_antenna_start)(struct Robot *) = &Navigator_planTowardsAntennaStart;
    struct Flags *fourth_flags = fourth_behavior->entry_conditions->goal_state->flags;
    struct Flags *start_signal_received_flags = Flags_new();
    Flags_setStartCycleSignalReceived(start_signal_received_flags, 1);

    cr_assert_eq(fourth_behavior->action, plan_navigation_to_antenna_start);
    cr_assert(Flags_haveTheSameValues(fourth_flags, start_signal_received_flags));
    assertBehaviorHasFreePoseEntry(fourth_behavior);

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

void validatePlannedTrajectoryIsSent(struct CoordinatesSequence *sequence)
{
    validation_planned_trajectory_is_sent = SIGNAL_SENT;
}

void validatePoseEstimateIsSent(struct Pose *pose)
{
    validation_pose_estimate_is_sent = SIGNAL_SENT;
}

void validateLowerPenIsSent(void)
{
    validation_lower_pen_command_is_sent = PEN_DOWN_COMMAND_SENT;
}

void validateRisePenIsSent(void)
{
    validation_rise_pen_command_is_sent = PEN_UP_COMMAND_SENT;
}

void validateFetchManchesterCodeIsSent(void)
{
    manchester_code_command_count++;
}

void setup_robot(void)
{
    robot = Robot_new();
    validation_ready_to_start_is_sent = SIGNAL_NOT_SENT;
    validation_planned_trajectory_is_sent = SIGNAL_NOT_SENT;
    validation_pose_estimate_is_sent = SIGNAL_NOT_SENT;
    validation_lower_pen_command_is_sent = PEN_DOWN_COMMAND_NOT_SENT;
    validation_rise_pen_command_is_sent = PEN_UP_COMMAND_NOT_SENT;
    manchester_code_command_count = 0;
    validation_data_sender_callbacks.sendSignalReadyToStart = &validateReadyToStartIsSent;
    validation_data_sender_callbacks.sendPlannedTrajectory = &validatePlannedTrajectoryIsSent;
    validation_data_sender_callbacks.sendRobotPoseEstimate = &validatePoseEstimateIsSent;
    validation_command_sender_callbacks.sendFetchManchesterCodeCommand = &validateFetchManchesterCodeIsSent;
    validation_command_sender_callbacks.sendLowerPenCommand = &validateLowerPenIsSent;
    validation_command_sender_callbacks.sendRisePenCommand = &validateRisePenIsSent;
    DataSender_changeTarget(robot->data_sender, validation_data_sender_callbacks);
    CommandSender_changeTarget(robot->command_sender, validation_command_sender_callbacks);
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

Test(Robot, given_aRobot_when_askedToSendAPlannedTrajectory_then_theSignalIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_sendPlannedTrajectory(robot);
    cr_assert(validation_planned_trajectory_is_sent);
}

Test(Robot, given_aRobot_when_askedToSendAPoseEstimate_then_theSignalIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_sendPoseEstimate(robot);
    cr_assert(validation_pose_estimate_is_sent);
}

void assertBehaviorIsAFreeEntrySendingPlannedTrajectory(struct Behavior *received)
{

    void (*sendPlannedTrajectory) = &Robot_sendPlannedTrajectory;

    while(received->action != sendPlannedTrajectory) {
        received = received->first_child;
    }

    struct Behavior *expected = BehaviorBuilder_build(
                                    BehaviorBuilder_withAction(sendPlannedTrajectory,
                                            BehaviorBuilder_withFreeEntry(
                                                    BehaviorBuilder_end())));

    cr_assert(Pose_haveTheSameValues(expected->entry_conditions->tolerances->pose,
                                     received->entry_conditions->tolerances->pose),);

    cr_assert(Flags_haveTheSameValues(expected->entry_conditions->goal_state->flags,
                                      received->entry_conditions->goal_state->flags));

    cr_assert_eq(expected->action, received->action);

    Behavior_delete(expected);
}

Test(Robot,
     given_aBehaviorWithPlanTowardsAntennaStartAction_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planTowardsAntennaStart;
    Behavior_act(robot->current_behavior, robot);
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(robot->current_behavior);
}

Test(Robot,
     given_aBehaviorWithPlanTowardsAntennaMiddleAction_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planTowardsAntennaMiddle;
    Behavior_act(robot->current_behavior, robot);
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(robot->current_behavior);
}

/*
Test(Robot,
     given_aBehaviorWithPlanTowardsAntennaStopAction_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planTowardsAntennaStop;
    Behavior_act(robot->current_behavior, robot);
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(robot->current_behavior);
}
*/

void assertBehaviorsAreAMovementChainFollowingThePlannedTrajectory(struct Behavior *behavior,
        struct CoordinatesSequence *trajectory)
{

    // Fetches the sendPlannedTrajectory behavior
    void (*sendPlannedTrajectory) = &Robot_sendPlannedTrajectory;

    while(behavior->action != sendPlannedTrajectory) {
        behavior = behavior->first_child;
    }

    behavior = behavior->first_child;

    void (*navigationAction)(struct Robot *) = &Navigator_navigateRobotTowardsGoal;

    struct Coordinates *point_in_trajectory;
    struct Coordinates *behavior_goal_coordinates;

    struct Flags *flags_planned_trajectory_received = Flags_new();
    Flags_setPlannedTrajectoryReceivedByStation(flags_planned_trajectory_received, 1);

    point_in_trajectory = trajectory->coordinates;
    behavior_goal_coordinates = behavior->entry_conditions->goal_state->pose->coordinates;

    cr_assert_eq(behavior->action, navigationAction);
    cr_assert(Flags_haveTheSameValues(behavior->entry_conditions->goal_state->flags, flags_planned_trajectory_received));
    cr_assert(Coordinates_haveTheSameValues(point_in_trajectory, behavior_goal_coordinates));
    assertBehaviorHasFreePoseEntry(behavior);

    do {
        trajectory = trajectory->next_element;
        behavior = behavior->first_child;
        point_in_trajectory = trajectory->coordinates;
        behavior_goal_coordinates = behavior->entry_conditions->goal_state->pose->coordinates;
        cr_assert(Coordinates_haveTheSameValues(point_in_trajectory, behavior_goal_coordinates));

        if(!CoordinatesSequence_isLast(trajectory)) {
            cr_assert_eq(behavior->action, navigationAction);
        }

        assertBehaviorHasFreeTrajectoryEntry(behavior);
    } while(!CoordinatesSequence_isLast(trajectory));

    Flags_delete(flags_planned_trajectory_received);
}

struct Behavior *fetchBeforeLastBehavior(struct Behavior *behavior)
{
    struct Behavior *current = behavior;

    while(current->first_child != current->first_child->first_child) {
        current = current->first_child;
    }

    return current;
}

struct Behavior *fetchLastBehavior(struct Behavior *behavior)
{
    struct Behavior *current = fetchBeforeLastBehavior(behavior);
    return current->first_child;
}

Test(Robot,
     given_aBehaviorWithPlanTowardsAntennaStartAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planTowardsAntennaStart;
    Behavior_act(robot->current_behavior, robot);
    assertBehaviorsAreAMovementChainFollowingThePlannedTrajectory(robot->current_behavior,
            robot->navigator->planned_trajectory);
}

Test(Robot,
     given_aBehaviorWithPlanTowardsAntennaStartAction_when_behaviorActs_then_theLastBehaviorsActionIsToPlanToOrientTowardsTheAntenna
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planTowardsAntennaStart;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    void (*planOrientationTowardsAntenna)(struct Robot *) = &Navigator_planOrientationTowardsAntenna;
    cr_assert_eq(last_behavior->action, planOrientationTowardsAntenna);
}

Test(Robot,
     given_aBehaviorWithPlanTowardsAntennaMiddleAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planTowardsAntennaMiddle;
    Behavior_act(robot->current_behavior, robot);
    assertBehaviorsAreAMovementChainFollowingThePlannedTrajectory(robot->current_behavior,
            robot->navigator->planned_trajectory);
}

Test(Robot,
     given_aBehaviorWithPlanTowardsAntennaStartAction_when_behaviorActs_then_theLastBehaviorsActionIsToPlanFetchingManchesterCode
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planTowardsAntennaMiddle;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    void (*planFetchingManchesterCode)(struct Robot *) = &Navigator_planFetchingManchesterCode;
    cr_assert_eq(last_behavior->action, planFetchingManchesterCode);
}

Test(Robot,
     given_aBehaviorWithPlanOrientationTowardsAntennaAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planOrientationTowardsAntenna;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *before_last_behavior = fetchBeforeLastBehavior(robot->current_behavior);
    assertBehaviorHasFreeEntry(before_last_behavior);
}

Test(Robot,
     given_aBehaviorWithPlanOrientationTowardsAntennaAction_when_behaviorActs_then_theBeforeLastBehaviorHasAnOrientRobotTowardsGoalAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planOrientationTowardsAntenna;
    Behavior_act(robot->current_behavior, robot);
    void (*orientationTowardsAntenna)(struct Robot *) = &Navigator_orientRobotTowardsGoal;
    struct Behavior *before_last_behavior = fetchBeforeLastBehavior(robot->current_behavior);
    cr_assert_eq(before_last_behavior->action, orientationTowardsAntenna);
}

Test(Robot,
     given_aBehaviorWithPlanOrientationTowardsAntennaAction_when_behaviorActs_then_theLastBehaviorHasAThetaZeroEntryGoal
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planOrientationTowardsAntenna;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasOrientationEntry(last_behavior, EAST);
}

Test(Robot,
     given_aBehaviorWithPlanOrientationTowardsAntennaAction_when_behaviorActs_then_theLastBehaviorHasAPlanTowardsAntennaMiddleAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planOrientationTowardsAntenna;
    Behavior_act(robot->current_behavior, robot);
    void (*planTowardsAntennaMiddle)(struct Robot *) = &Navigator_planTowardsAntennaMiddle;
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    cr_assert_eq(last_behavior->action, planTowardsAntennaMiddle);
}

Test(Robot,
     given_aBehaviorWithPlanFetchingManchesterCodeAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planFetchingManchesterCode;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *before_last_behavior = fetchBeforeLastBehavior(robot->current_behavior);
    assertBehaviorHasFreeEntry(before_last_behavior);
}

Test(Robot,
     given_aBehaviorWithPlanFetchingManchesterCodeAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFetchManchesterCodeIfAtLeastASecondHasPassedSingceLastRobotTimerResetAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planFetchingManchesterCode;
    Behavior_act(robot->current_behavior, robot);
    void (*fetchManchesterCode)(struct Robot *) =
        &Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset;
    struct Behavior *before_last_behavior = fetchBeforeLastBehavior(robot->current_behavior);
    cr_assert_eq(before_last_behavior->action, fetchManchesterCode);
}

void assertBehaviorHasManchesterCodeIsReceivedFlagEntry(struct Behavior *behavior)
{
    struct Flags *manchester_code_is_received_flags = Flags_irrelevant();
    Flags_setManchesterCodeReceived(manchester_code_is_received_flags, 1);
    assertBehaviorHasFreePoseEntry(behavior);
    struct Flags *behavior_entry_flags = behavior->entry_conditions->goal_state->flags;
    cr_assert(Flags_haveTheSameValues(manchester_code_is_received_flags, behavior_entry_flags));
    Flags_delete(manchester_code_is_received_flags);
}

Test(Robot,
     given_aBehaviorWithPlanFetchingManchesterCodeAction_when_behaviorActs_then_theLastBehaviorHasAManchesterCodeIsReceivedFlagsEntryGoal
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planFetchingManchesterCode;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasManchesterCodeIsReceivedFlagEntry(last_behavior);
}

Test(Robot,
     given_aBehaviorWithPlanFetchingManchesterCodeAction_when_behaviorActs_then_theLastBehaviorHasAPlanLowerPenForAntennaMarkAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planFetchingManchesterCode;
    Behavior_act(robot->current_behavior, robot);
    void (*planLowerPenForAntennaMark)(struct Robot *) = &Navigator_planLowerPenForAntennaMark;
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    cr_assert_eq(last_behavior->action, planLowerPenForAntennaMark);
}

/*
Test(Robot,
     given_aBehaviorWithPlanTowardsAntennaStopAction_when_behaviorActs_then_theLastBehaviorOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    robot->current_behavior->action = &Navigator_planTowardsAntennaStop;
    Behavior_act(robot->current_behavior, robot);
    assertBehaviorsAreAMovementChainFollowingThePlannedTrajectory(robot->current_behavior,
            robot->navigator->planned_trajectory);
}
*/

Test(Robot,
     given_theRobot_when_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset_then_theCommandIsSend
     , .init = setup_robot
     , .fini = teardown_robot)
{
    while(!Timer_hasTimePassed(robot->timer, ONE_SECOND));

    Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset(robot);

    cr_assert_eq(manchester_code_command_count, 1);
}

Test(Robot,
     given_theRobot_when_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerResetTwiceUnderASecond_then_theCommandIsSendOnlyOnce
     , .init = setup_robot
     , .fini = teardown_robot)
{
    while(!Timer_hasTimePassed(robot->timer, ONE_SECOND));

    Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset(robot);
    Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset(robot);

    cr_assert_eq(manchester_code_command_count, 1);
}

Test(Robot,
     given_theRobot_when_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerResetTwiceOverASecond_then_theCommandIsSendTwice
     , .init = setup_robot
     , .fini = teardown_robot)
{
    while(!Timer_hasTimePassed(robot->timer, ONE_SECOND));

    double current_time = Timer_elapsedTime(robot->timer);

    Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset(robot);

    while(!Timer_hasTimePassed(robot->timer, ONE_SECOND));

    Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset(robot);

    cr_assert_eq(manchester_code_command_count, 2);
}

Test(Robot, given_theRobot_when_askForLowerPenAndWaitASecondAndAnHalf_then_theCommandIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_lowerPenAndWaitASecondAndAnHalf(robot);

    cr_assert_eq(validation_lower_pen_command_is_sent, PEN_DOWN_COMMAND_SENT);
}

Test(Robot,
     given_theRobot_when_askForLowerPenAndWaitASecondAndAnHalf_then_atLeastASecondAndAnHalfHasPassedSinceTheActionWasTriggered
     , .init = setup_robot
     , .fini = teardown_robot)
{
    struct Timer *timer = Timer_new();
    Robot_lowerPenAndWaitASecondAndAnHalf(robot);

    cr_assert(Timer_hasTimePassed(timer, ONE_SECOND_AND_AN_HALF));

    Timer_delete(timer);
}

Test(Robot, given_theRobot_when_askForRisePenAndWaitASecondAnHalfAnd_then_theCommandIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_risePenAndWaitASecondAndAnHalf(robot);

    cr_assert_eq(validation_rise_pen_command_is_sent, PEN_UP_COMMAND_SENT);
}

Test(Robot,
     given_theRobot_when_askForRisePenAndWaitASecondAndAnHalf_then_atLeastASecondAndAnHalfHasPassedSinceTheActionWasTriggered
     , .init = setup_robot
     , .fini = teardown_robot)
{
    struct Timer *timer = Timer_new();
    Robot_risePenAndWaitASecondAndAnHalf(robot);

    cr_assert(Timer_hasTimePassed(timer, ONE_SECOND_AND_AN_HALF));

    Timer_delete(timer);
}

Test(Robot, given_initialRobot_when_takesAPicture_then_thePicureTakenFlagValueIsOne
     , .init = setup_robot
     , .fini = teardown_robot)
{
    // TODO: take the actual picture
    Robot_takePicture(robot);
    cr_assert(robot->current_state->flags->picture_taken);
}

