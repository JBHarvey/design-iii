#include <criterion/criterion.h>
#include <stdio.h>
#include "Angle.h"

Test(Angle, creation_destruction)
{
    const int halfPi = 157080;
    struct Angle* angle = Angle_new(halfPi);

    cr_assert(angle->theta == halfPi);
    Angle_delete(angle);
}
