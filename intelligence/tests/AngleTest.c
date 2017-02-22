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

Test(Angle, given_PiAndMinusPi_when_askedForDistance_returnsZero)
{
    struct Angle* pi = Angle_new(PI);
    struct Angle* minusPi = Angle_new(MINUS_PI);

    struct Angle* betweenAngle = Angle_smallestAngleBetween(pi, minusPi);

    cr_assert(betweenAngle->theta == 0);
}

//Test(Angle_Difference, given_twoPositiveAngles
