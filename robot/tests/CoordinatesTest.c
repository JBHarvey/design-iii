#include <criterion/criterion.h>
#include <stdio.h>
#include <math.h>
#include "Coordinates.h"
#include "Angle.h"


const int COORDINATES_X = 1000;
const int COORDINATES_Y = 2500;
const int COORDINATES_DELTA = 42;

struct Coordinates *coordinates;
struct Coordinates *other_coordinates;

void setup_coordinates(void)
{
    coordinates = Coordinates_new(COORDINATES_X, COORDINATES_Y);
    other_coordinates = Coordinates_new(COORDINATES_X, COORDINATES_Y);
}

void teardown_coordinates(void)
{
    Coordinates_delete(coordinates);
    Coordinates_delete(other_coordinates);
}

Test(Coordinates, given__when_createsCoordinatesZero_then_coordinatesHasAllZeroValues)
{
    struct Coordinates *coordinates_zero = Coordinates_zero();
    cr_assert(
        coordinates_zero->x == 0 &&
        coordinates_zero->y == 0);
    Coordinates_delete(coordinates_zero);
}
Test(Coordinates, creation_destruction
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    cr_assert(
        coordinates->x == COORDINATES_X &&
        coordinates->y == COORDINATES_Y);
}

Test(Coordinates, given_twoCoordinatesWithSameValues_when_comparesThem_then_returnsTrue
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    int comparison = Coordinates_haveTheSameValues(coordinates, other_coordinates);
    cr_assert(comparison == 1);
}

Test(Coordinates, given_twoCoordinatesWithDifferentValues_when_comparesThem_then_returnsFalse
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_Y;
    other_coordinates->y = COORDINATES_X;
    int comparison = Coordinates_haveTheSameValues(coordinates, other_coordinates);
    cr_assert(comparison == 0);
}

Test(Coordinates, given_twoCoordinatesWithDifferentValues_when_copied_then_theCoordinatesHaveTheSameValues
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_Y;

    Coordinates_copyValuesFrom(coordinates, other_coordinates);

    int comparison = Coordinates_haveTheSameValues(coordinates, other_coordinates);
    cr_assert(comparison == 1);
}

void assert_cardinal_direction_from_coordinate_to_other_coordinate_is(enum CardinalDirection expected)
{
    enum CardinalDirection actual = Coordinates_cardinalDirectionTo(coordinates, other_coordinates);
    cr_assert_eq(expected, actual,
                 "Expected (%d,%d) to be %d of (%d,%d). Actual: %d \n (N:%d E:%d S:%d W:%d CENTER: %d NE:%d SE:%d SW:%d NW:%d)\n",
                 other_coordinates->x, other_coordinates->y, expected, coordinates->x, coordinates->y, actual,
                 NORTH, EAST, SOUTH, WEST, CENTER, NORTHEAST, SOUTHEAST, SOUTHWEST, NORTHWEST);
}

Test(Coordinates, given_twoIdenticalCoordinates_when_checksCardinalDirection_then_isCenter
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    assert_cardinal_direction_from_coordinate_to_other_coordinate_is(CENTER);
}

Test(Coordinates, given_aCoordinatesWithBiggerYValue_when_checksCardinalDirection_then_isNorth
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->y = COORDINATES_Y + COORDINATES_DELTA;
    assert_cardinal_direction_from_coordinate_to_other_coordinate_is(NORTH);
}

Test(Coordinates, given_aCoordinatesWithBiggerXValue_when_checksCardinalDirection_then_isEast
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_X + COORDINATES_DELTA;
    assert_cardinal_direction_from_coordinate_to_other_coordinate_is(EAST);
}

Test(Coordinates, given_aCoordinatesWithSmallerYValue_when_checksCardinalDirection_then_isSouth
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->y = COORDINATES_Y - COORDINATES_DELTA;
    assert_cardinal_direction_from_coordinate_to_other_coordinate_is(SOUTH);
}

Test(Coordinates, given_aCoordinatesWithSmallerXValue_when_checksCardinalDirection_then_isWest
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_X - COORDINATES_DELTA;
    assert_cardinal_direction_from_coordinate_to_other_coordinate_is(WEST);
}

