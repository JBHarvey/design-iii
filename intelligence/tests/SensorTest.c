#include <criterion/criterion.h>
#include <stdio.h>
#include "Sensor.h"

struct Sensor *sensor;

const int ZERO = 0;

void setupSensor(void)
{
    sensor = Sensor_new();
}

void teardownSensor(void)
{
    Sensor_delete(sensor);
}

Test(Sensor, creation_destruction
     , .init = setupSensor
     , .fini = teardownSensor)
{
    cr_assert(sensor->hasReceivedNewData == ZERO);
}
/*
Test(Sensor, given_anSensor_when_addOneReference_then_referenceCountGoesUpByOne
     , .init = setupSensor
     , .fini = teardownSensor)
{
    Sensor_addOneReference(sensor);
    cr_assert(sensor->referenceCount == ZERO + ONE);
}

Test(Sensor, given_anSensor_when_removeOneReference_then_referenceCountGoesDownByOne
     , .init = setupSensor
     , .fini = teardownSensor)
{
    Sensor_removeOneReference(sensor);
    cr_assert(sensor->referenceCount == ZERO - ONE);
}

Test(Sensor, given_anSensorWithReferenceCountEqualToZero_when_checksIfCanBeDeleted_then_returnsTrue
     , .init = setupSensor
     , .fini = teardownSensor)
{
    Sensor_removeOneReference(sensor);
    int canBeDeleted = Sensor_canBeDeleted(sensor);
    cr_assert(canBeDeleted == ONE);
}
*/
