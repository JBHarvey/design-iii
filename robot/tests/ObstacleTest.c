#include <criterion/criterion.h>
#include <stdio.h>
#include "Obstacle.h"

const int OBSTACLE_X = 25000;
const int OBSTACLE_Y = 30000;

struct Obstacle *center_obstacle;
struct Obstacle *north_oriented_obstacle;
struct Obstacle *south_oriented_obstacle;

void setup_Obstacle(void)
{
    center_obstacle = Obstacle_new(OBSTACLE_X, OBSTACLE_Y, THEORICAL_OBSTACLE_RADIUS, CENTER);
    north_oriented_obstacle = Obstacle_new(OBSTACLE_X, OBSTACLE_Y, THEORICAL_OBSTACLE_RADIUS, NORTH);
    south_oriented_obstacle = Obstacle_new(OBSTACLE_X, OBSTACLE_Y, THEORICAL_OBSTACLE_RADIUS, SOUTH);
}

void teardown_Obstacle(void)
{
    Obstacle_delete(center_obstacle);
    Obstacle_delete(north_oriented_obstacle);
    Obstacle_delete(south_oriented_obstacle);
}

Test(Obstacle, creation_destruction
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    cr_assert(
        center_obstacle->coordinates->x == OBSTACLE_X &&
        center_obstacle->coordinates->y == OBSTACLE_Y &&
        center_obstacle->radius == THEORICAL_OBSTACLE_RADIUS &&
        center_obstacle->orientation == CENTER);
}

Test(Obstacle, given_newCoordinates_when_changingCoordinates_then_theyAreChanged
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    struct Coordinates *new_coordinates = Coordinates_new(OBSTACLE_Y, OBSTACLE_X);
    Obstacle_changeCoordinates(center_obstacle, new_coordinates);
    cr_assert(
        center_obstacle->coordinates->x == OBSTACLE_Y &&
        center_obstacle->coordinates->y == OBSTACLE_X);
    Coordinates_delete(new_coordinates);
}

Test(Obstacle, given_newOrientation_when_changingOrientation_then_itIsChanged
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    enum CardinalDirection new_orientation = NORTH;
    Obstacle_changeOrientation(center_obstacle, new_orientation);
    cr_assert(center_obstacle->orientation == new_orientation);
}

Test(Obstacle,
     given__when_askedToRetrieveTheEasternPointOnTheObstacle_then_theCoordinatesOfTheObstacleAretReturnedPlusItsRadiusInX
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    struct Coordinates *retrieved_coordinates = Obstacle_retrieveEasternPointOf(center_obstacle);

    struct Coordinates *expected_coordinates = Coordinates_new(OBSTACLE_X + THEORICAL_OBSTACLE_RADIUS, OBSTACLE_Y);

    cr_assert(Coordinates_haveTheSameValues(retrieved_coordinates, expected_coordinates));

    Coordinates_delete(retrieved_coordinates);
    Coordinates_delete(expected_coordinates);
}

Test(Obstacle,
     given__when_askedToRetrieveTheWesternPointOnTheObstacle_then_theCoordinatesOfTheObstacleAretReturnedMinusItsRadiusInX
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    struct Coordinates *retrieved_coordinates = Obstacle_retrieveWesternPointOf(center_obstacle);

    struct Coordinates *expected_coordinates = Coordinates_new(OBSTACLE_X - THEORICAL_OBSTACLE_RADIUS, OBSTACLE_Y);

    cr_assert(Coordinates_haveTheSameValues(retrieved_coordinates, expected_coordinates));

    Coordinates_delete(retrieved_coordinates);
    Coordinates_delete(expected_coordinates);
}

