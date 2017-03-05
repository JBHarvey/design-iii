#include <criterion/criterion.h>
#include <stdio.h>
#include "Coordinates.h"

const int COORDINATES_X = 13256;
const int COORDINATES_Y = 521651;

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
