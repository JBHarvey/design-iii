#include <criterion/criterion.h>
#include <stdio.h>
#include "Wheels.h"

const int WHEELS_COMMMAND_X = 1000;
const int WHEELS_COMMMAND_Y = 1200;
const int WHEELS_COMMMAND_THETA = MINUS_THREE_QUARTER_PI;
struct Wheels *wheels;

void setup_wheels(void)
{
    wheels = Wheels_new();
}

void teardown_wheels(void)
{
    Wheels_delete(wheels);
}

Test(Wheels, creation_destruction
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle_zero = Angle_new(0);
    struct Coordinates *coordinates_zero = Coordinates_zero();

    cr_assert(wheels->translation_sensor->has_received_new_data == 0);
    cr_assert(wheels->rotation_sensor->has_received_new_data == 0);
    cr_assert(wheels->translation_actuator->has_prepared_new_command == 0);
    cr_assert(wheels->rotation_actuator->has_prepared_new_command == 0);

    // Actuator_preparesCommand();
    // Actuator_sendsCommand();
    cr_assert(Coordinates_haveTheSameValues(wheels->translation_command, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(wheels->translation_data, coordinates_zero));
    cr_assert(Angle_smallestAngleBetween(wheels->rotation_data, angle_zero) == 0);
    cr_assert(Angle_smallestAngleBetween(wheels->rotation_command, angle_zero) == 0);

    Coordinates_delete(coordinates_zero);
    Angle_delete(angle_zero);
}

Test(Wheels, given_aPose_when_receivesData_then_theAngleAndCoordinatesCorrespondToThePoseComponents
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Pose *pose = Pose_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y, WHEELS_COMMMAND_THETA);
    struct Coordinates *coordinates = Coordinates_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);
    struct Angle *angle = Angle_new(WHEELS_COMMMAND_THETA);

    Wheels_receiveData(wheels, pose);
    cr_assert(Coordinates_haveTheSameValues(wheels->translation_data, coordinates));
    cr_assert(Angle_smallestAngleBetween(wheels->rotation_data, angle) == 0);

    Angle_delete(angle);
    Coordinates_delete(coordinates);
    Pose_delete(pose);
}

Test(Wheels, given_aPose_when_receivesData_then_theRotationAndTranslationSensorHaveReceivedNewData
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Pose *pose = Pose_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y, WHEELS_COMMMAND_THETA);

    Wheels_receiveData(wheels, pose);
    cr_assert(wheels->translation_sensor->has_received_new_data);
    cr_assert(wheels->rotation_sensor->has_received_new_data);

    Pose_delete(pose);
}

Test(Wheels, given_aPoseWithZeroCoordinates_when_receivesData_then_onlyTheRotationSensorHasReceivedNewData
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Pose *pose = Pose_new(0, 0, WHEELS_COMMMAND_THETA);

    Wheels_receiveData(wheels, pose);
    cr_assert(!wheels->translation_sensor->has_received_new_data);
    cr_assert(wheels->rotation_sensor->has_received_new_data);

    Pose_delete(pose);
}

Test(Wheels, given_aPoseWithZeroAngle_when_receivesData_then_onlyTheTranslationSensorHasReceivedNewData
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Pose *pose = Pose_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y, 0);

    Wheels_receiveData(wheels, pose);
    cr_assert(wheels->translation_sensor->has_received_new_data);
    cr_assert(!wheels->rotation_sensor->has_received_new_data);

    Pose_delete(pose);
}

Test(Wheels, given_anAngle_when_preparesRotationCommand_then_theRotationCommandContainsTheAngle
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle = Angle_new(WHEELS_COMMMAND_THETA);

    Wheels_prepareRotationCommand(wheels, angle);
    cr_assert(Angle_smallestAngleBetween(wheels->rotation_command, angle) == 0);

    Angle_delete(angle);
}

Test(Wheels, given_anAngle_when_preparesRotationCommand_then_theWheelsRotationActuatorHasPreparedACommand
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle = Angle_new(WHEELS_COMMMAND_THETA);

    Wheels_prepareRotationCommand(wheels, angle);
    cr_assert(wheels->rotation_actuator->has_prepared_new_command);

    Angle_delete(angle);
}

Test(Wheels, given_aZeroAngle_when_preparesRotationCommand_then_theWheelsRotationActuatorHasNotPreparedACommand
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle = Angle_new(0);

    Wheels_prepareRotationCommand(wheels, angle);
    cr_assert(!wheels->rotation_actuator->has_prepared_new_command);

    Angle_delete(angle);
}

Test(Wheels,
     given_coordinatesAndAnAngle_when_preparesTranslationCommand_then_theWheelsTranslationActuatorHasPreparedACommand
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Coordinates *coordinates = Coordinates_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);
    struct Angle *angle = Angle_new(WHEELS_COMMMAND_THETA);

    Wheels_prepareTranslationCommand(wheels, coordinates, angle);
    cr_assert(wheels->translation_actuator->has_prepared_new_command);

    Angle_delete(angle);
    Coordinates_delete(coordinates);
}

Test(Wheels,
     given_zeroCoordinatesAndAnAngle_when_preparesTranslationCommand_then_theWheelsTranslationActuatorHasPreparedACommand
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Coordinates *coordinates = Coordinates_new(0, 0);
    struct Angle *angle = Angle_new(WHEELS_COMMMAND_THETA);

    Wheels_prepareTranslationCommand(wheels, coordinates, angle);
    cr_assert(!wheels->translation_actuator->has_prepared_new_command);

    Angle_delete(angle);
    Coordinates_delete(coordinates);
}