Test(Obstacle,
     given__when_askedToRetrieveTheNorthernPointOnTheObstacle_then_theCoordinatesOfTheObstacleAretReturnedPlusItsRadiusInY
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    struct Coordinates *retrieved_coordinates = Obstacle_retrieveNorthernPointOf(center_obstacle);

    struct Coordinates *expected_coordinates = Coordinates_new(OBSTACLE_X, OBSTACLE_Y + THEORICAL_OBSTACLE_RADIUS);

    cr_assert(Coordinates_haveTheSameValues(retrieved_coordinates, expected_coordinates));

    Coordinates_delete(retrieved_coordinates);
    Coordinates_delete(expected_coordinates);
}

Test(Obstacle,
     given__when_askedToRetrieveTheSouthernPointOnTheObstacle_then_theCoordinatesOfTheObstacleAretReturnedMinusItsRadiusInY
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    struct Coordinates *retrieved_coordinates = Obstacle_retrieveSouthernPointOf(center_obstacle);

    struct Coordinates *expected_coordinates = Coordinates_new(OBSTACLE_X, OBSTACLE_Y - THEORICAL_OBSTACLE_RADIUS);

    cr_assert(Coordinates_haveTheSameValues(retrieved_coordinates, expected_coordinates));

    Coordinates_delete(retrieved_coordinates);
    Coordinates_delete(expected_coordinates);
}

Test(Obstacle,
     given_twoObstacleWithTheSameXCoordinatesAndRadius_when_askedToRetrieveTheEasternOne_then_theNorthenOneIsReturned
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    south_oriented_obstacle->coordinates->y = OBSTACLE_Y + THEORICAL_OBSTACLE_RADIUS;
    struct Obstacle *eastern = Obstacle_retrieveEastern(north_oriented_obstacle, south_oriented_obstacle);
    cr_assert(eastern == south_oriented_obstacle);
}

Test(Obstacle,
     given_twoObstacle_when_askedToRetrieveTheEasternOne_then_theOneWithTheHighestXValueOfEasternCoordinatesIsReturned
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    south_oriented_obstacle->coordinates->x = OBSTACLE_X * 2;
    struct Obstacle *eastern = Obstacle_retrieveEastern(north_oriented_obstacle, south_oriented_obstacle);
    cr_assert(eastern == south_oriented_obstacle);
}

Test(Obstacle,
     given_twoObstacleWithTheSameXCoordinatesAndRadius_when_askedToRetrieveTheWesternOne_then_nullIsReturned
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    north_oriented_obstacle->coordinates->y = OBSTACLE_Y - THEORICAL_OBSTACLE_RADIUS;
    struct Obstacle *western = Obstacle_retrieveWestern(north_oriented_obstacle, south_oriented_obstacle);
    cr_assert(western == north_oriented_obstacle);
}

Test(Obstacle,
     given_twoObstacle_when_askedToRetrieveTheWesternOne_then_theOneWithTheLowestXValueOfWesternCoordinatesIsReturned
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    south_oriented_obstacle->coordinates->x = OBSTACLE_X / 2;
    struct Obstacle *western = Obstacle_retrieveWestern(north_oriented_obstacle, south_oriented_obstacle);
    cr_assert(western == south_oriented_obstacle);
}

Test(Obstacle,
     given_twoObstacleSeparatedHorizontallyByASmallerDistanceThanTheAdditionOfTheirRadius_when_askedIfTheyAreOverlappingEachOtherInX_then_returnsOne
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    int areOverlapping = Obstacle_areOverlappingInX(center_obstacle, south_oriented_obstacle);
    cr_assert(areOverlapping);
}

Test(Obstacle,
     given_twoObstacleSeparatedHorizontallyByTheExactDistanceOfTheAdditionOfTheirRadius_when_askedIfTheyAreOverlappingEachOtherInX_then_returnsOne
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    center_obstacle->coordinates->x = OBSTACLE_X + 2 * THEORICAL_OBSTACLE_RADIUS;
    int areOverlapping = Obstacle_areOverlappingInX(center_obstacle, south_oriented_obstacle);
    cr_assert(areOverlapping);
}

