#ifndef ACTUATOR_H_
#define ACTUATOR_H_

#include "Object.h"

struct Actuator {
    struct Object *object;
    int has_prepared_new_command;
};

struct Actuator *Actuator_new(void);
void Actuator_delete(struct Actuator *actuator);

void Actuator_preparesCommand(struct Actuator *actuator);
void Actuator_sendsCommand(struct Actuator *actuator);

#endif // ACTUATOR_H_
