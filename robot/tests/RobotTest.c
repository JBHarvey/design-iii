#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"
#include "Timer.h"

struct Robot *robot;
struct DataSender_Callbacks validation_data_sender_callbacks;
struct CommandSender_Callbacks validation_command_sender_callbacks;
int validation_ready_to_start_is_sent;
int validation_ready_to_draw_is_sent;
int validation_end_of_cycle_is_sent;
int validation_planned_trajectory_is_sent;
int validation_pose_estimate_is_sent;
int validation_light_green_led_command_is_sent;
int validation_light_red_led_command_is_sent;
int validation_lower_pen_command_is_sent;
int validation_rise_pen_command_is_sent;
int validation_send_speeds_command_is_sent;
const int SENT = 1;
const int NOT_SENT = 0;
const int PEN_DOWN_COMMAND_SENT = 1;
const int PEN_DOWN_COMMAND_NOT_SENT = 0;
const int PEN_UP_COMMAND_SENT = 1;
const int PEN_UP_COMMAND_NOT_SENT = 0;
const int NON_ZERO_X_VALUE = 10;
const int NON_ZERO_Y_VALUE = 15;
const int NON_ZERO_X_SPEED_VALUE = 10000;
const int NON_ZERO_Y_SPEED_VALUE = 15000;
int NON_ZERO_THETA_VALUE = 21000;
int manchester_code_command_count;
int ready_to_draw_signal_count;

void giveADummyDrawingTrajectoryToTheRobot(struct Robot *robot)
{
    struct Coordinates *zero = Coordinates_zero();
    struct CoordinatesSequence *dummy_trajectory = CoordinatesSequence_new(zero);
    robot->drawing_trajectory = dummy_trajectory;
    Coordinates_delete(zero);
}
void assertBehaviorHasFreeFlagsEntry(struct Behavior *behavior)
{
    struct Flags *irrelevant_flags = Flags_irrelevant();
    cr_assert(Flags_haveTheSameValues(irrelevant_flags, behavior->entry_conditions->goal_state->flags));
    Flags_delete(irrelevant_flags);
}

void assertBehaviorHasAngleEntry(struct Behavior *behavior, int angle)
{
    cr_assert_eq(angle, behavior->entry_conditions->goal_state->pose->angle->theta);
    assertBehaviorHasFreeFlagsEntry(behavior);
}

void assertBehaviorHasOrientationEntry(struct Behavior *behavior, enum CardinalDirection orientation)
{
    struct Pose *angle_tolerance = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_ORIENTATION);
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

    assertBehaviorHasAngleEntry(behavior, angle);
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
    struct Pose *free_coordinates_tolerance = Pose_new(X_TOLERANCE_MOVING, Y_TOLERANCE_MOVING, THETA_TOLERANCE_MAX);
    cr_assert(Pose_haveTheSameValues(free_coordinates_tolerance, behavior->entry_conditions->tolerances->pose));
    Pose_delete(free_coordinates_tolerance);
    assertBehaviorHasFreeFlagsEntry(behavior);
}

void assertBehaviorHasFreeTrajectoryEntryAndDrawingTolerances(struct Behavior *behavior)
{
    struct Pose *free_coordinates_tolerance = Pose_new(X_TOLERANCE_DRAWING, Y_TOLERANCE_DRAWING, THETA_TOLERANCE_MAX);
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

    while(!Timer_hasTimePassed(robot->timer, THREE_SECONDS));

    while(!Timer_hasTimePassed(robot->timer, THREE_SECONDS));

    struct Flags *third_flags = third_behavior->entry_conditions->goal_state->flags;
    struct Flags *ready_to_start_received_flags = Flags_new();
    Flags_setReadyToStartReceivedByStation(ready_to_start_received_flags, 1);

    cr_assert_eq(third_behavior->action, idle_action);
    cr_assert(Flags_haveTheSameValues(third_flags, ready_to_start_received_flags));
    assertBehaviorHasFreePoseEntry(third_behavior);


    struct Behavior *fourth_behavior = third_behavior->first_child;
    void (*plan_navigation_to_antenna_start)(struct Robot *) = &Navigator_planTowardsAntennaMiddle;
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
    validation_ready_to_start_is_sent = SENT;
}

void validateReadyToDrawIsSent(void)
{
    validation_ready_to_draw_is_sent = SENT;
    ++ready_to_draw_signal_count;
}

void validateEndOfCycleIsSent(void)
{
    validation_end_of_cycle_is_sent = SENT;
}

void validatePlannedTrajectoryIsSent(struct CoordinatesSequence *sequence)
{
    validation_planned_trajectory_is_sent = SENT;
}

void validatePoseEstimateIsSent(struct Pose *pose)
{
    validation_pose_estimate_is_sent = SENT;
}

void validateLightGreenLedIsSent(void)
{
    validation_light_green_led_command_is_sent = SENT;
}

void validateLightRedLedIsSent(void)
{
    validation_light_red_led_command_is_sent = SENT;
}

void validateLowerPenIsSent(void)
{
    validation_lower_pen_command_is_sent = SENT;
}

void validateRisePenIsSent(void)
{
    validation_rise_pen_command_is_sent = SENT;
}

void validateFetchManchesterCodeIsSent(void)
{
    manchester_code_command_count++;
}

void validateSendSpeedsCommand(struct Command_Speeds speeds)
{
    validation_send_speeds_command_is_sent = SENT;
}

