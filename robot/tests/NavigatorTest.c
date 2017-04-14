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
    cr_assert(navigator->trajectory_start_angle->theta == 0);
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
    int angle = THETA_TOLERANCE_MOVING / 2;
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED, THETA_TOLERANCE_MOVING);
    cr_assert(is_oriented);
}

Test(Navigator, given_anAngleEqualToTheTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIsNot
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle = THETA_TOLERANCE_MOVING;
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED, THETA_TOLERANCE_MOVING);
    cr_assert(!is_oriented);
}

Test(Navigator,
     given_anAngleBiggerThanToleranceButSmallerTheAQuarterOfPi_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIsNot
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle = QUARTER_PI - THETA_TOLERANCE_MOVING;
    cr_assert(angle > THETA_TOLERANCE_MOVING,
              "\n Attention! The current theta tolerance default doesn't allow correct orientation. It is too big.");
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED, THETA_TOLERANCE_MOVING);
    cr_assert(!is_oriented);
}

Test(Navigator,
     given_anAngleBiggerThanAQuarterOfPiButSmallerThanHalfPiMinusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIsNot
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle = QUARTER_PI + THETA_TOLERANCE_MOVING;
    cr_assert(angle < (HALF_PI - THETA_TOLERANCE_MOVING),
              "\n Attention! The current theta tolerance default doesn't allow correct orientation. It is too big.");
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED, THETA_TOLERANCE_MOVING);
    cr_assert(!is_oriented);
}

Test(Navigator,
     given_anAngleHalfPiMinusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIsNot
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  HALF_PI - THETA_TOLERANCE_MOVING;
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED, THETA_TOLERANCE_MOVING);
    cr_assert(!is_oriented);
}

Test(Navigator,
     given_anAngleBetweenHalfPiAndHalfPiMinusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  HALF_PI - (THETA_TOLERANCE_MOVING / 2);
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED, THETA_TOLERANCE_MOVING);
    cr_assert(is_oriented);
}

Test(Navigator,
     given_anAngleExactlyEqualToHalfPi_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  HALF_PI;
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED, THETA_TOLERANCE_MOVING);
    cr_assert(is_oriented);
}

Test(Navigator,
     given_anAngleBetweenPiAndPiMinusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  PI - (THETA_TOLERANCE_MOVING / 2);
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED, THETA_TOLERANCE_MOVING);
    cr_assert(is_oriented);
}

Test(Navigator,
     given_anAngleBetweenMinusPiAndMinusPiPlusTolerance_when_askedIfTheAngleIsWithinTheCapTolerance_then_itIs
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int angle =  MINUS_PI + (THETA_TOLERANCE_MOVING / 2);
    int is_oriented = Navigator_isAngleWithinCapTolerance(angle, DEFAULT_SPEED, THETA_TOLERANCE_MOVING);
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
     given__when_askedToStopMovement_then_aSpeedCommandWithAllZeroValuesIsSent
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
    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(sent_speeds_command_x == 0);
    cr_assert(sent_speeds_command_y == 0);

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

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

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
    cr_assert(sent_speeds_command_y == 0);
    cr_assert(sent_speeds_command_x == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheEastAndNorthEast_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithTheNegativeAngleBetweenTheAbsoluteRobotNorthAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y + TARGET_DELTA_Y - MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta < 0);

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

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

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

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta > 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheWestAndNorthWest_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithThePositiveAngleBetweenTheAbsoluteRobotNorthAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X - TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y + TARGET_DELTA_Y - MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta > 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheEastAndSouthEast_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithThePositiveAngleBetweenTheAbsoluteRobotSouthAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X + TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y - TARGET_DELTA_Y + MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta > 0);

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

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

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

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta < 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotNotOrientedWithItsGoalBetweenTheWestAndSouthWest_when_askedToNavigateTowardsGoal_then_aRotationCommandIsSentWithTheNegativeAngleBetweenTheAbsoluteRobotSouthAndTheTarget
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    struct Pose *robot_pose = Pose_new(NAVIGATOR_ROBOT_X, NAVIGATOR_ROBOT_Y, 0);
    Pose_copyValuesFrom(robot->current_state->pose, robot_pose);

    struct Coordinates *target_coordinates = Coordinates_new(NAVIGATOR_ROBOT_X - TARGET_DELTA_X,
            NAVIGATOR_ROBOT_Y - TARGET_DELTA_Y + MINIMAL_GAP);
    updateRobotGoalCoordinatesTo(target_coordinates);

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(speeds_validator != COMMAND_SENT);
    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(sent_rotation_command_theta < 0);

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

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

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

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

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

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

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

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_navigateRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_x < 0);
    cr_assert(sent_speeds_command_y == 0);

    Coordinates_delete(target_coordinates);
    Pose_delete(robot_pose);
}

Test(Navigator,
     given_aRobotAngularlySeparatedOfItsGoalOfAnAngleGreaterThanTheDefaultThetaTolerance_when_orientTowardsGoal_then_aRotationCommandIsSentWithAPositiveValue
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int target_angle = 2 * THETA_TOLERANCE_MOVING;
    updateRobotGoalAngleTo(target_angle);

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_orientRobotTowardsGoal(robot);

    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(speeds_validator != COMMAND_SENT);

    cr_assert(sent_rotation_command_theta > 0);
}

Test(Navigator,
     given_aRobotAngularlySeparatedOfItsGoalOfAnAngleSmallerThanTheTheDefaultThetaToleranceAndGreaterThanHalfOfTheDefaultTolerance_when_orientTowardsGoal_then_aRotationCommandIsSentWithOmegaMediumSpeed
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int target_angle = THETA_TOLERANCE_MOVING - 1;
    updateRobotGoalAngleTo(target_angle);

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_orientRobotTowardsGoal(robot);

    cr_assert(rotation_validator == COMMAND_SENT);
    cr_assert(speeds_validator != COMMAND_SENT);

    cr_assert(sent_rotation_command_theta == OMEGA_MEDIUM_SPEED_MOVING);
}

Test(Navigator,
     given_aRobotAngularlySeparatedOfItsGoalOfAnAngleSmallerThanTheHalfOfTheDefaultThetaTolerance_when_orientTowardsGoal_then_aStopCommandIsSent
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{
    int target_angle = THETA_TOLERANCE_MOVING / 3;
    updateRobotGoalAngleTo(target_angle);

    while(!Timer_hasTimePassed(command_timer, STM_CLOCK_TIME_IN_MS));

    Navigator_orientRobotTowardsGoal(robot);

    cr_assert(rotation_validator != COMMAND_SENT);
    cr_assert(speeds_validator == COMMAND_SENT);

    cr_assert(sent_speeds_command_y == 0);
    cr_assert(sent_speeds_command_x == 0);
}

void assertEqualityWithTolerance(int expected_value, int received_value, int tolerance)
{
    cr_assert((abs(expected_value - received_value) <= tolerance));
}