Test(Coordinates, given_aCoordinatesWithBiggerXAndBiggerYValue_when_checksCardinalDirection_then_isNorthEast,
     .init = setup_coordinates
             , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_X + COORDINATES_DELTA;
    other_coordinates->y = COORDINATES_Y + COORDINATES_DELTA;
    assert_cardinal_direction_from_coordinate_to_other_coordinate_is(NORTHEAST);
}

Test(Coordinates, given_aCoordinatesWithBiggerXandSmallerYValue_when_checksCardinalDirection_then_isSouthEast
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_X + COORDINATES_DELTA;
    other_coordinates->y = COORDINATES_Y - COORDINATES_DELTA;
    assert_cardinal_direction_from_coordinate_to_other_coordinate_is(SOUTHEAST);
}

Test(Coordinates, given_aCoordinatesWithSmallerXAndSmallerYValue_when_checksCardinalDirection_then_isSouthWest
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_X - COORDINATES_DELTA;
    other_coordinates->y = COORDINATES_Y - COORDINATES_DELTA;
    assert_cardinal_direction_from_coordinate_to_other_coordinate_is(SOUTHWEST);
}

Test(Coordinates, given_aCoordinatesWithSmallerXAndBiggerYValue_when_checksCardinalDirection_then_isNorthWest
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_X - COORDINATES_DELTA;
    other_coordinates->y = COORDINATES_Y + COORDINATES_DELTA;
    assert_cardinal_direction_from_coordinate_to_other_coordinate_is(NORTHWEST);
}

Test(Coordinates,
     given_twoCoordinatesWithDifferentYValue_when_computeDistanceBetween_then_returnsTheCorrectDistanceBetween
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->y = COORDINATES_Y + COORDINATES_DELTA;
    int distance = Coordinates_distanceBetween(coordinates, other_coordinates);
    cr_assert(distance == COORDINATES_DELTA,
              "Expected the distance to be: %d. It returned: %d"
              , COORDINATES_DELTA, distance);
}

Test(Coordinates,
     given_twoCoordinatesWithDifferentXAndYValues_when_computeDistanceBetween_then_returnsTheCorrectDistanceBetween
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_X + COORDINATES_DELTA;
    other_coordinates->y = COORDINATES_Y + COORDINATES_DELTA;
    int expected = sqrt(2 * pow(COORDINATES_DELTA, 2));
    int distance = Coordinates_distanceBetween(coordinates, other_coordinates);
    cr_assert(distance == expected,
              "Expected the distance to be: %d. It returned: %d"
              , expected, distance);
}

Test(Coordinates,
     given_aCoordinates_when_computeDistanceFromOrigin_then_returnsTheCorrectDistanceBetweenTheCoordinatesAndTheOrigin
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    struct Coordinates *origin = Coordinates_zero();
    int distance_x = abs(origin->x - coordinates->x);
    int distance_y = abs(origin->y - coordinates->y);
    int expected = sqrt(pow(distance_x, 2) + pow(distance_y, 2));
    int distance = Coordinates_distanceFromOrigin(coordinates);
    cr_assert(distance == expected,
              "Expected the distance to be: %d. It returned: %d"
              , expected, distance);
    Coordinates_delete(origin);
}

void assertRotationResultIs(int initial_x, int initial_y, int origin_x, int origin_y, int expected_x, int expected_y,
                            int theta)
{
    cr_assert((other_coordinates->x == expected_x ||
               other_coordinates->x == expected_x + 1 ||
               other_coordinates->x == expected_x - 1)
              &&
              (other_coordinates->y == expected_y ||
               other_coordinates->y == expected_y + 1 ||
               other_coordinates->y == expected_y - 1)
              ,
              "Rotation of (%d,%d) of %d rad around (%d,%d) -> returns: (%d,%d) - expected: (%d,%d)",
              initial_x, initial_y, theta, origin_x, origin_y,
              other_coordinates->x, other_coordinates->y,
              expected_x, expected_y);
}

Test(Coordinates,
     given_twoCoordinatesAndAnAngle_when_rotatesCoordinatesOfAngle_thenTheSecondCoordinatesRotatesWithTheFirstOneAsOrigin
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    struct Angle *angle = Angle_new(HALF_PI);
    int initial_x = COORDINATES_X + COORDINATES_DELTA;
    int initial_y = COORDINATES_Y;
    other_coordinates->x = initial_x;
    other_coordinates->y = initial_y;

    Coordinates_rotateOfAngleAround(other_coordinates, angle, coordinates);
    int expected_x = COORDINATES_X;
    int expected_y = COORDINATES_Y + COORDINATES_DELTA;

    assertRotationResultIs(initial_x, initial_y, coordinates->x, coordinates->y, expected_x, expected_y, angle->theta);

    Angle_delete(angle);
}

