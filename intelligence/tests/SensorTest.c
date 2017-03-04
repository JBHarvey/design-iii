#include <criterion/criterion.h>
#include <stdio.h>
#include "Sensor.h"

struct Sensor *sensor;

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
    cr_assert(sensor->hasReceivedNewData == 0);
}

Test(Sensor, given_aSensor_when_receivesNewData_then_hasReceivedNewDataIsTrue
     , .init = setupSensor
     , .fini = teardownSensor)
{
    Sensor_receivesData(sensor);
    cr_assert(sensor->hasReceivedNewData);
}

Test(Sensor, given_aSensor_when_dataIsRead_then_hasReceivedNewDataIsFalse
     , .init = setupSensor
     , .fini = teardownSensor)
{
    Sensor_receivesData(sensor);
    Sensor_readsData(sensor);
    cr_assert(sensor->hasReceivedNewData == 0);
}
