#include <criterion/criterion.h>
#include <stdio.h>
#include "Coordinates.h"

const int COORDINATES_X = 1000;
const int COORDINATES_Y = 2500;
const int COORDINATES_DELTA = 42;

struct Coordinates *coordinates;
struct Coordinates *otherCoordinates;

void setupCoordinates(void)
{
    coordinates = Coordinates_new(COORDINATES_X, COORDINATES_Y);
    otherCoordinates = Coordinates_new(COORDINATES_X, COORDINATES_Y);
}

void teardownCoordinates(void)
{
    Coordinates_delete(coordinates);
    Coordinates_delete(otherCoordinates);
}

Test(Coordinates, given__when_createsCoordinatesZero_then_coordinatesHasAllZeroValues)
{
    struct Coordinates *coordinatesZero = Coordinates_zero();
    cr_assert(
        coordinatesZero->x == 0 &&
        coordinatesZero->y == 0);
    Coordinates_delete(coordinatesZero);
}
Test(Coordinates, creation_destruction
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    cr_assert(
        coordinates->x == COORDINATES_X &&
        coordinates->y == COORDINATES_Y);
}

Test(Coordinates, given_twoCoordinatesWithSameValues_when_comparesThem_then_returnsTrue
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    int comparison = Coordinates_haveTheSameValues(coordinates, otherCoordinates);
    cr_assert(comparison == 1);
}

Test(Coordinates, given_twoCoordinatesWithDifferentValues_when_comparesThem_then_returnsFalse
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->x = COORDINATES_Y;
    otherCoordinates->y = COORDINATES_X;
    int comparison = Coordinates_haveTheSameValues(coordinates, otherCoordinates);
    cr_assert(comparison == 0);
}

Test(Coordinates, given_twoCoordinatesWithDifferentValues_when_copied_then_theCoordinatesHaveTheSameValues
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->x = COORDINATES_Y;

    Coordinates_copyValuesFrom(coordinates, otherCoordinates);

    int comparison = Coordinates_haveTheSameValues(coordinates, otherCoordinates);
    cr_assert(comparison == 1);
}

void assertCardinalDirectionFromCoordinateToOtherCoordinateIs(enum CardinalDirection expected)
{
    enum CardinalDirection actual = Coordinates_cardinalDirectionTo(coordinates, otherCoordinates);
    cr_assert_eq(expected, actual,
                 "Expected (%d,%d) to be %d of (%d,%d). Actual: %d \n (N:%d E:%d S:%d W:%d CENTER: %d NE:%d SE:%d SW:%d NW:%d)\n",
                 otherCoordinates->x, otherCoordinates->y, expected, coordinates->x, coordinates->y, actual,
                 NORTH, EAST, SOUTH, WEST, CENTER, NORTHEAST, SOUTHEAST, SOUTHWEST, NORTHWEST);
}

Test(Coordinates, given_twoIdenticalCoordinates_when_checksCardinalDirection_then_isCenter
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    assertCardinalDirectionFromCoordinateToOtherCoordinateIs(CENTER);
}

Test(Coordinates, given_aCoordinateWithBiggerYValue_when_checksCardinalDirection_then_isNorth
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->y = COORDINATES_Y + COORDINATES_DELTA;
    assertCardinalDirectionFromCoordinateToOtherCoordinateIs(NORTH);
}

Test(Coordinates, given_aCoordinateWithBiggerXValue_when_checksCardinalDirection_then_isEast
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->x = COORDINATES_X + COORDINATES_DELTA;
    assertCardinalDirectionFromCoordinateToOtherCoordinateIs(EAST);
}

Test(Coordinates, given_aCoordinateWithSmallerYValue_when_checksCardinalDirection_then_isSouth
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->y = COORDINATES_Y - COORDINATES_DELTA;
    assertCardinalDirectionFromCoordinateToOtherCoordinateIs(SOUTH);
}

Test(Coordinates, given_aCoordinateWithSmallerXValue_when_checksCardinalDirection_then_isWest
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->x = COORDINATES_X - COORDINATES_DELTA;
    assertCardinalDirectionFromCoordinateToOtherCoordinateIs(WEST);
}

Test(Coordinates, given_aCoordinateWithBiggerXAndBiggerYValue_when_checksCardinalDirection_then_isNorthEast
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->x = COORDINATES_X + COORDINATES_DELTA;
    otherCoordinates->y = COORDINATES_Y + COORDINATES_DELTA;
    assertCardinalDirectionFromCoordinateToOtherCoordinateIs(NORTHEAST);
}

Test(Coordinates, given_aCoordinateWithBiggerXandSmallerYValue_when_checksCardinalDirection_then_isSouthEast
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->x = COORDINATES_X + COORDINATES_DELTA;
    otherCoordinates->y = COORDINATES_Y - COORDINATES_DELTA;
    assertCardinalDirectionFromCoordinateToOtherCoordinateIs(SOUTHEAST);
}

Test(Coordinates, given_aCoordinateWithSmallerXAndSmallerYValue_when_checksCardinalDirection_then_isSouthWest
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->x = COORDINATES_X - COORDINATES_DELTA;
    otherCoordinates->y = COORDINATES_Y - COORDINATES_DELTA;
    assertCardinalDirectionFromCoordinateToOtherCoordinateIs(SOUTHWEST);
}

Test(Coordinates, given_aCoordinateWithSmallerXAndBiggerYValue_when_checksCardinalDirection_then_isNorthWest
     , .init = setupCoordinates
     , .fini = teardownCoordinates)
{
    otherCoordinates->x = COORDINATES_X - COORDINATES_DELTA;
    otherCoordinates->y = COORDINATES_Y + COORDINATES_DELTA;
    assertCardinalDirectionFromCoordinateToOtherCoordinateIs(NORTHWEST);
}