void setup_robot(void)
{
    robot = Robot_new();
    validation_ready_to_start_is_sent = NOT_SENT;
    validation_ready_to_draw_is_sent = NOT_SENT;
    validation_end_of_cycle_is_sent = NOT_SENT;
    validation_planned_trajectory_is_sent = NOT_SENT;
    validation_pose_estimate_is_sent = NOT_SENT;
    validation_light_green_led_command_is_sent = NOT_SENT;
    validation_light_red_led_command_is_sent = NOT_SENT;
    validation_lower_pen_command_is_sent = NOT_SENT;
    validation_rise_pen_command_is_sent = NOT_SENT;
    validation_send_speeds_command_is_sent = NOT_SENT;
    manchester_code_command_count = 0;
    validation_data_sender_callbacks.sendSignalReadyToStart = &validateReadyToStartIsSent;
    validation_data_sender_callbacks.sendSignalReadyToDraw = &validateReadyToDrawIsSent;
    validation_data_sender_callbacks.sendPlannedTrajectory = &validatePlannedTrajectoryIsSent;
    validation_data_sender_callbacks.sendRobotPoseEstimate = &validatePoseEstimateIsSent;
    validation_data_sender_callbacks.sendSignalEndOfCycle = &validateEndOfCycleIsSent;
    validation_command_sender_callbacks.sendFetchManchesterCodeCommand = &validateFetchManchesterCodeIsSent;
    validation_command_sender_callbacks.sendLightGreenLEDCommand = &validateLightGreenLedIsSent;
    validation_command_sender_callbacks.sendLightRedLEDCommand = &validateLightRedLedIsSent;
    validation_command_sender_callbacks.sendLowerPenCommand = &validateLowerPenIsSent;
    validation_command_sender_callbacks.sendRisePenCommand = &validateRisePenIsSent;
    validation_command_sender_callbacks.sendSpeedsCommand = &validateSendSpeedsCommand,
                                        DataSender_changeTarget(robot->data_sender, validation_data_sender_callbacks);
    CommandSender_changeTarget(robot->command_sender, validation_command_sender_callbacks);

    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
}

void teardown_robot(void)
{
    Robot_delete(robot);
}

Test(Robot, given_aRobot_when_askedToResetAllActuators_then_hasPrepareCommandStatusOfRotationActuatorIsSetToZero
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Actuator_preparesCommand(robot->wheels->rotation_actuator);
    Robot_resetAllActuators(robot);

    cr_assert_eq(robot->wheels->rotation_actuator->has_prepared_new_command, 0);
}

Test(Robot, given_aRobot_when_askedToResetAllActuators_then_hasPrepareCommandStatusOfTranslationActuatorIsSetToZero
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Actuator_preparesCommand(robot->wheels->translation_actuator);
    Robot_resetAllActuators(robot);

    cr_assert_eq(robot->wheels->translation_actuator->has_prepared_new_command, 0);
}

Test(Robot, given_aRobot_when_askedToResetAllActuators_then_hasPrepareCommandStatusOfSpeedActuatorIsSetToZero
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Actuator_preparesCommand(robot->wheels->speed_actuator);
    Robot_resetAllActuators(robot);

    cr_assert_eq(robot->wheels->speed_actuator->has_prepared_new_command, 0);
}

Test(Robot, given_aRobot_when_askedToSendAReadyToStartSignal_then_theSignalIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_sendReadyToStartSignal(robot);
    cr_assert(validation_ready_to_start_is_sent);
}

Test(Robot, given_aRobot_when_askedToSendAReadyToStartSignal_then_theDrawingFlagIsSetToZero
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_sendReadyToStartSignal(robot);
    cr_assert_eq(robot->current_state->flags->drawing, 0);
}

Test(Robot, given_aRobot_when_askedToSendAReadyToDrawSignal_then_theSignalIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    while(!Timer_hasTimePassed(robot->timer, THREE_SECONDS));

    Robot_sendReadyToDrawSignal(robot);
    cr_assert(validation_ready_to_draw_is_sent);
}

