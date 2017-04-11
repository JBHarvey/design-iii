#include <criterion/criterion.h>
#include <stdio.h>
#include "Logger.h"
#include "Timer.h"

struct CommandSender *command_sender;
struct CommandSender_Callbacks callbacks;
struct Navigator *navigator;
struct Robot *robot;
const int COMMAND_SENT = 1;
const int DEFAULT_SPEED = 500;
extern const int STM_CLOCK_TIME_IN_MS;
extern struct Timer *command_timer;
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
     given_aRobotWithNewDataInItsWorldCamera_when_askedToUpdateNavigableWorld_then_theWorldCameraNoLongerHasNewData
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    int has_new_data = robot->world_camera->map_sensor->has_received_new_data;
    cr_assert(!has_new_data);
}

Test(Navigator,
     given_aRobotWithNewDataInItsWorldCamera_when_askedToUpdateNavigableWorld_then_theRobotsNavigableMapIsReadyFlagIsOne
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    Sensor_receivesData(robot->world_camera->map_sensor);
    Navigator_updateNavigableMap(robot);
    int navigable_map_is_ready = robot->current_state->flags->navigable_map_is_ready;
    cr_assert(navigable_map_is_ready);
}

Test(Navigator, given_anAngleSmallerThanTheThetaTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle = THETA_TOLERANCE_DEFAULT / 2;
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED);
    cr_assert(is_oriented);
}

Test(Navigator, given_anAngleEqualToTheTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIsNot
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle = THETA_TOLERANCE_DEFAULT;
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED);
    cr_assert(!is_oriented);
}

Test(Navigator,
     given_anAngleBiggerThanToleranceButSmallerTheAQuarterOfPi_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIsNot
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle = QUARTER_PI - THETA_TOLERANCE_DEFAULT;
    cr_assert(angle > THETA_TOLERANCE_DEFAULT,
              "\n Attention! The current theta tolerance default doesn't allow correct orientation. It is too big.");
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED);
    cr_assert(!is_oriented);
}

Test(Navigator,
     given_anAngleBiggerThanAQuarterOfPiButSmallerThanHalfPiMinusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIsNot
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle = QUARTER_PI + THETA_TOLERANCE_DEFAULT;
    cr_assert(angle < (HALF_PI - THETA_TOLERANCE_DEFAULT),
              "\n Attention! The current theta tolerance default doesn't allow correct orientation. It is too big.");
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED);
    cr_assert(!is_oriented);
}

Test(Navigator,
     given_anAngleHalfPiMinusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIsNot
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  HALF_PI - THETA_TOLERANCE_DEFAULT;
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED);
    cr_assert(!is_oriented);
}

Test(Navigator,
     given_anAngleBetweenHalfPiAndHalfPiMinusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  HALF_PI - (THETA_TOLERANCE_DEFAULT / 2);
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED);
    cr_assert(is_oriented);
}

Test(Navigator,
     given_anAngleExactlyEqualToHalfPi_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  HALF_PI;
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED);
    cr_assert(is_oriented);
}

Test(Navigator,
     given_anAngleBetweenPiAndPiMinusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  PI - (THETA_TOLERANCE_DEFAULT / 2);
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED);
    cr_assert(is_oriented);
}

Test(Navigator,
     given_anAngleBetweenMinusPiAndMinusPiPlusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  MINUS_PI + (THETA_TOLERANCE_DEFAULT / 2);
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED);
    cr_assert(is_oriented);
}

void updateRobotGoalCoordinatesTo(struct Coordinates *coordinates)
{
    Coordinates_copyValuesFrom(robot->current_behavior->first_child->entry_conditions->goal_state->pose->coordinates,
                               coordinates);
}

void updateRobotGoalAngleTo(int angle)
{
    robot->current_behavior->first_child->entry_conditions->goal_state->pose->angle->theta = angle;
}

const int NAVIGATOR_ROBOT_X = 2000;
const int NAVIGATOR_ROBOT_Y = 2000;
const int TARGET_DELTA_X = 1000;
const int TARGET_DELTA_Y = 1000;

