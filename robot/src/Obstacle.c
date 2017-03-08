#include <stdlib.h>
#include <stdio.h>
#include "Obstacle.h"

struct Obstacle *Obstacle_new(int newX, int newY, int new_radius, enum CardinalDirection new_orientation)
{
    struct Object *new_object = Object_new();
    struct Coordinates *new_coordinates = Coordinates_new(newX, newY);
    struct Obstacle *pointer = (struct Obstacle *) malloc(sizeof(struct Obstacle));

    pointer->object = new_object;
    pointer->coordinates = new_coordinates;
    pointer->radius = new_radius;
    pointer->orientation = new_orientation;

    return pointer;
}

void Obstacle_delete(struct Obstacle *obstacle)
{
    Object_removeOneReference(obstacle->object);

    if(Object_canBeDeleted(obstacle->object)) {
        Object_delete(obstacle->object);
        Coordinates_delete(obstacle->coordinates);
        free(obstacle);
    }
}

void Obstacle_changeCoordinates(struct Obstacle *obstacle, struct Coordinates *new_coordinates)
{
    Coordinates_copyValuesFrom(obstacle->coordinates, new_coordinates);
}

void Obstacle_changeOrientation(struct Obstacle *obstacle, enum CardinalDirection new_orientation)
{
    obstacle->orientation = new_orientation;
}
