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