Test(Robot, given_aRobot_when_askedToSendAReadyToDrawSignal_then_theDrawingFlagIsSetToOne
     , .init = setup_robot
     , .fini = teardown_robot)
{
    while(!Timer_hasTimePassed(robot->timer, THREE_SECONDS));

    Robot_sendReadyToDrawSignal(robot);
    cr_assert_eq(robot->current_state->flags->drawing, 1);
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

void assertBehaviorIsAFreeEntrySendingPlannedTrajectory(void (*action)(struct Robot *))
{
    struct Behavior *behavior = robot->current_behavior;
    behavior->action = action;
    Behavior_act(behavior, robot);

    void (*sendPlannedTrajectory) = &Robot_sendPlannedTrajectory;

    while(behavior->action != sendPlannedTrajectory) {
        behavior = behavior->first_child;
    }

    assertBehaviorHasFreeEntry(behavior);

    cr_assert_eq(sendPlannedTrajectory, behavior->action);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsAntennaMiddleAction_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(&Navigator_planTowardsAntennaMiddle);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsAntennaMarkEndAction_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(&Navigator_planTowardsAntennaMarkEnd);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsObstacleZoneEastSide_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(&Navigator_planTowardsObstacleZoneEastSide);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsPainting_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(&Navigator_planTowardsPainting);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsObstacleZoneWestSide_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(&Navigator_planTowardsObstacleZoneWestSide);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsCenterOfDrawingZone_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(&Navigator_planTowardsCenterOfDrawingZone);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsDrawingStart_when_behaviorActs_then_theBehaviorsFirstChildHasFreeEntryAndSendsThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    giveADummyDrawingTrajectoryToTheRobot(robot);
    assertBehaviorIsAFreeEntrySendingPlannedTrajectory(&Navigator_planTowardsDrawingStart);
}

void assertBehaviorIsAFreeEntryNavigation(struct Behavior *behavior)
{
    assertBehaviorHasFreeEntry(behavior);
    void (*action)(struct Robot *) = &Navigator_navigateRobotTowardsGoal;
    cr_assert_eq(behavior->action, action);
}

struct Behavior *findBehaviorSendingPlannedTrajectoryAfterAction(void (*action)(struct Robot *))
{
    struct Behavior *behavior = robot->current_behavior;
    behavior->action = action;
    Behavior_act(behavior, robot);

    void (*sendPlannedTrajectory) = &Robot_sendPlannedTrajectory;

    while(behavior->action != sendPlannedTrajectory) {
        behavior = behavior->first_child;
    }

    return behavior;
}

void assertBehaviorHasPlannedTrajectoryReceivedFlagsAndFreePoseEntry(struct Behavior *behavior)
{
    struct Flags *flags_planned_trajectory_received = Flags_irrelevant();
    Flags_setPlannedTrajectoryReceivedByStation(flags_planned_trajectory_received, 1);
    cr_assert(Flags_haveTheSameValues(behavior->entry_conditions->goal_state->flags, flags_planned_trajectory_received));
    Flags_delete(flags_planned_trajectory_received);

    assertBehaviorHasFreePoseEntry(behavior);
}

void assertBehaviorsAreANavigationAndMovementChainFollowingThePlannedTrajectoryAfterAction(void (*action)(
            struct Robot *), void (*assertTrajectoryEntryIsCorrect)(struct Behavior *behavior))
{

    struct Behavior *behavior = findBehaviorSendingPlannedTrajectoryAfterAction(action);
    behavior = behavior->first_child;

    void (*computeTrajectoryStartAngle)(struct Robot *) = &Navigator_computeTrajectoryStartAngle;
    cr_assert_eq(behavior->action, computeTrajectoryStartAngle);

    assertBehaviorHasPlannedTrajectoryReceivedFlagsAndFreePoseEntry(behavior);

    behavior = behavior->first_child;

    assertBehaviorIsAFreeEntryNavigation(behavior);

    struct CoordinatesSequence *trajectory = robot->navigator->planned_trajectory;
    struct Coordinates *point_in_trajectory;
    struct Coordinates *behavior_goal_coordinates;

    do {
        trajectory = trajectory->next_element;
        behavior = behavior->first_child;

        point_in_trajectory = trajectory->coordinates;
        behavior_goal_coordinates = behavior->entry_conditions->goal_state->pose->coordinates;
        cr_assert_eq(behavior->action, computeTrajectoryStartAngle);
        cr_assert(Coordinates_haveTheSameValues(point_in_trajectory, behavior_goal_coordinates));
        assertTrajectoryEntryIsCorrect(behavior);

        behavior = behavior->first_child;

        if(!CoordinatesSequence_isLast(trajectory)) {
            assertBehaviorIsAFreeEntryNavigation(behavior);
        }


    } while(!CoordinatesSequence_isLast(trajectory));

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
void assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(void (*firstAction)(struct Robot *),
        void (*expectedAction)(struct Robot *))
{
    struct Behavior *current_behavior = robot->current_behavior;
    current_behavior->action = firstAction;
    Behavior_act(current_behavior, robot);
    struct Behavior *before_last_behavior = fetchBeforeLastBehavior(current_behavior);
    cr_assert_eq(before_last_behavior->action, expectedAction);
}

void assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(void (*firstAction)(struct Robot *),
        void (*expectedAction)(struct Robot *))
{
    struct Behavior *current_behavior = robot->current_behavior;
    current_behavior->action = firstAction;
    Behavior_act(current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(current_behavior);
    cr_assert_eq(last_behavior->action, expectedAction);
}

void assertBeforeLastBehaviorHasFreeEntryAfterAction(void (*action)(struct Robot *))
{
    robot->current_behavior->action = action;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *before_last_behavior = fetchBeforeLastBehavior(robot->current_behavior);
    assertBehaviorHasFreeEntry(before_last_behavior);
}

void assertLastBehaviorHasFreeEntryAfterAction(void (*action)(struct Robot *))
{
    robot->current_behavior->action = action;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasFreeEntry(last_behavior);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanOrientationTowardsAntennaAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planOrientationTowardsAntenna);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanOrientationTowardsAntennaAction_when_behaviorActs_then_theBeforeLastBehaviorHasAnOrientRobotTowardsGoalAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planOrientationTowardsAntenna,
            &Navigator_orientRobotTowardsGoal);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanOrientationTowardsAntennaAction_when_behaviorActs_then_theLastBehaviorHasAThetaZeroEntryGoal
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planOrientationTowardsAntenna;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasOrientationEntry(last_behavior, EAST);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanOrientationTowardsAntennaAction_when_behaviorActs_then_theLastBehaviorHasAStopMotionBeforeFetchingManchesterAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planOrientationTowardsAntenna,
            &Navigator_planStopMotionBeforeFetchingManchester);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsAntennaMiddleAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorsAreANavigationAndMovementChainFollowingThePlannedTrajectoryAfterAction(
        &Navigator_planTowardsAntennaMiddle, &assertBehaviorHasFreeTrajectoryEntry);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsAntennaMiddleAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotHasTheMiddleOfTheAntennaZoneCoordinatesAsEntryConditions
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planTowardsObstacleZoneEastSide;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    struct Coordinates *start = robot->navigator->navigable_map->antenna_zone_start;
    struct Coordinates *stop = robot->navigator->navigable_map->antenna_zone_stop;
    int center_x = Coordinates_computeMeanX(start, stop);
    int center_y = Coordinates_computeMeanY(start, stop);
    struct Coordinates *expected_coordinates = Coordinates_new(center_x, center_y);
    struct Coordinates *goal_coordinates = last_behavior->entry_conditions->goal_state->pose->coordinates;
    cr_assert(Coordinates_haveTheSameValues(expected_coordinates, goal_coordinates));
    Coordinates_delete(expected_coordinates);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsAntennaMiddleAction_when_behaviorActs_then_theLastBehaviorsActionIsToPlanOrientationTowardsAntenna
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTowardsAntennaMiddle,
            &Navigator_planOrientationTowardsAntenna);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionBeforeFetchingManchesterAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planStopMotionBeforeFetchingManchester);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionBeforeFetchingManchesterAction_when_behaviorActs_then_theBeforeLastBehaviorHasAStopMovementAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planStopMotionBeforeFetchingManchester,
            &Navigator_stopMovement);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionBeforeFetchingManchesterAction_when_behaviorActs_then_theLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorHasFreeEntryAfterAction(&Navigator_planStopMotionBeforeFetchingManchester);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionBeforeFetchingManchesterAction_when_behaviorActs_then_theLastBehaviorHasAPlanFetchingManchesterAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planStopMotionBeforeFetchingManchester,
            &Navigator_planFetchingManchesterCode);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanFetchingManchesterCodeAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planFetchingManchesterCode);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanFetchingManchesterCodeAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFetchManchesterCodeIfAtLeastASecondHasPassedSingeLastRobotTimerResetAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planFetchingManchesterCode,
            &Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset);
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

