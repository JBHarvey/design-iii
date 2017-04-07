#ifndef WHEELS_H_
#define WHEELS_H_

#include "Sensor.h"
#include "Actuator.h"
#include "Coordinates.h"
#include "Angle.h"

struct Wheels {
    struct Object *object;
    struct Sensor *translation_sensor;
    struct Sensor *rotation_sensor;
    struct Actuator *translation_actuator;
    struct Actuator *rotation_actuator;
    struct Actuator *speed_actuator;
    struct Coordinates *translation_data_movement;
    struct Coordinates *translation_data_speed;
    struct Angle *rotation_data_movement;
    struct Angle *rotation_data_speed;
    struct Coordinates *translation_command;
    struct Coordinates *speed_command;
    struct Angle *rotation_command;
};

struct Wheels *Wheels_new(void);
void Wheels_delete(struct Wheels *wheels);

void Wheels_receiveTranslationData(struct Wheels *wheels, struct Coordinates *translation_vector, struct Coordinates *translation_speed_vector);
void Wheels_receiveRotationData(struct Wheels *wheels, struct Angle *rotation_angle, struct Angle *rotation_angle_speed);

void Wheels_readRotationSpeedData(struct Wheels *wheels, struct Angle *rotation_speed);
void Wheels_readTranslationData(struct Wheels *wheels, struct Angle *current_angle, struct Coordinates *translation_delta, struct Coordinates *translation_speed);
void Wheels_readTranslationSpeedData(struct Wheels *wheels, struct Coordinates *translation_speed);

void Wheels_prepareRotationCommand(struct Wheels *wheels, struct Angle *angle);
void Wheels_prepareSpeedCommand(struct Wheels *wheels, struct Coordinates *speeds_vector);
void Wheels_prepareTranslationCommand(struct Wheels *wheels, struct Coordinates *translation_vector, struct Angle *current_angle);

#endif // WHEELS_H_
