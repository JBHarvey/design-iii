#include <criterion/criterion.h>
#include <stdio.h>
#include "Sensor.h"

struct Sensor *sensor;

void setup_sensor(void)
{
    sensor = Sensor_new();
}

void teardown_sensor(void)
{
    Sensor_delete(sensor);
}

Test(Sensor, creation_destruction
     , .init = setup_sensor
     , .fini = teardown_sensor)
{
    cr_assert(sensor->has_received_new_data == 0);
}

Test(Sensor, given_aSensor_when_receivesNewData_then_hasReceivedNewDataIsTrue
     , .init = setup_sensor
     , .fini = teardown_sensor)
{
    Sensor_receivesData(sensor);
    cr_assert(sensor->has_received_new_data);
}

Test(Sensor, given_aSensor_when_dataIsRead_then_hasReceivedNewDataIsFalse
     , .init = setup_sensor
     , .fini = teardown_sensor)
{
    Sensor_receivesData(sensor);
    Sensor_readsData(sensor);
    cr_assert(sensor->has_received_new_data == 0);
}