Test(RobotBehaviors,
     given_aBehaviorWithPlanFetchingManchesterCodeAction_when_behaviorActs_then_theLastBehaviorHasAManchesterCodeIsReceivedFlagsEntryGoal
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planFetchingManchesterCode;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasManchesterCodeIsReceivedFlagEntry(last_behavior);
}

/**/
Test(RobotBehaviors,
     given_aBehaviorWithPlanFetchingManchesterCodeActionAndNoCycleCompleted_when_behaviorActs_then_theLastBehaviorHasAPlanLowerPenForAntennaMarkAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Flags_setHasCompletedACycle(robot->current_state->flags, FALSE);
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planFetchingManchesterCode,
            &Navigator_planLowerPenForAntennaMark);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanFetchingManchesterCodeActionAndACycleHasBeenCompleted_when_behaviorActs_then_theLastBehaviorHasAPlanTowardsObstacleZoneEastSideAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Flags_setHasCompletedACycle(robot->current_state->flags, TRUE);
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planFetchingManchesterCode,
            &Navigator_planTowardsObstacleZoneEastSide);
}
/**/

Test(RobotBehaviors,
     given_aBehaviorWithPlanLoweringPenForAntennaMarkAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planLowerPenForAntennaMark);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLoweringPenForAntennaMarkAction_when_behaviorActs_then_theBeforeLastBehaviorHasALowerPenAndWaitAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planLowerPenForAntennaMark,
            &Robot_stopWaitTwoSecondsLowerPenWaitTwoSecond);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLoweringPenForAntennaMarkAction_when_behaviorActs_then_theLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorHasFreeEntryAfterAction(&Navigator_planLowerPenForAntennaMark);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLoweringPenForAntennaMarkAction_when_behaviorActs_then_theLastBehaviorHasAPlanTowardsAntennaMarkEndAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planLowerPenForAntennaMark,
            &Navigator_planTowardsAntennaMarkEnd);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsAntennaMarkEndAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorsAreANavigationAndMovementChainFollowingThePlannedTrajectoryAfterAction(
        &Navigator_planTowardsAntennaMarkEnd, &assertBehaviorHasFreeTrajectoryEntryAndDrawingTolerances);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsAntennaMarkEndAction_when_behaviorActs_then_theLastBehaviorOfTheRobotHasItsCurrentCoordinatesMinusTheMarkDistanceInYAsEntryConditions
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planTowardsAntennaMarkEnd;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    int x = robot->current_state->pose->coordinates->x;
    int y = robot->current_state->pose->coordinates->y - ANTENNA_MARK_DISTANCE;
    struct Coordinates *expected_coordinates = Coordinates_new(x, y);
    struct Coordinates *goal_coordinates = last_behavior->entry_conditions->goal_state->pose->coordinates;
    cr_assert(expected_coordinates->x == goal_coordinates->x);
    cr_assert(abs(expected_coordinates->y == goal_coordinates->y) <= 100);
    Coordinates_delete(expected_coordinates);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsAntennaMarkEndAction_when_behaviorActs_then_theLastBehaviorsActionIsToPlanRisingThePenForObstacleCrossing
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTowardsAntennaMarkEnd,
            &Navigator_planRisePenForObstacleCrossing);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanRisePenForObstacleCrossingAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planRisePenForObstacleCrossing);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanRisePenForObstacleCrossingAction_when_behaviorActs_then_theBeforeLastBehaviorHasARisePenAndWaitAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planRisePenForObstacleCrossing,
            &Robot_stopWaitTwoSecondsRisePenWaitTwoSecond);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanRisePenForObstacleCrossingAction_when_behaviorActs_then_theLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorHasFreeEntryAfterAction(&Navigator_planRisePenForObstacleCrossing);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanRisePenForObstacleCrossingAction_when_behaviorActs_then_theLastBehaviorHasAPlanTowardsObstacleZoneEastSide
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planRisePenForObstacleCrossing,
            &Navigator_planTowardsObstacleZoneEastSide);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsObstacleZoneEastSideAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorsAreANavigationAndMovementChainFollowingThePlannedTrajectoryAfterAction(
        &Navigator_planTowardsObstacleZoneEastSide, &assertBehaviorHasFreeTrajectoryEntry);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsObstacleZoneEastSideAction_when_behaviorActs_then_theLastBehaviorOfTheRobotHasTheMapsGraphEasternNodesCoordinatesAsEntryConditions
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planTowardsObstacleZoneEastSide;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    struct Coordinates *expected_coordinates = robot->navigator->graph->eastern_node->coordinates;
    struct Coordinates *goal_coordinates = last_behavior->entry_conditions->goal_state->pose->coordinates;
    cr_assert(Coordinates_haveTheSameValues(expected_coordinates, goal_coordinates));
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsObstacleZoneEastSideAction_when_behaviorActs_then_theLastBehaviorsActionIsToPlanTowardsPaintingZone
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTowardsObstacleZoneEastSide,
            &Navigator_planTowardsPaintingZone);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsPaintingAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorsAreANavigationAndMovementChainFollowingThePlannedTrajectoryAfterAction(&Navigator_planTowardsPainting,
            &assertBehaviorHasFreeTrajectoryEntry);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsPaintingAction_when_behaviorActs_then_theLastBehaviorOfTheRobotHasThePaintingCoordinatesCorrespondingToThePaintingNumberInTheRobotsManchesterCodeAsEntryConditions
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planTowardsPainting;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    int painting_number = robot->manchester_code->painting_number;
    struct Coordinates *expected_coordinates =
            robot->navigator->navigable_map->painting_zones[painting_number]->coordinates;
    struct Coordinates *goal_coordinates = last_behavior->entry_conditions->goal_state->pose->coordinates;
    cr_assert(Coordinates_haveTheSameValues(expected_coordinates, goal_coordinates));
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsPaintingAction_when_behaviorActs_then_theLastBehaviorsActionIsToPlanOrientationTowardsPainting
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTowardsPainting,
            &Navigator_planOrientationTowardsPainting);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanOrientationTowardsPaintingAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planOrientationTowardsPainting);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanOrientationTowardsPaintingAction_when_behaviorActs_then_theBeforeLastBehaviorHasAnOrientRobotTowardsGoalAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planOrientationTowardsPainting,
            &Navigator_orientRobotTowardsGoal);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanOrientationTowardsPaintingAction_when_behaviorActs_then_theLastBehaviorHasAThetaEquivalentToTheCorrectPaintingOrientationEntryGoal
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planOrientationTowardsPainting;
    Behavior_act(robot->current_behavior, robot);
    int painting_number = robot->manchester_code->painting_number;
    int angle = robot->navigator->navigable_map->painting_zones[painting_number]->angle->theta;
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasAngleEntry(last_behavior, angle);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanOrientationTowardsPaintingAction_when_behaviorActs_then_theLastBehaviorHasAPlanStopMotionAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planOrientationTowardsPainting,
            &Navigator_planStopMotionBeforePicture);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionBeforePictureAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planStopMotionBeforePicture);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionBeforePictureAction_when_behaviorActs_then_theBeforeLastBehaviorHasAStopMovementAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planStopMotionBeforePicture,
            &Navigator_stopMovement);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionBeforePictureAction_when_behaviorActs_then_theLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorHasFreeEntryAfterAction(&Navigator_planStopMotionBeforePicture);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionBeforePictureAction_when_behaviorActs_then_theLastBehaviorHasAPlanLightingGreenLedAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planStopMotionBeforePicture,
            &Navigator_planLightingGreenLedBeforePicture);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLightingGreenLedBeforePictureAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planLightingGreenLedBeforePicture);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLightingGreenLedBeforePictureAction_when_behaviorActs_then_theBeforeLastBehaviorHasALightGreenLedAndWaitAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planLightingGreenLedBeforePicture,
            &Robot_lightGreenLedAndWaitASecond);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLightingGreenLedBeforePictureAction_when_behaviorActs_then_theLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorHasFreeEntryAfterAction(&Navigator_planLightingGreenLedBeforePicture);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLightingGreenLedBeforePictureAction_when_behaviorActs_then_theLastBehaviorHasAPlanTakingPictureAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planLightingGreenLedBeforePicture,
            &Navigator_planTakingPicture);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTakingPictureAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planTakingPicture);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTakingPictureAction_when_behaviorActs_then_theBeforeLastBehaviorHasAnTakePictureAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTakingPicture,
            &OnboardCamera_takePictureAndIfValidSendAndUpdateDrawingBaseTrajectory);
}