Test(Navigator,
     given__when_askedToStopMovement_then_aRotationCommandWithValueZeroIsSentAndASpeedCommandWithAllZeroValuesIsSent
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{

    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_stopMovement(robot);

    cr_assert(speeds_validator == COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_speeds_command_x == 0);
    cr_assert(sent_speeds_command_y == 0);
    cr_assert(sent_rotation_command_theta == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotThatWasAndIsOrientedTowardsItsGoal_when_askedToNavigateTowardsGoal_then_aSpeedsCommandIsSent
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{

    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator == COMMAND_SENT);
    cr_assert(rotation_validator != COMMAND_SENT);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotOrientedTowardsItsGoal_when_askedToNavigateTowardsGoal_then_wasOrientedBeforeLastCommandIsOne
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(navigator->was_oriented_before_last_command);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedTowardsItsGoal_when_askedToNavigateTowardsGoal_then_wasOrientedBeforeLastCommandIsZero
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y + TARGET_DELTA_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(!navigator->was_oriented_before_last_command);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedTowardsItsGoal_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSent
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y + TARGET_DELTA_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotWithAPlannedTrajectoryReceivedByStationFlagValueOfOne_when_askedToNavigateTowardsGoal_then_theFlagsValueIsSetToZero
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    Flags_setPlannedTrajectoryReceivedByStation(robot->current_state->flags, 1);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert_eq(robot->current_state->flags->planned_trajectory_received_by_station, FALSE);
    Coordinates_delete(target_coordinates);
}

Test(Navigator,
     given_aRobotThatWasNotOrientedTowardsItsGoalAndIsNow_when_askedToNavigateTowardsGoal_then_aStopCommandIsSent
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 0;

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator == COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta == 0);
    cr_assert(sent_speeds_command_y == 0);
    cr_assert(sent_speeds_command_x == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheEastAndNorthEast_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithThePositiveAngleBetweenTheAbsoluteRobotEastAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y + TARGET_DELTA_Y - MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta > 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheNorthEastAndNorth_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithTheNegativeAngleBetweenTheAbsoluteRobotNorthAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y + TARGET_DELTA_Y + MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta < 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheNorthAndNorthWest_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithThePositiveAngleBetweenTheAbsoluteRobotNorthAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X - TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y + TARGET_DELTA_Y + MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta > 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheWestAndNorthWest_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithTheNegativeAngleBetweenTheAbsoluteRobotWestAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X - TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y + TARGET_DELTA_Y - MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta < 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheEastAndSouthEast_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithTheNegativeAngleBetweenTheAbsoluteRobotEastAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y - TARGET_DELTA_Y + MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta < 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheSouthEastAndSouth_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithThePositiveAngleBetweenTheAbsoluteRobotSouthAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y - TARGET_DELTA_Y - MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta > 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheSouthAndSouthWest_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithTheNegativeAngleBetweenTheAbsoluteRobotSouthAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X - TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y - TARGET_DELTA_Y - MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta < 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheWestAndSouthWest_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithThePositiveAngleBetweenTheAbsoluteRobotWestAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X - TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y - TARGET_DELTA_Y + MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta > 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotOrientedWithItsGoalToTheEast_when_askedToNavigateTowardsGoal_then_theSpeedsCommandIsSentWithPositiveXAndZeroY
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x > 0);
    cr_assert(sent_speeds_command_y == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotOrientedWithItsGoalToTheNorth_when_askedToNavigateTowardsGoal_then_theSpeedsCommandIsSentWithZeroXAndPositiveY
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y + TARGET_DELTA_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x == 0);
    cr_assert(sent_speeds_command_y > 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotOrientedWithItsGoalToTheSouth_when_askedToNavigateTowardsGoal_then_theSpeedsCommandIsSentWithZeroXAndNegativeY
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y - TARGET_DELTA_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x == 0, "Angle to target : %d \n", sent_speeds_command_x);
    cr_assert(sent_speeds_command_y < 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotOrientedWithItsGoalToTheWest_when_askedToNavigateTowardsGoal_then_theSpeedsCommandIsSentWithNegativeXAndZeroY
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X - TARGET_DELTA_X, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x < 0);
    cr_assert(sent_speeds_command_y == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}
/*
Test(Navigator,
     given_aRobotSeparatedOfItsGoalOfMoreThanTheLowSpeedDistance_when_navigateTowardsGoal_then_theSpeedsCommandValueIsTheMediumSpeed
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + (2 * LOW_SPEED_DISTANCE),
            NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x == MEDIUM_SPEED_VALUE);
    cr_assert(sent_speeds_command_y == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotSeparatedOfItsGoalByTheLowSpeedDistance_when_navigateTowardsGoal_then_theSpeedsCommandValueIsTheLowSpeed
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + LOW_SPEED_DISTANCE, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x == LOW_SPEED_VALUE);
    cr_assert(sent_speeds_command_y == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotSeparatedOfItsGoalByLessThanTheLowSpeedDistanceButMoreThanTheStopDistance_when_navigateTowardsGoal_then_theSpeedsCommandValueIsTheLowSpeed
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    int distance = (4 * LOW_SPEED_DISTANCE / 5);
    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + distance, NAVIGATOR_ROBOT_Y);
    cr_assert(distance > STOP_DISTANCE,
              "Warning! This test won't pass because the stop distance is greater than the used distance\n\n");
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x == LOW_SPEED_VALUE);
    cr_assert(sent_speeds_command_y == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}
*/
/*
Test(Navigator,
     given_aRobotSeparatedOfItsGoalByTheStopDistance_when_navigateTowardsGoal_then_theSpeedsCommandValueIsTheStopSpeed
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    int distance = STOP_DISTANCE;
    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + distance, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x == STOP_VALUE);
    cr_assert(sent_speeds_command_y == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}
*/

Test(Navigator,
     given_aRobotSeparatedOfItsGoalByLessThanTheStopDistance_when_navigateTowardsGoal_then_theSpeedsCommandValueIsTheStopSpeed
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y);
    updateRobotGoalCoordinatesTo(target_coordinates);

    navigator->was_oriented_before_last_command = 1;
    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x == STOP_VALUE);
    cr_assert(sent_speeds_command_y == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotDirectlyAlignedWithItsGoal_when_orientTowardsGoal_then_aRotationCommandIsSentWithAValueOfZero
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int target_angle = 0;
    updateRobotGoalAngleTo(target_angle);

    Navigator_orientRobotTowardsGoal(robot);

    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(speeds_validator != COMMAND_SENT);

    cr_assert(sent_rotation_command_theta == 0);
}

Test(Navigator,
     given_aRobotAngularlySeparatedOfItsGoalOfTheDefaultThetaTolerance_when_orientTowardsGoal_then_aRotationCommandIsSentWithAValueOfZero
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int target_angle = THETA_TOLERANCE_DEFAULT;
    updateRobotGoalAngleTo(target_angle);

    Navigator_orientRobotTowardsGoal(robot);

    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(speeds_validator != COMMAND_SENT);

    cr_assert(sent_rotation_command_theta == 0);
}

Test(Navigator,
     given_aRobotAngularlySeparatedOfItsGoalOfAnAngleGreaterThanTheDefaultThetaTolerance_when_orientTowardsGoal_then_aRotationCommandIsSentWithAPositiveValue
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int target_angle = 2 * THETA_TOLERANCE_DEFAULT;
    updateRobotGoalAngleTo(target_angle);

    Navigator_orientRobotTowardsGoal(robot);

    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(speeds_validator != COMMAND_SENT);

    cr_assert(sent_rotation_command_theta > 0);
}

Test(Navigator,
     given_aRobotAngularlySeparatedOfItsGoalOfMinusTheDefaultThetaTolerance_when_orientTowardsGoal_then_aRotationCommandIsSentWithAValueOfZero
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int target_angle = -1 * THETA_TOLERANCE_DEFAULT;
    updateRobotGoalAngleTo(target_angle);

    Navigator_orientRobotTowardsGoal(robot);

    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(speeds_validator != COMMAND_SENT);

    cr_assert(sent_rotation_command_theta == 0);
}

Test(Navigator,
     given_aRobotAngularlySeparatedOfItsGoalOfAnAngleSmallerThanMinusTheDefaultThetaTolerance_when_orientTowardsGoal_then_aRotationCommandIsSentWithANegativeValue
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int target_angle = -2 * THETA_TOLERANCE_DEFAULT;
    updateRobotGoalAngleTo(target_angle);

    Navigator_orientRobotTowardsGoal(robot);

    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(speeds_validator != COMMAND_SENT);

    cr_assert(sent_rotation_command_theta < 0);
}

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

