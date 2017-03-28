#include <criterion/criterion.h>
#include <stdio.h>
#include "Logger.h"

struct CommandSender *command_sender;
struct CommandSender_Callbacks callbacks;
struct Navigator *navigator;
struct Robot *robot;
const int COMMAND_SENT = 1;
int translation_validator;
int rotation_validator;

void setup_Navigator(void)
{

    robot = Robot_new();
    navigator = robot->navigator;
    command_sender = CommandSender_new();
    callbacks = CommandSender_fetchCallbacksForRobot();
    translation_validator = 0;
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

void sendTranslateCommandValidator(struct Command_Translate translate)
{
    ++translation_validator;
}

void sendRotateCommandValidator(struct Command_Rotate rotate)
{
    ++rotation_validator;
}

/*
Test(Navigator,
     given_aRobotWithStatePerfectlyAlignedToTheEastWithItsMovementTarget_when_askedToMoveTowardTheCoordinates_then_aTranslationCommandIsSent
     , .init = setup_Navigator
     , .fini = teardown_Navigator)
{

    cr_assert(translation_validator == COMMAND_SENT);
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