void assertBehaviorHasImageReceivedByStationFlagEntry(struct Behavior *behavior)
{
    struct Flags *image_received_by_station_flags = Flags_irrelevant();
    Flags_setImageReceivedByStation(image_received_by_station_flags, 1);
    assertBehaviorHasFreePoseEntry(behavior);
    struct Flags *behavior_entry_flags = behavior->entry_conditions->goal_state->flags;
    cr_assert(Flags_haveTheSameValues(image_received_by_station_flags, behavior_entry_flags));
    Flags_delete(image_received_by_station_flags);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTakingPictureAction_when_behaviorActs_then_theLastBehaviorHasAImageReceivedByStationFlagsWithFreePoseEntryGoal
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planTakingPicture;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasImageReceivedByStationFlagEntry(last_behavior);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTakingPictureAction_when_behaviorActs_then_theLastBehaviorHasAPlanTowardsObstacleZoneWestSide
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTakingPicture,
            &Navigator_planTowardsObstacleZoneWestSide);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsObstacleZoneWestSideAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorsAreANavigationAndMovementChainFollowingThePlannedTrajectoryAfterAction(
        &Navigator_planTowardsObstacleZoneWestSide, &assertBehaviorHasFreeTrajectoryEntry);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsObstacleZoneWestSideAction_when_behaviorActs_then_theLastBehaviorOfTheRobotHasHasTheMapsGraphWesternNodesCoordinatesAsEntryConditions
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planTowardsObstacleZoneWestSide;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    struct Coordinates *expected_coordinates = robot->navigator->graph->western_node->coordinates;
    struct Coordinates *goal_coordinates = last_behavior->entry_conditions->goal_state->pose->coordinates;
    cr_assert(Coordinates_haveTheSameValues(expected_coordinates, goal_coordinates));
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsObstacleZoneWestSideAction_when_behaviorActs_then_theLastBehaviorsActionIsToPlanTowardsDrawingZoneAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTowardsObstacleZoneWestSide,
            &Navigator_planTowardsDrawingZone);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsCenterOfDrawingZoneAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorsAreANavigationAndMovementChainFollowingThePlannedTrajectoryAfterAction(
        &Navigator_planTowardsCenterOfDrawingZone, &assertBehaviorHasFreeTrajectoryEntryAndDrawingTolerances);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsCenterOfDrawingZoneAction_when_behaviorActs_then_theLastBehaviorOfTheRobotHasTheCenterOfTheDrawingZoneAsEntryConditions
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planTowardsCenterOfDrawingZone;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    struct Coordinates *south_west_corner = robot->navigator->navigable_map->south_western_drawing_corner;
    struct Coordinates *north_east_corner = robot->navigator->navigable_map->north_eastern_drawing_corner;
    int center_x = Coordinates_computeMeanX(south_west_corner, north_east_corner);
    int center_y = Coordinates_computeMeanY(south_west_corner, north_east_corner);
    struct Coordinates *expected_coordinates = Coordinates_new(center_x, center_y);
    struct Coordinates *goal_coordinates = last_behavior->entry_conditions->goal_state->pose->coordinates;
    cr_assert(Coordinates_haveTheSameValues(expected_coordinates, goal_coordinates));
    Coordinates_delete(expected_coordinates);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsCenterOfDrawingZoneAction_when_behaviorActs_then_theLastBehaviorsActionIsToPlanToTellReadyToDraw
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTowardsCenterOfDrawingZone,
            &Navigator_planToTellReadyToDraw);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanToTellReadyToDrawAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planToTellReadyToDraw);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanToTellReadyToDrawAction_when_behaviorActs_then_theBeforeLastBehaviorHasASendReadyToDrawSignalAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planToTellReadyToDraw,
            &Robot_sendReadyToDrawSignal);
}

