#include <stdlib.h>
#include "Wheels.h"

struct Wheels *Wheels_new(void)
{
    struct Object *new_object = Object_new();
    struct Sensor *new_translation_sensor = Sensor_new();
    struct Sensor *new_rotation_sensor = Sensor_new();
    struct Actuator *new_translation_actuator = Actuator_new();
    struct Actuator *new_rotation_actuator = Actuator_new();
    struct Coordinates *new_translation_data = Coordinates_zero();
    struct Angle *new_rotation_data = Angle_new(0);
    struct Coordinates *new_translation_command = Coordinates_zero();
    struct Angle *new_rotation_command = Angle_new(0);
    struct Wheels *pointer = (struct Wheels *) malloc(sizeof(struct Wheels));

    pointer->object = new_object;
    pointer->translation_sensor = new_translation_sensor;
    pointer->rotation_sensor = new_rotation_sensor;
    pointer->translation_actuator = new_translation_actuator;
    pointer->rotation_actuator = new_rotation_actuator;

    pointer->translation_data = new_translation_data;
    pointer->rotation_data = new_rotation_data;
    pointer->translation_command = new_translation_command;
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
        Actuator_delete(wheels->rotation_actuator);
        Coordinates_delete(wheels->translation_data);
        Angle_delete(wheels->rotation_data);
        Coordinates_delete(wheels->translation_command);
        Angle_delete(wheels->rotation_command);
        free(wheels);
    }
}

void Wheels_prepareRotationCommand(struct Wheels *wheels, struct Angle *angle)
{
    if(angle->theta != 0) {
        Actuator_preparesCommand(wheels->rotation_actuator);
    }

    wheels->rotation_command->theta = angle->theta;
}

void Wheels_receiveTranslationData(struct Wheels *wheels, struct Coordinates *translation_vector)
{
    struct Coordinates *coordinates_zero = Coordinates_zero();

    if(!Coordinates_haveTheSameValues(translation_vector, coordinates_zero)) {
        Sensor_receivesData(wheels->translation_sensor);
    }

    Coordinates_copyValuesFrom(wheels->translation_data, translation_vector);
    Coordinates_delete(coordinates_zero);
}

void Wheels_receiveRotationData(struct Wheels *wheels, struct Angle *rotation_angle)
{
    if(rotation_angle->theta != 0) {
        Sensor_receivesData(wheels->rotation_sensor);
    }

    wheels->rotation_data->theta = rotation_angle->theta;
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

struct Angle *Wheels_readRotationData(struct Wheels *wheels)
{
    Sensor_readsData(wheels->rotation_sensor);
    return wheels->rotation_data;
}

struct Coordinates *Wheels_readTranslationData(struct Wheels *wheels, struct Angle *current_angle)
{
    Sensor_readsData(wheels->translation_sensor);
    Coordinates_rotateOfAngle(wheels->translation_data, current_angle);
    return wheels->translation_data;
}