Test(Wheels,
     given_coordinatesAndAZeroAngle_when_preparesTranslationCommand_then_theWheelsTranslationCommandIsTheCoordinates
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Coordinates *coordinates = Coordinates_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);
    struct Angle *angle = Angle_new(0);

    Wheels_prepareTranslationCommand(wheels, coordinates, angle);
    cr_assert(Coordinates_haveTheSameValues(wheels->translation_command, coordinates));

    Angle_delete(angle);
    Coordinates_delete(coordinates);
}

void assertTranslationCommandIs(int initial_x, int initial_y, int expected_x, int expected_y, int theta)
{
    cr_assert((wheels->translation_command->x == expected_x ||
               wheels->translation_command->x == expected_x + 1 ||
               wheels->translation_command->x == expected_x - 1)
              &&
              (wheels->translation_command->y == expected_y ||
               wheels->translation_command->y == expected_y + 1 ||
               wheels->translation_command->y == expected_y - 1)
              ,
              "Rotation of (%d,%d) of %d -> returns: (%d,%d) - expected: (%d,%d)",
              initial_x, initial_y, -1 * theta,
              wheels->translation_command->x, wheels->translation_command->y,
              expected_x, expected_y);
}

Test(Wheels,
     given_coordinatesAndANonZeroAngle_when_preparesTranslationCommand_then_theWheelsTranslationCommandIsTheCoordinatesRotatedOfMinusTheAngle
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    int theta = PI;
    int initial_x = WHEELS_COMMMAND_X;
    int initial_y = WHEELS_COMMMAND_Y;
    struct Coordinates *coordinates = Coordinates_new(initial_x, initial_y);
    struct Angle *angle = Angle_new(theta);

    Wheels_prepareTranslationCommand(wheels, coordinates, angle);

    int expected_x = -1 * WHEELS_COMMMAND_X;
    int expected_y = -1 * WHEELS_COMMMAND_Y;
    assertTranslationCommandIs(initial_x, initial_y, expected_x, expected_y, theta);

    Angle_delete(angle);
    Coordinates_delete(coordinates);
}

Test(Wheels, given_noReceivedData_when_readsRotationData_then_angleZeroIsReturned
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle_zero = Angle_new(0);
    struct Angle *rotation_data = Wheels_readRotationData(wheels);
    cr_assert(Angle_smallestAngleBetween(angle_zero, rotation_data) == 0);
    Angle_delete(angle_zero);
}

Test(Wheels, given_receivedData_when_readsRotationData_then_rotationSensorHasNoMoreNewData
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Pose *pose = Pose_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y, WHEELS_COMMMAND_THETA);
    Wheels_receiveData(wheels, pose);

    Wheels_readRotationData(wheels);
    cr_assert(!wheels->rotation_sensor->has_received_new_data);

    Pose_delete(pose);
}

Test(Wheels, given_noReceivedDataAndAZeroAngle_when_readsTranslationData_then_coordinatesZeroIsReturned
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle_zero = Angle_new(0);
    struct Coordinates *coordinates_zero = Coordinates_zero();
    struct Coordinates *translation_data = Wheels_readTranslationData(wheels, angle_zero);
    cr_assert(Coordinates_haveTheSameValues(coordinates_zero, translation_data));
    Coordinates_delete(coordinates_zero);
    Angle_delete(angle_zero);
}

Test(Wheels, given_receivedData_when_readsTranslationData_then_translationSensorHasNoMoreNewData
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle_zero = Angle_new(0);
    struct Pose *pose = Pose_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y, WHEELS_COMMMAND_THETA);
    Wheels_receiveData(wheels, pose);

    Wheels_readTranslationData(wheels, angle_zero);
    cr_assert(!wheels->translation_sensor->has_received_new_data);

    Pose_delete(pose);
    Angle_delete(angle_zero);
}

void assertReceivedTranslationIs(struct Angle *current_angle, struct Coordinates *sent, int expected_x, int expected_y)
{
    struct Coordinates *received_coordinates = Wheels_readTranslationData(wheels, current_angle);
    cr_assert((received_coordinates->x == expected_x ||
               received_coordinates->x == expected_x + 1 ||
               received_coordinates->x == expected_x - 1)
              &&
              (received_coordinates->y == expected_y ||
               received_coordinates->y == expected_y + 1 ||
               received_coordinates->y == expected_y - 1)
              ,
              "\nCoordinates (%d,%d) were sent. With angle of %d, (%d,%d) is expected. -> Actual: (%d,%d)",
              sent->x, sent->y, current_angle->theta,
              expected_x, expected_y,
              received_coordinates->x, received_coordinates->y);
}

Test(Wheels, given_receivedDataAndAZeroAngle_when_readsTranslationData_then_returnedCoordinatesAreTheSentOnes
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle_zero = Angle_new(0);
    struct Pose *pose = Pose_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y, WHEELS_COMMMAND_THETA);

    Wheels_receiveData(wheels, pose);
    struct Coordinates *sent_coordinates = Coordinates_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);

    assertReceivedTranslationIs(angle_zero, sent_coordinates, WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);

    Pose_delete(pose);
    Angle_delete(angle_zero);
    Coordinates_delete(sent_coordinates);
}

Test(Wheels,
     given_receivedDataAndANonZeroAngle_when_readsTranslationData_then_returnedCoordinatesAreTheSentOnesRotatedAroundTheOriginOfCurrentAngle
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle_zero = Angle_new(WHEELS_COMMMAND_THETA);
    struct Pose *pose = Pose_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y, WHEELS_COMMMAND_THETA);

    Wheels_receiveData(wheels, pose);
    struct Coordinates *sent_coordinates = Coordinates_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);

    int expected_x = 141;
    int expected_y = -1556;
    assertReceivedTranslationIs(angle_zero, sent_coordinates, expected_x, expected_y);

    Pose_delete(pose);
    Angle_delete(angle_zero);
    Coordinates_delete(sent_coordinates);
}