void assertBehaviorHasReadyToDrawReceivedByStationEntry(struct Behavior *behavior)
{
    struct Flags *ready_to_draw_received_by_station = Flags_irrelevant();
    Flags_setReadyToStartReceivedByStation(ready_to_draw_received_by_station, 1);
    assertBehaviorHasFreePoseEntry(behavior);
    struct Flags *behavior_entry_flags = behavior->entry_conditions->goal_state->flags;
    cr_assert(Flags_haveTheSameValues(ready_to_draw_received_by_station, behavior_entry_flags));
    Flags_delete(ready_to_draw_received_by_station);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanToTellReadyToDrawAction_when_behaviorActs_then_theLastBehaviorHasAReadyToDrawReceivedByStationFlagsWithFreePoseEntryGoal
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planToTellReadyToDraw;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasReadyToDrawReceivedByStationEntry(last_behavior);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanToTellReadyToDrawAction_when_behaviorActs_then_theLastBehaviorHasAPlanTowardsDrawingStartAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planToTellReadyToDraw,
            &Navigator_planTowardsDrawingStart);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsDrawingStartAction_when_behaviorActs_then_theLastBehaviorsOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    giveADummyDrawingTrajectoryToTheRobot(robot);
    assertBehaviorsAreANavigationAndMovementChainFollowingThePlannedTrajectoryAfterAction(
        &Navigator_planTowardsDrawingStart, &assertBehaviorHasFreeTrajectoryEntryAndDrawingTolerances);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsDrawingStartAction_when_behaviorActs_then_theLastBehaviorOfTheRobotHasTheCenterOfTheDrawingZoneAsEntryConditions
     , .init = setup_robot
     , .fini = teardown_robot)
{
    giveADummyDrawingTrajectoryToTheRobot(robot);
    robot->current_behavior->action = &Navigator_planTowardsDrawingStart;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    struct Coordinates *expected_coordinates = robot->drawing_trajectory->coordinates;
    struct Coordinates *goal_coordinates = last_behavior->entry_conditions->goal_state->pose->coordinates;
    cr_assert(Coordinates_haveTheSameValues(expected_coordinates, goal_coordinates));
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsDrawingStartAction_when_behaviorActs_then_theLastBehaviorsActionIsToPlanLowerPenBeforeDrawing
     , .init = setup_robot
     , .fini = teardown_robot)
{
    giveADummyDrawingTrajectoryToTheRobot(robot);
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTowardsDrawingStart,
            &Navigator_planLowerPenBeforeDrawing);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLoweringPenBeforeDrawingAntennaMarkAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planLowerPenBeforeDrawing);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLoweringPenBeforeDrawingAntennaMarkAction_when_behaviorActs_then_theBeforeLastBehaviorHasALowerPenAndWaitAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planLowerPenBeforeDrawing,
            &Robot_stopWaitTwoSecondsLowerPenWaitTwoSecond);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLoweringPenBeforeDrawingAntennaMarkAction_when_behaviorActs_then_theLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorHasFreeEntryAfterAction(&Navigator_planLowerPenBeforeDrawing);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLoweringPenBeforeDrawingAntennaMarkAction_when_behaviorActs_then_theLastBehaviorHasAPlanDrawingAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planLowerPenBeforeDrawing, &Navigator_planDrawing);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanRisePenBeforeGoingToSafeZoneAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planRisePenBeforeGoingToSafeZone);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanRisePenBeforeGoingToSafeZoneAction_when_behaviorActs_then_theBeforeLastBehaviorHasARisePenAndWaitAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planRisePenBeforeGoingToSafeZone,
            &Robot_stopWaitTwoSecondsRisePenWaitTwoSecond);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanRisePenBeforeGoingToSafeZoneAction_when_behaviorActs_then_theLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorHasFreeEntryAfterAction(&Navigator_planRisePenBeforeGoingToSafeZone);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanRisePenBeforeGoingToSafeZoneAction_when_behaviorActs_then_theLastBehaviorHasAPlanTowardsSafeZoneAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planRisePenBeforeGoingToSafeZone,
            &Navigator_planTowardsSafeZone);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsSafeZoneAction_when_behaviorActs_then_theLastBehaviorOfTheRobotAreMovementBehaviorsFollowingThePlannedTrajectory
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBehaviorsAreANavigationAndMovementChainFollowingThePlannedTrajectoryAfterAction(&Navigator_planTowardsSafeZone,
            &assertBehaviorHasFreeTrajectoryEntryAndDrawingTolerances);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsSafeZoneAction_when_behaviorActs_then_theLastBehaviorOfTheRobotHasTheSafeZoneAsEntryConditions
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planTowardsSafeZone;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    struct Coordinates *expected_coordinates = Map_retrieveSafeZone(robot->navigator->navigable_map);
    struct Coordinates *goal_coordinates = last_behavior->entry_conditions->goal_state->pose->coordinates;
    cr_assert(expected_coordinates->x == goal_coordinates->x);
    cr_assert(abs(expected_coordinates->y == goal_coordinates->y) <= 100);
    Coordinates_delete(expected_coordinates);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanTowardsSafeZoneAction_when_behaviorActs_then_theLastBehaviorHasAPlanStopMotionForEndOfCycleAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planTowardsSafeZone,
            &Navigator_planStopMotionForEndOfCycle);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionForEndOfCycleAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planStopMotionForEndOfCycle);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionForEndOfCycleAction_when_behaviorActs_then_theBeforeLastBehaviorHasAStopMovementAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planStopMotionForEndOfCycle,
            &Navigator_stopMovement);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionForEndOfCycleAction_when_behaviorActs_then_theLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorHasFreeEntryAfterAction(&Navigator_planStopMotionForEndOfCycle);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanStopMotionForEndOfCycleAction_when_behaviorActs_then_theLastBehaviorHasAPlanEndOfCycleAndSendSignalAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planStopMotionForEndOfCycle,
            &Navigator_planEndOfCycleAndSendSignal);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanEndOfCycleAndSendSignalAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planEndOfCycleAndSendSignal);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanEndOfCycleAndSendSignalAction_when_behaviorActs_then_theBeforeLastBehaviorHasACloseCycleAndSendEndOfCycleSignalAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planEndOfCycleAndSendSignal,
            &Robot_closeCycleAndSendEndOfCycleSignal);
}

