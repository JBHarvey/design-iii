#ifndef SENSOR_H_
#define SENSOR_H_

#include "Object.h"

struct Sensor {
    struct Object *object;
    int hasReceivedNewData;
};

struct Sensor *Sensor_new(void);
void Sensor_delete(struct Sensor *sensor);

void Sensor_receivesData(struct Sensor *sensor);
void Sensor_readsData(struct Sensor *sensor);

#endif // SENSOR_H_
