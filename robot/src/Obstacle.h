#ifndef OBSTACLE_H_
#define OBSTACLE_H_

#include "Coordinates.h"

#define THEORICAL_OBSTACLE_RADIUS 625  /* Unit: 0.1 mm */

struct Obstacle {
    struct Object *object;
    struct Coordinates *coordinates;
    int radius;
    enum CardinalDirection orientation;
};

struct Obstacle *Obstacle_new(int newX, int newY, int new_radius, enum CardinalDirection new_orientation);
void Obstacle_delete(struct Obstacle *obstacle);

void Obstacle_changeCoordinates(struct Obstacle *obstacle, struct Coordinates *new_coordinates);
void Obstacle_changeOrientation(struct Obstacle *obstacle, enum CardinalDirection new_orientation);

struct Coordinates *Obstacle_retrieveEasternPointOf(struct Obstacle *obstacle);
struct Coordinates *Obstacle_retrieveWesternPointOf(struct Obstacle *obstacle);
struct Coordinates *Obstacle_retrieveNorthernPointOf(struct Obstacle *obstacle);
struct Coordinates *Obstacle_retrieveSouthernPointOf(struct Obstacle *obstacle);

struct Obstacle *Obstacle_retrieveEastern(struct Obstacle *a, struct Obstacle *b);
struct Obstacle *Obstacle_retrieveWestern(struct Obstacle *a, struct Obstacle *b);

int Obstacle_areFacing(struct Obstacle *a, struct Obstacle *b);
#endif // OBSTACLE_H_