Test(Coordinates,
     given_twoCoordinatesWhichAlreadyHaveAnAngleBetweenThem_when_rotatesCoordinatesOfAngle_thenTheSecondCoordinatesRotatesWithTheFirstOneAsOrigin
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    struct Angle *angle = Angle_new(HALF_PI);
    int initial_x = COORDINATES_X;
    int initial_y = COORDINATES_Y + COORDINATES_DELTA;
    other_coordinates->x = initial_x;
    other_coordinates->y = initial_y;

    Coordinates_rotateOfAngleAround(other_coordinates, angle, coordinates);
    int expected_x = COORDINATES_X - COORDINATES_DELTA;
    int expected_y = COORDINATES_Y;

    assertRotationResultIs(initial_x, initial_y, coordinates->x, coordinates->y, expected_x, expected_y, angle->theta);

    Angle_delete(angle);
}

Test(Coordinates,
     given_twoCoordinatesAndANegativeAngle_when_rotatesCoordinatesOfAngle_thenTheSecondCoordinatesRotatesWithTheFirstOneAsOrigin
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    struct Angle *angle = Angle_new(MINUS_HALF_PI);
    int initial_x = COORDINATES_X + COORDINATES_DELTA;
    int initial_y = COORDINATES_Y;
    other_coordinates->x = initial_x;
    other_coordinates->y = initial_y;

    Coordinates_rotateOfAngleAround(other_coordinates, angle, coordinates);
    int expected_x = COORDINATES_X;
    int expected_y = COORDINATES_Y - COORDINATES_DELTA;

    assertRotationResultIs(initial_x, initial_y, coordinates->x, coordinates->y, expected_x, expected_y, angle->theta);

    Angle_delete(angle);
}

Test(Coordinates,
     given_coordinatesAndAnAngle_when_rotatesCoordinatesOfAngle_thenRotatesItWithZeroAsOrigin
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    struct Angle *angle = Angle_new(MINUS_HALF_PI);
    int initial_x = COORDINATES_X;
    int initial_y = COORDINATES_Y;
    other_coordinates->x = initial_x;
    other_coordinates->y = initial_y;

    Coordinates_rotateOfAngle(other_coordinates, angle);
    int expected_x = COORDINATES_Y;
    int expected_y = 0 - COORDINATES_X;

    assertRotationResultIs(initial_x, initial_y, 0, 0, expected_x, expected_y, angle->theta);

    Angle_delete(angle);
}

Test(Coordinates, given_twoCoordinates_when_askedToComputeTheirMeanX_then_theMeanOfTheirXIsReturned
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = other_coordinates->x + COORDINATES_DELTA;
    int computed_mean = Coordinates_computeMeanX(coordinates, other_coordinates);
    int expected_mean = (COORDINATES_X + COORDINATES_X + COORDINATES_DELTA) / 2;
    cr_assert_eq(computed_mean, expected_mean);
}

Test(Coordinates, given_twoCoordinates_when_askedToComputeTheirMeanY_then_theMeanOfTheirYIsReturned
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->y = other_coordinates->y + COORDINATES_DELTA;
    int computed_mean = Coordinates_computeMeanY(coordinates, other_coordinates);
    int expected_mean = (COORDINATES_Y + COORDINATES_Y + COORDINATES_DELTA) / 2;
    cr_assert_eq(computed_mean, expected_mean);
}

Test(Coordinates,
     given_coordinates_when_askedToTranslate_then_theOriginalCoordinatesValuesAreChangedByTheValuesOfThePassedOne
     , .init = setup_coordinates
     , .fini = teardown_coordinates)
{
    other_coordinates->x = COORDINATES_DELTA;
    other_coordinates->y = COORDINATES_DELTA;
    Coordinates_translateOf(coordinates, other_coordinates);

    int expected_x = COORDINATES_X + COORDINATES_DELTA;
    int expected_y = COORDINATES_Y + COORDINATES_DELTA;

    struct Coordinates *expected_coordinates = Coordinates_new(expected_x, expected_y);
    cr_assert(Coordinates_haveTheSameValues(coordinates, expected_coordinates));
    Coordinates_delete(expected_coordinates);
}
