#include <criterion/criterion.h>
#include <stdio.h>
#include "Wheels.h"

const int WHEELS_COMMMAND_X = 1000;
const int WHEELS_COMMMAND_X_SPEED = 10000;
const int WHEELS_COMMMAND_Y = 1200;
const int WHEELS_COMMMAND_Y_SPEED = 12000;
const int WHEELS_COMMMAND_THETA = MINUS_THREE_QUARTER_PI;
const int WHEELS_COMMMAND_THETA_SPEED = MINUS_QUARTER_PI;
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
    cr_assert(Coordinates_haveTheSameValues(wheels->translation_data_movement, coordinates_zero));
    cr_assert(Coordinates_haveTheSameValues(wheels->translation_data_speed, coordinates_zero));
    cr_assert(Angle_smallestAngleBetween(wheels->rotation_data_movement, angle_zero) == 0);
    cr_assert(Angle_smallestAngleBetween(wheels->rotation_data_speed, angle_zero) == 0);
    cr_assert(Coordinates_haveTheSameValues(wheels->translation_command, coordinates_zero));
    cr_assert(Angle_smallestAngleBetween(wheels->rotation_command, angle_zero) == 0);

    Coordinates_delete(coordinates_zero);
    Angle_delete(angle_zero);
}

Test(Wheels, given_anglesAndCoordinates_when_receivesData_then_theAngleAndCoordinatesCorrespondToThePoseComponents
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Coordinates *translation_movement = Coordinates_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);
    struct Coordinates *translation_speed = Coordinates_new(WHEELS_COMMMAND_X_SPEED, WHEELS_COMMMAND_Y_SPEED);
    struct Angle *rotation_movement = Angle_new(WHEELS_COMMMAND_THETA);
    struct Angle *rotation_speed = Angle_new(WHEELS_COMMMAND_THETA_SPEED);

    Wheels_receiveTranslationData(wheels, translation_movement, translation_speed);
    Wheels_receiveRotationData(wheels, rotation_movement, rotation_speed);

    cr_assert(Coordinates_haveTheSameValues(wheels->translation_data_movement, translation_movement));
    cr_assert(Coordinates_haveTheSameValues(wheels->translation_data_speed, translation_speed));
    cr_assert(Angle_smallestAngleBetween(wheels->rotation_data_movement, rotation_movement) == 0);
    cr_assert(Angle_smallestAngleBetween(wheels->rotation_data_speed, rotation_speed) == 0);

    Angle_delete(rotation_movement);
    Angle_delete(rotation_speed);
    Coordinates_delete(translation_movement);
    Coordinates_delete(translation_speed);
}

Test(Wheels, given_angles_when_receivesData_then_theRotationSensorHasReceivedNewData
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle = Angle_new(WHEELS_COMMMAND_THETA);

    Wheels_receiveRotationData(wheels, angle, angle);

    cr_assert(wheels->rotation_sensor->has_received_new_data);

    Angle_delete(angle);
}

Test(Wheels, given_coordinates_when_receivesData_then_theRotationSensorHasReceivedNewData
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Coordinates *coordinates = Coordinates_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);

    Wheels_receiveTranslationData(wheels, coordinates, coordinates);

    cr_assert(wheels->translation_sensor->has_received_new_data);

    Coordinates_delete(coordinates);
}

Test(Wheels, given_receivedData_when_readsRotationData_then_angleAreModifiedToCorrespondTheReceivedOnes
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle_movement = Angle_new(WHEELS_COMMMAND_THETA);
    struct Angle *angle_speed = Angle_new(WHEELS_COMMMAND_THETA_SPEED);
    Wheels_receiveRotationData(wheels, angle_movement, angle_speed);

    struct Angle *rotation_movement = Angle_new(0);
    struct Angle *rotation_speed = Angle_new(0);
    Wheels_readRotationData(wheels, rotation_movement, rotation_speed);

    cr_assert(Angle_smallestAngleBetween(rotation_movement, angle_movement) == 0);
    cr_assert(Angle_smallestAngleBetween(rotation_speed, angle_speed) == 0);

    Angle_delete(angle_movement);
    Angle_delete(angle_speed);
    Angle_delete(rotation_movement);
    Angle_delete(rotation_speed);

}

Test(Wheels, given_receivedData_when_readsRotationData_then_rotationSensorHasNoMoreNewData
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle = Angle_new(WHEELS_COMMMAND_THETA);
    Wheels_receiveRotationData(wheels, angle, angle);

    struct Angle *rotation_movement = Angle_new(0);
    struct Angle *rotation_speed = Angle_new(0);
    Wheels_readRotationData(wheels, rotation_movement, rotation_speed);

    cr_assert(!wheels->rotation_sensor->has_received_new_data);

    Angle_delete(angle);
    Angle_delete(rotation_movement);
    Angle_delete(rotation_speed);

}

