#include <stdlib.h>
#include "Wheels.h"

struct Wheels *Wheels_new(void)
{
    struct Object *new_object = Object_new();
    struct Sensor *new_translation_sensor = Sensor_new();
    struct Sensor *new_rotation_sensor = Sensor_new();
    struct Actuator *new_translation_actuator = Actuator_new();
    struct Actuator *new_rotation_actuator = Actuator_new();
    struct Actuator *new_speed_actuator = Actuator_new();
    struct Coordinates *new_translation_data_movement = Coordinates_zero();
    struct Coordinates *new_translation_data_speed = Coordinates_zero();
    struct Angle *new_rotation_data_movement = Angle_new(0);
    struct Angle *new_rotation_data_speed = Angle_new(0);
    struct Coordinates *new_translation_command = Coordinates_zero();
    struct Coordinates *new_speed_command = Coordinates_zero();
    struct Angle *new_rotation_command = Angle_new(0);
    struct Wheels *pointer = malloc(sizeof(struct Wheels));

    pointer->object = new_object;
    pointer->translation_sensor = new_translation_sensor;
    pointer->rotation_sensor = new_rotation_sensor;
    pointer->translation_actuator = new_translation_actuator;
    pointer->speed_actuator = new_speed_actuator;
    pointer->rotation_actuator = new_rotation_actuator;

    pointer->translation_data_movement = new_translation_data_movement;
    pointer->translation_data_speed = new_translation_data_speed;
    pointer->rotation_data_movement = new_rotation_data_movement;
    pointer->rotation_data_speed = new_rotation_data_speed;
    pointer->translation_command = new_translation_command;
    pointer->speed_command = new_speed_command;
    pointer->rotation_command = new_rotation_command;
    return pointer;
}

void Wheels_delete(struct Wheels *wheels)
{
    Object_removeOneReference(wheels->object);

    if(Object_canBeDeleted(wheels->object)) {
        Object_delete(wheels->object);
        Sensor_delete(wheels->translation_sensor);
        Sensor_delete(wheels->rotation_sensor);
        Actuator_delete(wheels->translation_actuator);
        Actuator_delete(wheels->speed_actuator);
        Actuator_delete(wheels->rotation_actuator);
        Coordinates_delete(wheels->translation_data_movement);
        Coordinates_delete(wheels->translation_data_speed);
        Angle_delete(wheels->rotation_data_movement);
        Angle_delete(wheels->rotation_data_speed);
        Coordinates_delete(wheels->translation_command);
        Coordinates_delete(wheels->speed_command);
        Angle_delete(wheels->rotation_command);
        free(wheels);
    }
}

void Wheels_receiveTranslationData(struct Wheels *wheels, struct Coordinates *translation_vector,
                                   struct Coordinates *translation_speed_vector)
{
    struct Coordinates *coordinates_zero = Coordinates_zero();

    if(!Coordinates_haveTheSameValues(translation_vector, coordinates_zero) &&
       !Coordinates_haveTheSameValues(translation_speed_vector, coordinates_zero)) {
        Sensor_receivesData(wheels->translation_sensor);
    }

    Coordinates_copyValuesFrom(wheels->translation_data_movement, translation_vector);
    Coordinates_copyValuesFrom(wheels->translation_data_speed, translation_speed_vector);
    Coordinates_delete(coordinates_zero);
}

void Wheels_receiveRotationData(struct Wheels *wheels, struct Angle *rotation_angle, struct Angle *rotation_angle_speed)
{
    if(rotation_angle->theta != 0 && rotation_angle_speed->theta != 0) {
        Sensor_receivesData(wheels->rotation_sensor);
    }

    wheels->rotation_data_movement->theta = rotation_angle->theta;
    wheels->rotation_data_speed->theta = rotation_angle_speed->theta;
}

void Wheels_prepareTranslationCommand(struct Wheels *wheels, struct Coordinates *translation_vector,
                                      struct Angle *current_angle)
{
    struct Coordinates *coordinates_zero = Coordinates_zero();

    if(!Coordinates_haveTheSameValues(coordinates_zero, translation_vector)) {
        Actuator_preparesCommand(wheels->translation_actuator);
    }

    // Reverses the angle for the wheels need a translation
    // command for their relative (0,0)
    struct Angle *correction_angle = Angle_new(-1 * current_angle->theta);

    Coordinates_rotateOfAngle(translation_vector, correction_angle);
    Coordinates_copyValuesFrom(wheels->translation_command, translation_vector);

    Coordinates_delete(coordinates_zero);
    Angle_delete(correction_angle);
}

void Wheels_prepareSpeedCommand(struct Wheels *wheels, struct Coordinates *speeds_vector)
{
    Actuator_preparesCommand(wheels->speed_actuator);

    Coordinates_copyValuesFrom(wheels->speed_command, speeds_vector);
}

void Wheels_prepareRotationCommand(struct Wheels *wheels, struct Angle *angle)
{
    Actuator_preparesCommand(wheels->rotation_actuator);

    wheels->rotation_command->theta = angle->theta;
}

void Wheels_readTranslationData(struct Wheels *wheels, struct Angle *current_angle,
                                struct Coordinates *translation_delta, struct Coordinates *translation_speed)
{
    Sensor_readsData(wheels->translation_sensor);

    Coordinates_rotateOfAngle(wheels->translation_data_movement, current_angle);
    Coordinates_rotateOfAngle(wheels->translation_data_speed, current_angle);

    Coordinates_copyValuesFrom(translation_delta, wheels->translation_data_movement);
    Coordinates_copyValuesFrom(translation_speed, wheels->translation_data_speed);
}

void Wheels_readSpeedData(struct Wheels *wheels, struct Coordinates *translation_speed)
{
    Sensor_readsData(wheels->translation_sensor);

    Coordinates_copyValuesFrom(translation_speed, wheels->translation_data_speed);
}

void Wheels_readRotationData(struct Wheels *wheels, struct Angle *rotation_delta, struct Angle *rotation_speed)
{
    Sensor_readsData(wheels->rotation_sensor);
    rotation_delta->theta = wheels->rotation_data_movement->theta;
    rotation_speed->theta = wheels->rotation_data_speed->theta;
}

