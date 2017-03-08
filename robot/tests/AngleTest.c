#include <criterion/criterion.h>
#include <stdio.h>
#include "Angle.h"

Test(Angle, creation_destruction)
{
    int value = PI / 2;
    struct Angle *angle = Angle_new(value);

    cr_assert(angle->theta == value);
    Angle_delete(angle);
}

Test(Angle, given_minusPi_when_created_then_valueIsPi)
{
    struct Angle *angle = Angle_new(MINUS_PI);

    cr_assert_eq(angle->theta, PI);
    Angle_delete(angle);
}

Test(Angle, given_aValueBiggerThanPi_when_created_then_valueIsWrappedBetweenPiAndMinusPi)
{
    const int FIVE_PI = 5 * PI;
    struct Angle *angle = Angle_new(FIVE_PI);

    cr_assert(angle->theta == PI);
    Angle_delete(angle);
}

Test(Angle, given_aValueSmallerThanMinusPi_when_created_then_valueIsWrappedBetweenPiAndMinusPi)
{
    const int MINUS_THREE_PI = -3 * PI;
    struct Angle *angle = Angle_new(MINUS_THREE_PI);

    cr_assert_eq(angle->theta, PI);
    Angle_delete(angle);
}

void assert_difference_between_angles_is(int alpha_value, int beta_value, int expected_result)
{
    struct Angle *alpha = Angle_new(alpha_value);
    struct Angle *beta = Angle_new(beta_value);

    int between_angle = Angle_smallestAngleBetween(alpha, beta);

    cr_assert_eq(between_angle, expected_result, "Expected (Alpha: %d <-> Beta: %d) -->  %d, got %d"
                 , alpha->theta, beta->theta, expected_result,  between_angle);
    Angle_delete(alpha);
    Angle_delete(beta);
}

Test(Angle, given_PiAndMinusPi_when_askedForDistance_returnsZero)
{
    assert_difference_between_angles_is(PI, MINUS_PI, 0);
}

Test(Angle_Difference, given_twoDifferentPositiveAngles_when_askedForDistance_returnsBiggestMinusSmallest)
{
    assert_difference_between_angles_is(PI / 2, PI / 3, PI / 6);
    assert_difference_between_angles_is(PI / 3, PI / 2, PI / 6);
}

Test(Angle_Difference, given_twoDifferentNegativeAngles_when_askedForDistance_returnsBiggestMinusSmallest)
{
    assert_difference_between_angles_is(-3 * PI / 4, -7 * PI / 8, PI / 8);
    assert_difference_between_angles_is(-7 * PI / 8, -3 * PI / 4, PI / 8);
}

Test(Angle_Difference, given_twoDifferentPolaritiesAngles_when_askedForDistance_returnsWrappedAddition)
{
    assert_difference_between_angles_is(-PI / 2, PI / 2, PI);
    assert_difference_between_angles_is(PI / 4, -PI / 3, 7 * PI / 12);
    assert_difference_between_angles_is(3 * PI / 4, -4 * PI / 5, 9 * PI / 20);
}

void assert_rotation_direction_between_angles_is(int goal_value, int current_value, enum RotationDirection expected_result)
{
    struct Angle *goal = Angle_new(goal_value);
    struct Angle *current = Angle_new(current_value);

    enum RotationDirection suggested_direction = Angle_fetchOptimalRotationDirection(goal, current);

    cr_assert(suggested_direction == expected_result, "Rotating from %d to %d in %d direction (expected %d)",
              current_value, goal_value, expected_result, suggested_direction);
    Angle_delete(goal);
    Angle_delete(current);
}

Test(Angle, given_twoAnglesWithNullDistance_when_askedWhichDirectionToTurnToBringThemCloser_then_returnsSTOP_TURNING)
{
    assert_rotation_direction_between_angles_is(PI, PI, STOP_TURNING);
}

Test(Angle_RotationDirection,
     given_currentWithinLessThanPiDegreesAntiClockwiseFromGoal_when_askedWhichDirectionToTurnToBringThemCloser_then_returnsANTICLOCKWISE)
{
    assert_rotation_direction_between_angles_is(PI, PI / 6, ANTICLOCKWISE);
    assert_rotation_direction_between_angles_is(0, 9 * PI / 8, ANTICLOCKWISE);
    assert_rotation_direction_between_angles_is(-3 * PI / 2, 3 * PI / 2, ANTICLOCKWISE);
    assert_rotation_direction_between_angles_is(PI / 12, PI / -6, ANTICLOCKWISE);
}

Test(Angle_RotationDirection,
     given_currentWithinMoreThanPiDegreesAntiClockwiseFromGoal_when_askedWhichDirectionToTurnToBringThemCloser_then_returnsCLOCKWISE)
{
    assert_rotation_direction_between_angles_is(PI / 6, PI, CLOCKWISE);
    assert_rotation_direction_between_angles_is(9 * PI / 8, 0, CLOCKWISE);
    assert_rotation_direction_between_angles_is(3 * PI / 4, -4 * PI / 5, CLOCKWISE);
    assert_rotation_direction_between_angles_is(PI / -6, PI / 12, CLOCKWISE);
}



