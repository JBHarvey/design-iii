#include <criterion/criterion.h>
#include <stdio.h>
#include "Wheels.h"

struct Wheels *wheels;

void setup_wheels(void)
{
    wheels = Wheels_new();
}

void teardown_wheels(void)
{
    Wheels_delete(wheels);
}

Test(Wheels, creation_destruction
     , .init = setup_wheels
     , .fini = teardown_wheels)
{
    cr_assert(wheels->sensor->has_received_new_data == 0);
}