Test(Wheels, given_noReceivedDataAndAZeroAngle_when_readsTranslationData_then_coordinatesZeroIsReturned
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle_zero = Angle_new(0);
    struct Coordinates *coordinates_zero = Coordinates_zero();
    struct Coordinates *translation_movement = Coordinates_zero();
    struct Coordinates *translation_speed = Coordinates_zero();

    Wheels_readTranslationData(wheels, angle_zero, translation_movement, translation_speed);

    cr_assert(Coordinates_haveTheSameValues(coordinates_zero, translation_movement));
    cr_assert(Coordinates_haveTheSameValues(coordinates_zero, translation_speed));

    Coordinates_delete(coordinates_zero);
    Coordinates_delete(translation_movement);
    Coordinates_delete(translation_speed);
    Angle_delete(angle_zero);
}

Test(Wheels, given_receivedData_when_readsTranslationData_then_translationSensorHasNoMoreNewData
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Angle *angle_zero = Angle_new(0);
    struct Coordinates *coordinates_zero = Coordinates_zero();
    struct Coordinates *translation_movement = Coordinates_zero();
    struct Coordinates *translation_speed = Coordinates_zero();

    Wheels_readTranslationData(wheels, angle_zero, translation_movement, translation_speed);

    cr_assert(!wheels->translation_sensor->has_received_new_data);

    Coordinates_delete(coordinates_zero);
    Coordinates_delete(translation_movement);
    Coordinates_delete(translation_speed);
    Angle_delete(angle_zero);
}

void assertReceivedTranslationIs(struct Angle *current_angle, struct Coordinates *sent_movement,
                                 struct Coordinates *sent_speed, int expected_x_delta, int expected_y_delta, int expected_x_speed, int expected_y_speed)
{
    struct Coordinates *received_movement = Coordinates_zero();
    struct Coordinates *received_speed = Coordinates_zero();
    Wheels_readTranslationData(wheels, current_angle, received_movement, received_speed);

    cr_assert((received_movement->x == expected_x_delta ||
               received_movement->x == expected_x_delta + 1 ||
               received_movement->x == expected_x_delta - 1)
              &&
              (received_movement->y == expected_y_delta ||
               received_movement->y == expected_y_delta + 1 ||
               received_movement->y == expected_y_delta - 1)
              &&
              (received_speed->x == expected_x_speed ||
               received_speed->x == expected_x_speed + 1 ||
               received_speed->x == expected_x_speed - 1)
              &&
              (received_speed->y == expected_y_speed ||
               received_speed->y == expected_y_speed + 1 ||
               received_speed->y == expected_y_speed - 1),
              "Sent translation (%d,%d), speed (%d,%d). Received translation (%d,%d), speed (%d,%d). Expected translation (%d,%d), speed (%d,%d).",
              sent_movement->x, sent_movement->y, sent_speed->x, sent_speed->y,
              received_movement->x, received_movement->y, received_speed->x, received_speed->y,
              expected_x_delta, expected_y_delta, expected_x_speed, expected_y_speed);
    Coordinates_delete(received_movement);
    Coordinates_delete(received_speed);
}

Test(Wheels, given_receivedDataAndAZeroAngle_when_readsTranslationData_then_returnedCoordinatesAreTheSentOnes
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Coordinates *translation_movement = Coordinates_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);
    struct Coordinates *translation_speed = Coordinates_new(WHEELS_COMMMAND_X_SPEED, WHEELS_COMMMAND_Y_SPEED);
    struct Angle *angle_zero = Angle_new(0);

    Wheels_receiveTranslationData(wheels, translation_movement, translation_speed);

    assertReceivedTranslationIs(angle_zero, translation_movement, translation_speed, WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y,
                                WHEELS_COMMMAND_X_SPEED, WHEELS_COMMMAND_Y_SPEED);

    Coordinates_delete(translation_movement);
    Coordinates_delete(translation_speed);
    Angle_delete(angle_zero);
}

Test(Wheels,
     given_receivedDataAndANonZeroAngle_when_readsTranslationData_then_returnedCoordinatesAreTheSentOnesRotatedAroundTheOriginOfCurrentAngle
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    struct Coordinates *translation_movement = Coordinates_new(WHEELS_COMMMAND_X, WHEELS_COMMMAND_Y);
    struct Coordinates *translation_speed = Coordinates_new(WHEELS_COMMMAND_X_SPEED, WHEELS_COMMMAND_Y_SPEED);
    struct Angle *angle = Angle_new(WHEELS_COMMMAND_THETA);

    Wheels_receiveTranslationData(wheels, translation_movement, translation_speed);

    int expected_x_delta = 141;
    int expected_y_delta = -1556;
    int expected_x_speed = 1415;
    int expected_y_speed = -15555;
    assertReceivedTranslationIs(angle, translation_movement, translation_speed, expected_x_delta, expected_y_delta,
                                expected_x_speed, expected_y_speed);

    Coordinates_delete(translation_movement);
    Coordinates_delete(translation_speed);
    Angle_delete(angle);
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