Test(Obstacle,
     given_twoObstacleSeparatedHorizontallyByABiggerDistanceOfTheAdditionOfTheirRadius_when_askedIfTheyAreOverlappingEachOtherInX_then_returnsZero
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    center_obstacle->coordinates->x = OBSTACLE_X + 3 * THEORICAL_OBSTACLE_RADIUS;
    int areOverlapping = Obstacle_areOverlappingInX(center_obstacle, south_oriented_obstacle);
    cr_assert(!areOverlapping);
}

Test(Obstacle,
     given_twoObstacleSeparatedVerticallyByASmallerDistanceThanTheAdditionOfTheirRadius_when_askedIfTheyAreOverlappingEachOtherInY_then_returnsOne
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    int areOverlapping = Obstacle_areOverlappingInY(center_obstacle, south_oriented_obstacle);
    cr_assert(areOverlapping);
}

Test(Obstacle,
     given_twoObstacleSeparatedVerticallyByTheExactDistanceOfTheAdditionOfTheirRadius_when_askedIfTheyAreOverlappingEachOtherInY_then_returnsOne
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    center_obstacle->coordinates->y = OBSTACLE_Y + 2 * THEORICAL_OBSTACLE_RADIUS;
    int areOverlapping = Obstacle_areOverlappingInY(center_obstacle, south_oriented_obstacle);
    cr_assert(areOverlapping);
}

Test(Obstacle,
     given_twoObstacleSeparatedVerticallyByABiggerDistanceOfTheAdditionOfTheirRadius_when_askedIfTheyAreOverlappingEachOtherInY_then_returnsZero
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    center_obstacle->coordinates->y = OBSTACLE_Y + 3 * THEORICAL_OBSTACLE_RADIUS;
    int areOverlapping = Obstacle_areOverlappingInY(center_obstacle, south_oriented_obstacle);
    cr_assert(!areOverlapping);
}

Test(Obstacle,
     given_twoObstacle_when_askedToRetrieveTheNorthernOne_then_theOneWithTheHighestYValueOfNorthernCoordinatesIsReturned
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    south_oriented_obstacle->coordinates->y = OBSTACLE_Y * 2;
    struct Obstacle *northern = Obstacle_retrieveNorthern(north_oriented_obstacle, south_oriented_obstacle);
    cr_assert(northern == south_oriented_obstacle);
}

Test(Obstacle,
     given_twoObstacle_when_askedToRetrieveTheSouthernOne_then_theOneWithTheLowestYValueOfNorthernCoordinatesIsReturned
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    south_oriented_obstacle->coordinates->y = OBSTACLE_Y * 2;
    struct Obstacle *southern = Obstacle_retrieveSouthern(north_oriented_obstacle, south_oriented_obstacle);
    cr_assert(southern == north_oriented_obstacle);
}

Test(Obstacle, given_coordinatesFarAwayFromTheObstacle_when_askedIfTheCoordinateIsFree_then_itIs
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    struct Coordinates *zero = Coordinates_zero();
    int is_free = Obstacle_isCoordinateFree(center_obstacle, zero);
    cr_assert(is_free);
    Coordinates_delete(zero);
}

Test(Obstacle, given_theObstacleCoordinates_when_askedIfTheCoordinateIsFree_then_itIsNot
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    struct Coordinates *coordinates = Coordinates_new(OBSTACLE_X, OBSTACLE_Y);
    int is_free = Obstacle_isCoordinateFree(center_obstacle, coordinates);
    cr_assert(!is_free);
    Coordinates_delete(coordinates);
}

Test(Obstacle,
     given_coordinatesAnObstacleRadiusDistanceFromTheObstacleCoordinates_when_askedIfTheCoordinateIsFree_then_itIsNot
     , .init = setup_Obstacle
     , .fini = teardown_Obstacle)
{
    int radius = center_obstacle->radius;
    struct Coordinates *coordinates = Coordinates_new(OBSTACLE_X + radius, OBSTACLE_Y);
    int is_free = Obstacle_isCoordinateFree(center_obstacle, coordinates);
    cr_assert(!is_free);
    Coordinates_delete(coordinates);
}
