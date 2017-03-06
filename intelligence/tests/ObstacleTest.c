#include <criterion/criterion.h>
#include <stdio.h>
#include "Obstacle.h"

const int OBSTACLE_X = 25000;
const int OBSTACLE_Y = 30000;

struct Obstacle *centerObstacle;
struct Obstacle *northOrientedObstacle;
struct Obstacle *southOrientedObstacle;

void setupObstacle(void)
{
    centerObstacle = Obstacle_new(OBSTACLE_X, OBSTACLE_Y, THEORICAL_OBSTACLE_RADIUS, CENTER);
    northOrientedObstacle = Obstacle_new(OBSTACLE_X, OBSTACLE_Y, THEORICAL_OBSTACLE_RADIUS, NORTH);
    southOrientedObstacle = Obstacle_new(OBSTACLE_X, OBSTACLE_Y, THEORICAL_OBSTACLE_RADIUS, SOUTH);
}

void teardownObstacle(void)
{
    Obstacle_delete(centerObstacle);
    Obstacle_delete(northOrientedObstacle);
    Obstacle_delete(southOrientedObstacle);
}

Test(Obstacle, creation_destruction
     , .init = setupObstacle
     , .fini = teardownObstacle)
{
    cr_assert(
        centerObstacle->coordinates->x == OBSTACLE_X &&
        centerObstacle->coordinates->y == OBSTACLE_Y &&
        centerObstacle->radius == THEORICAL_OBSTACLE_RADIUS &&
        centerObstacle->orientation == CENTER);
}

Test(Obstacle, given_newCoordinates_when_changingCoordinates_then_theyAreChanged
     , .init = setupObstacle
     , .fini = teardownObstacle)
{
    struct Coordinates *newCoordinates = Coordinates_new(OBSTACLE_Y, OBSTACLE_X);
    Obstacle_changeCoordinates(centerObstacle, newCoordinates);
    cr_assert(
        centerObstacle->coordinates->x == OBSTACLE_Y &&
        centerObstacle->coordinates->y == OBSTACLE_X);
    Coordinates_delete(newCoordinates);
}

Test(Obstacle, given_newOrientation_when_changingOrientation_then_itIsChanged
     , .init = setupObstacle
     , .fini = teardownObstacle)
{
    enum CardinalDirection newOrientation = NORTH;
    Obstacle_changeOrientation(centerObstacle, newOrientation);
    cr_assert(centerObstacle->orientation == newOrientation);
}
