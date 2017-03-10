#ifndef WHEELS_H_
#define WHEELS_H_

#include "Sensor.h"

struct Wheels {
    struct Object *object;
    struct Sensor *sensor;
};

struct Wheels *Wheels_new(void);
void Wheels_delete(struct Wheels *wheels);

#endif // WHEELS_H_
