#ifndef WHEELS_H_
#define WHEELS_H_

#include "Sensor.h"
#include "Actuator.h"
#include "Pose.h"

struct Wheels {
    struct Object *object;
    struct Sensor *translation_sensor;
    struct Sensor *rotation_sensor;
    struct Actuator *translation_actuator;
    struct Actuator *rotation_actuator;
    struct Coordinates *translation_data;
    struct Angle *rotation_data;
    struct Coordinates *translation_command;
    struct Angle *rotation_command;
};

struct Wheels *Wheels_new(void);
void Wheels_delete(struct Wheels *wheels);

void Wheels_receiveData(struct Wheels *wheels, struct Pose *pose);
struct Angle *Wheels_readRotationData(struct Wheels *wheels);
struct Coordinates *Wheels_readTranslationData(struct Wheels *wheels, struct Angle *current_angle);

void Wheels_prepareRotationCommand(struct Wheels *wheels, struct Angle *angle);
void Wheels_prepareTranslationCommand(struct Wheels *wheels, struct Coordinates *translation_vector, struct Angle *current_angle);

#endif // WHEELS_H_