void assertBehaviorHasEndOfCycleReceivedByStationEntry(struct Behavior *behavior)
{
    struct Flags *end_of_cycle_received_by_station = Flags_irrelevant();
    Flags_setEndOfCycleReceivedByStation(end_of_cycle_received_by_station, 1);
    assertBehaviorHasFreePoseEntry(behavior);
    struct Flags *behavior_entry_flags = behavior->entry_conditions->goal_state->flags;
    cr_assert(Flags_haveTheSameValues(end_of_cycle_received_by_station, behavior_entry_flags));
    Flags_delete(end_of_cycle_received_by_station);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanEndOfCycleAndSendSignalAction_when_behaviorActs_then_theLastBehaviorHasAEndOfCycleReceivedByStationFlagsWithFreePoseEntryGoal
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planEndOfCycleAndSendSignal;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasEndOfCycleReceivedByStationEntry(last_behavior);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanEndOfCycleAndSendSignalAction_when_behaviorActs_then_theLastBehaviorHasAPlanTowardsDrawingStartAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planEndOfCycleAndSendSignal,
            &Navigator_planLightingRedLedUntilNewCycle);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLightingRedLedUntilNewCycleAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planLightingRedLedUntilNewCycle);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLightingRedLedUntilNewCycleAction_when_behaviorActs_then_theBeforeLastBehaviorHasALightRedLedAndWaitAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planLightingRedLedUntilNewCycle,
            &Robot_lightRedLed);
}

void assertBehaviorHasStartCycleFlagEntry(struct Behavior *behavior)
{
    struct Flags *start_signal_received_flags = Flags_irrelevant();
    Flags_setStartCycleSignalReceived(start_signal_received_flags, 1);
    assertBehaviorHasFreePoseEntry(behavior);
    struct Flags *behavior_entry_flags = behavior->entry_conditions->goal_state->flags;
    cr_assert(Flags_haveTheSameValues(start_signal_received_flags, behavior_entry_flags));
    Flags_delete(start_signal_received_flags);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLightingRedLedUntilNewCycleAction_when_behaviorActs_then_theLastBehaviorHasAStartCycleReceivedByStationFlagsWithFreePoseEntryGoal
     , .init = setup_robot
     , .fini = teardown_robot)
{
    robot->current_behavior->action = &Navigator_planLightingRedLedUntilNewCycle;
    Behavior_act(robot->current_behavior, robot);
    struct Behavior *last_behavior = fetchLastBehavior(robot->current_behavior);
    assertBehaviorHasEndOfCycleReceivedByStationEntry(last_behavior);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanLightingRedLedUntilNewCycleAction_when_behaviorActs_then_theLastBehaviorHasAPlanTakingPictureAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planLightingRedLedUntilNewCycle,
            &Navigator_planUpdateMapForNewCycle);
}

Test(RobotBehaviors,
     given_aBehaviorWithPlanUpdateMapForNewCycleAction_when_behaviorActs_then_theBeforeLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorHasFreeEntryAfterAction(&Navigator_planUpdateMapForNewCycle);
}

Test(RobotBehaviors,
     given_aBehaviorWithUpdateMapForNewCycleAction_when_behaviorActs_then_theBeforeLastBehaviorHasAnUpdateNavigableMapAction
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertBeforeLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planUpdateMapForNewCycle,
            &Navigator_updateNavigableMap);
}

Test(RobotBehaviors,
     given_aBehaviorWithUpdateMapForNewCycleAction_when_behaviorActs_then_theLastBehaviorHasAFreeEntry
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorHasFreeEntryAfterAction(&Navigator_planUpdateMapForNewCycle);
}

