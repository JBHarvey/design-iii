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

Test(Angle, given_aValueBiggerThanPi_when_created_then_valueIsWrappedBetweenPiAndMinusPi)
{
    const int fivePi = 1570795;
    struct Angle* angle = Angle_new(fivePi);

    cr_assert(angle->theta == PI);
    Angle_delete(angle);
}

Test(Angle, given_aValueSmallerThanMinusPi_when_created_then_valueIsWrappedBetweenPiAndMinusPi)
{
    const int minusThreePi = -942477;
    struct Angle* angle = Angle_new(minusThreePi);

    cr_assert(angle->theta == MINUS_PI);
    Angle_delete(angle);
}
