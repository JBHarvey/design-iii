#include <criterion/criterion.h>
#include <stdio.h>
#include "Logger.h"

struct CommandSender *command_sender;
struct CommandSender_Callbacks callbacks;
struct Navigator *navigator;
struct Robot *robot;
const int COMMAND_SENT = 1;
int speeds_validator;
int sent_speeds_command_x;
int sent_speeds_command_y;
int rotation_validator;
int sent_rotation_command_theta;


void sendSpeedsCommandValidator(struct Command_Speeds speeds)
{
    sent_speeds_command_x = speeds.x;
    sent_speeds_command_y = speeds.y;
    ++speeds_validator;
}

void sendRotateCommandValidator(struct Command_Rotate rotate)
{
    sent_rotation_command_theta = rotate.theta;
    ++rotation_validator;
}

void setup_Navigator(void)
{

    robot = Robot_new();
    navigator = robot->navigator;
    command_sender = CommandSender_new();
    callbacks = CommandSender_fetchCallbacksForRobot();
    callbacks.sendSpeedsCommand = &sendSpeedsCommandValidator;
    callbacks.sendRotateCommand = &sendRotateCommandValidator;
    CommandSender_changeTarget(robot->command_sender, callbacks);

    speeds_validator = 0;
    rotation_validator = 0;
}

void teardown_Navigator(void)
{

    CommandSender_delete(command_sender);
    Robot_delete(robot);
}

Test(Navigator, creation_destruction
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    cr_assert(navigator->navigable_map == NULL);
    cr_assert(navigator->was_oriented_before_last_command == 0);
}


Test(Navigator,
     given_aRobotWithNoNewDataInItsWorldCamera_when_askedToUpdateNavigableWorld_then_theNavigableMapPointerIsStillNull
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{

    Navigator_updateNavigableMap(robot);
    cr_assert(navigator->navigable_map == NULL);
}

Test(Navigator,
     given_aRobotWithNewDataInItsWorldCamera_when_askedToUpdateNavigableWorld_then_theNavigableMapPointerIsNoLongerNull
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{

    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    cr_assert(navigator->navigable_map != NULL);
}

Test(Navigator,
     given_aRobotWithNoNewDataInItsWorldCamera_when_askedToUpdateNavigableWorld_then_theGraphTypeIsNone
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    Navigator_updateNavigableMap(robot);
    cr_assert(navigator->graph->type == NONE);
}

Test(Navigator,
     given_aRobotWithNewDataInItsWorldCamera_when_askedToUpdateNavigableWorld_then_theGraphTypeBecomesTrio
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{

    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    cr_assert(navigator->graph->type == TRIO);
}

Test(Navigator,
     given_aRobotWithNewDataInItsWorldCamera_when_askedToUpdateNavigableWorld_then_theWorldCameraNoLongerHasNewData
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    int has_new_data = robot->world_camera->map_sensor->has_received_new_data;
    cr_assert(!has_new_data);
}
void updateRobotGoalCoordinatesTo(struct Coordinates *coordinates)
{
    Coordinates_copyValuesFrom(robot->behavior->first_child->entry_conditions->goal_state->pose->coordinates,
                               coordinates);
}

const int NAVIGATOR_ROBOT_X = 2000;
const int NAVIGATOR_ROBOT_Y = 2000;
const int TARGET_DELTA_X = 1000;
const int TARGET_DELTA_Y = 1000;

Test(Navigator,
     given_aRobotWithStatePerfectlyAlignedToTheEastWithItsMovementTarget_when_askedToNavigateTowardsGoal_then_aSpeedsCommandIsSent
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{

    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator == COMMAND_SENT);
    cr_assert(rotation_validator != COMMAND_SENT);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator, given_anAngleSmallerThanTheThetaTolerance_when_askedIfThAngleIsWithinTheRotationTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle = THETA_TOLERANCE_DEFAULT / 2;
    int is_oriented = Navigator_isAngleWithinRotationTolerance(angle);
    cr_assert(is_oriented);
}


/*
Test(Navigator,
     given_aRobotWithAnOrientationAngleOutsideTheDefaultThetaToleranceToItsTargetToTheEast_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSent
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    callbacks.sendSpeedsCommand = &sendSpeedsCommandValidator;
    callbacks.sendRotateCommand = &sendRotateCommandValidator;
    CommandSender_changeTarget(robot->command_sender, callbacks);
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y + TARGET_DELTA_X - MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    //TODO: cr_assert(Coordinates_angleBetween(robot_pose->coordinates, target_coordinates) > THETA_TOLERANCE_DEFAULT);
    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotWithAnOrientationAngleWithinTheDefaultThetaToleranceToItsTargetToTheEast_when_askedToNavigateTowardsGoal_then_aTheSpeedsCommandSendIsTheDistanceBetweenTheTwoInPositiveXAndZeroInY
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
}
*/

void assertEqualityWithTolerance(int expected_value, int received_value, int tolerance)
{
    cr_assert((abs(expected_value - received_value) <= tolerance));
}

Test(Navigator, given_zero_when_askedToComputeRotationToleranceForPrecisionMovement_then_returnsTheBaseTheoricalValue)
{
    int received_base = Navigator_computeRotationToleranceForPrecisionMovement(0);
    assertEqualityWithTolerance(THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_BASE, received_base, 1);
}

Test(Navigator,
     given_aNumber_when_askedToComputeRotationToleranceForPrecisionMovement_then_returnsThatNumberTimesTheTheoricalDistanceOverRotationToleranceRationPlusTheTheoricalDistanceOverRotationToleranceBase)
{
    int number = 4200;
    int expected_value = (int)(THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_BASE +
                               (THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_RATIO * number));
    int received_value = Navigator_computeRotationToleranceForPrecisionMovement(number);
    assertEqualityWithTolerance(expected_value, received_value, 1);
}