Test(RobotBehaviors,
     given_aBehaviorWithUpdateMapForNewCycleAction_when_behaviorActs_then_theLastBehaviorHasAPlanTowardsAntennaMiddle
     , .init = setup_robot
     , .fini = teardown_robot)
{
    assertLastBehaviorAfterExecutionOfFirstActionHasTheAction(&Navigator_planUpdateMapForNewCycle,
            &Navigator_planTowardsAntennaMiddle);
}

/*END OF BEHAVIORS*/

Test(Robot,
     given_theRobot_when_sendReadyToDrawIsCalledTwiceUnderThreeSeconds_then_theCommandIsSendOnlyOnce
     , .init = setup_robot
     , .fini = teardown_robot)
{
    while(!Timer_hasTimePassed(robot->timer, THREE_SECONDS));

    Robot_sendReadyToDrawSignal(robot);
    Robot_sendReadyToDrawSignal(robot);

    cr_assert_eq(ready_to_draw_signal_count, 1);
}

Test(Robot,
     given_theRobot_when_sendReadyToDrawIsCalledAndIsCalledAgainAfterThreeSeconds_then_theCommandIsSendTwice
     , .init = setup_robot
     , .fini = teardown_robot)
{
    while(!Timer_hasTimePassed(robot->timer, THREE_SECONDS));

    Robot_sendReadyToDrawSignal(robot);

    while(!Timer_hasTimePassed(robot->timer, THREE_SECONDS));

    Robot_sendReadyToDrawSignal(robot);

    cr_assert_eq(ready_to_draw_signal_count, 2);
}

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

    Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset(robot);

    while(!Timer_hasTimePassed(robot->timer, ONE_SECOND));

    Robot_fetchManchesterCodeIfAtLeastASecondHasPassedSinceLastRobotTimerReset(robot);

    cr_assert_eq(manchester_code_command_count, 2);
}

Test(Robot, given_theRobot_when_askedToLightGreenLedAndWaitASecond_then_theCommandIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_lightGreenLedAndWaitASecond(robot);

    cr_assert_eq(validation_light_green_led_command_is_sent, SENT);
}

Test(Robot,
     given_theRobot_when_askedToLightGreenLedAndWaitASecond_then_atLeastASecondHasPassedSinceTheActionWasTriggered
     , .init = setup_robot
     , .fini = teardown_robot)
{
    struct Timer *timer = Timer_new();
    Robot_lightGreenLedAndWaitASecond(robot);

    cr_assert(Timer_hasTimePassed(timer, ONE_SECOND));

    Timer_delete(timer);
}

Test(Robot, given_theRobot_when_askedToLightRedLedAndWaitASecond_then_theCommandIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_lightRedLed(robot);

    cr_assert_eq(validation_light_red_led_command_is_sent, SENT);
}

Test(Robot, given_theRobot_when_askedToStopWaitTwoSecondsLowerPenWaitTwoSecond_then_theCommandIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_stopWaitTwoSecondsLowerPenWaitTwoSecond(robot);

    cr_assert_eq(validation_lower_pen_command_is_sent, SENT);
}

Test(Robot,
     given_theRobot_when_askedToStopWaitTwoSecondsLowerPenWaitTwoSecond_then_atLeastFourSecondsHalfHavePassedSinceTheActionWasTriggered
     , .init = setup_robot
     , .fini = teardown_robot)
{
    struct Timer *timer = Timer_new();
    Robot_stopWaitTwoSecondsLowerPenWaitTwoSecond(robot);

    cr_assert(Timer_hasTimePassed(timer, FOUR_SECONDS));

    Timer_delete(timer);
}

Test(Robot,
     given_theRobot_when_askedToStopWaitTwoSecondsLowerPenWaitTwoSecond_then_aSpeedsCommandMovementIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{

    Robot_stopWaitTwoSecondsLowerPenWaitTwoSecond(robot);

    cr_assert_eq(validation_send_speeds_command_is_sent, SENT);
}

Test(Robot, given_theRobot_when_askedToStopWaitTwoSecondsRisePenWaitTwoSecond_then_theCommandIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{

    Robot_stopWaitTwoSecondsRisePenWaitTwoSecond(robot);

    cr_assert_eq(validation_rise_pen_command_is_sent, SENT);
}

Test(Robot,
     given_theRobot_when_askedToStopWaitTwoSecondsRisePenWaitTwoSecond_then_atLeastFourSecondsHalfHavePassedSinceTheActionWasTriggered
     , .init = setup_robot
     , .fini = teardown_robot)
{
    struct Timer *timer = Timer_new();

    Robot_stopWaitTwoSecondsRisePenWaitTwoSecond(robot);

    cr_assert(Timer_hasTimePassed(timer, FOUR_SECONDS));

    Timer_delete(timer);
}

Test(Robot,
     given_theRobot_when_askedToStopWaitTwoSecondsRisePenWaitTwoSecond_then_aSpeedsCommandMovementIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{

    Robot_stopWaitTwoSecondsRisePenWaitTwoSecond(robot);

    cr_assert_eq(validation_send_speeds_command_is_sent, SENT);
}

Test(Robot,
     given_aRobot_when_askedToCloseCycleAndSendEndOfCycleSignal_then_theRobotFlagsAreAllZeroedExpectHasCompletedACycle
     , .init = setup_robot
     , .fini = teardown_robot)
{
    struct Flags *expected_flags = Flags_new();
    Flags_setHasCompletedACycle(expected_flags, TRUE);
    Robot_closeCycleAndSendEndOfCycleSignal(robot);

    cr_assert(Flags_haveTheSameValues(expected_flags, robot->current_state->flags));
    Flags_delete(expected_flags);
}

Test(Robot,
     given_aRobot_when_askedToCloseCycleAndSendEndOfCycleSignal_then_theEndOfCycleSignalIsSent
     , .init = setup_robot
     , .fini = teardown_robot)
{
    Robot_closeCycleAndSendEndOfCycleSignal(robot);
    cr_assert_eq(validation_end_of_cycle_is_sent, SENT);
}
