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

struct Coordinates *Obstacle_retrieveEasternPointOf(struct Obstacle *obstacle)
{
    int x = obstacle->coordinates->x + obstacle->radius;
    int y = obstacle->coordinates->y;
    struct Coordinates *eastern_point = Coordinates_new(x, y);
    return eastern_point;
}

struct Coordinates *Obstacle_retrieveWesternPointOf(struct Obstacle *obstacle)
{
    int x = obstacle->coordinates->x - obstacle->radius;
    int y = obstacle->coordinates->y;
    struct Coordinates *western_point = Coordinates_new(x, y);
    return western_point;
}

struct Coordinates *Obstacle_retrieveNorthernPointOf(struct Obstacle *obstacle)
{
    int x = obstacle->coordinates->x;
    int y = obstacle->coordinates->y + obstacle->radius;
    struct Coordinates *northern_point = Coordinates_new(x, y);
    return northern_point;
}

struct Coordinates *Obstacle_retrieveSouthernPointOf(struct Obstacle *obstacle)
{
    int x = obstacle->coordinates->x;
    int y = obstacle->coordinates->y - obstacle->radius;
    struct Coordinates *southern_point = Coordinates_new(x, y);
    return southern_point;
}

struct Obstacle *Obstacle_retrieveEastern(struct Obstacle *a, struct Obstacle *b)
{
    if(a->coordinates->x == b->coordinates->x && a->radius == b->radius) {
        if(a->coordinates->y > b->coordinates->y) {
            return a;
        } else {
            return b;
        }
    }

    struct Coordinates *eastern_point_of_a = Obstacle_retrieveEasternPointOf(a);

    struct Coordinates *eastern_point_of_b = Obstacle_retrieveEasternPointOf(b);

    struct Obstacle *eastern_obstacle;

    if(eastern_point_of_a->x > eastern_point_of_b->x) {
        eastern_obstacle = a;
    } else {
        eastern_obstacle = b;
    }

    Coordinates_delete(eastern_point_of_a);
    Coordinates_delete(eastern_point_of_b);

    return eastern_obstacle;
}

struct Obstacle *Obstacle_retrieveWestern(struct Obstacle *a, struct Obstacle *b)
{
    if(a->coordinates->x == b->coordinates->x && a->radius == b->radius) {
        if(a->coordinates->y < b->coordinates->y) {
            return a;
        } else {
            return b;
        }
    }

    struct Coordinates *western_point_of_a = Obstacle_retrieveWesternPointOf(a);

    struct Coordinates *western_point_of_b = Obstacle_retrieveWesternPointOf(b);

    struct Obstacle *western_obstacle;

    if(western_point_of_a->x < western_point_of_b->x) {
        western_obstacle = a;
    } else {
        western_obstacle = b;
    }

    Coordinates_delete(western_point_of_a);
    Coordinates_delete(western_point_of_b);

    return western_obstacle;
}

int Obstacle_areFacing(struct Obstacle *a, struct Obstacle *b)
{
    int horizontal_distance = abs(a->coordinates->x - b->coordinates->x);
    int radius_total = a->radius + b->radius;
    return horizontal_distance <= radius_total;
}
