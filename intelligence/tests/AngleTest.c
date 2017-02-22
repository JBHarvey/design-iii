#include <criterion/criterion.h>
#include <stdio.h>
#include "Angle.h"

Test(Angle, creation_destruction)
{
    int value = PI / 2;
    struct Angle* angle = Angle_new(value);

    cr_assert(angle->theta == value);
    Angle_delete(angle);
}

Test(Angle, given_minusPi_when_created_then_valueIsPi)
{
    struct Angle * angle = Angle_new(MINUS_PI);

    cr_assert_eq(angle->theta, PI);
    Angle_delete(angle);
}

Test(Angle, given_aValueBiggerThanPi_when_created_then_valueIsWrappedBetweenPiAndMinusPi)
{
    const int fivePi = 5 * PI;
    struct Angle* angle = Angle_new(fivePi);

    cr_assert(angle->theta == PI);
    Angle_delete(angle);
}

Test(Angle, given_aValueSmallerThanMinusPi_when_created_then_valueIsWrappedBetweenPiAndMinusPi)
{
    const int minusThreePi = -3 * PI;
    struct Angle * angle = Angle_new(minusThreePi);

    cr_assert_eq(angle->theta, PI);
    Angle_delete(angle);
}

void assertDifferenceBetweenAnglesIs(int alphaValue, int betaValue, int expectedResult)
{
    struct Angle* alpha = Angle_new(alphaValue);
    struct Angle* beta = Angle_new(betaValue);

    int betweenAngle = Angle_smallestAngleBetween(alpha, beta);

    cr_assert_eq(betweenAngle, expectedResult, "Expected (Alpha: %d <-> Beta: %d) -->  %d, got %d"
                 , alpha->theta, beta->theta, expectedResult,  betweenAngle);
    Angle_delete(alpha);
    Angle_delete(beta);
}

Test(Angle, given_PiAndMinusPi_when_askedForDistance_returnsZero)
{
    assertDifferenceBetweenAnglesIs(PI, MINUS_PI, 0);
}

Test(Angle_Difference, given_twoDifferentPositiveAngles_when_askedForDistance_returnsBiggestMinusSmallest)
{
    assertDifferenceBetweenAnglesIs(PI / 2, PI / 3, PI / 6);
    assertDifferenceBetweenAnglesIs(PI / 3, PI / 2, PI / 6);
}

Test(Angle_Difference, given_twoDifferentNegativeAngles_when_askedForDistance_returnsBiggestMinusSmallest)
{
    assertDifferenceBetweenAnglesIs(-3 * PI / 4, -7 * PI / 8, PI / 8);
    assertDifferenceBetweenAnglesIs(-7 * PI / 8, -3 * PI / 4, PI / 8);
}

Test(Angle_Difference, given_twoDifferentPolaritiesAngles_when_askedForDistance_returnsWrappedAddition)
{
    assertDifferenceBetweenAnglesIs(-PI / 2, PI / 2, PI);
    assertDifferenceBetweenAnglesIs(PI / 4, -PI / 3, 7 * PI / 12);
    assertDifferenceBetweenAnglesIs(3 * PI / 4, -4 * PI / 5, 9 * PI / 20);
}

void assertRotationDirectionBetweenAnglesIs(int goalValue, int currentValue, enum RotationDirection expectedResult)
{
    struct Angle* goal = Angle_new(goalValue);
    struct Angle* current = Angle_new(currentValue);

    enum RotationDirection suggestedDirection = Angle_fetchRotationDirectionToReduceDistanceBetween(goal, current);

    cr_assert(suggestedDirection == expectedResult, "Rotating from %d to %d in %d direction (expected %d)",
              currentValue,              goalValue, expectedResult, suggestedDirection);
    Angle_delete(goal);
    Angle_delete(current);
}

Test(Angle, given_twoAnglesWithNullDistance_when_askedWhichDirectionToTurnToBringThemCloser_then_returnsSTOP_TURNING)
{
    assertRotationDirectionBetweenAnglesIs(PI, PI, STOP_TURNING);
}

Test(Angle_RotationDirection,
     given_currentWithinLessThanPiDegreesAntiClockwiseFromGoal_when_askedWhichDirectionToTurnToBringThemCloser_then_returnsANTICLOCKWISE)
{
    assertRotationDirectionBetweenAnglesIs(PI, PI / 6, ANTICLOCKWISE);
    assertRotationDirectionBetweenAnglesIs(0, 9 * PI / 8, ANTICLOCKWISE);
    assertRotationDirectionBetweenAnglesIs(-3 * PI / 2, 3 * PI / 2, ANTICLOCKWISE);
    assertRotationDirectionBetweenAnglesIs(PI / 12, PI / -6, ANTICLOCKWISE);
}

Test(Angle_RotationDirection,
     given_currentWithinMoreThanPiDegreesAntiClockwiseFromGoal_when_askedWhichDirectionToTurnToBringThemCloser_then_returnsCLOCKWISE)
{
    assertRotationDirectionBetweenAnglesIs(PI / 6, PI, CLOCKWISE);
    assertRotationDirectionBetweenAnglesIs(9 * PI / 8, 0, CLOCKWISE);
    assertRotationDirectionBetweenAnglesIs(3 * PI / 4, -4 * PI / 5, CLOCKWISE);
    assertRotationDirectionBetweenAnglesIs(PI / -6, PI / 12, CLOCKWISE);
}



