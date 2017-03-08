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

#endif // OBSTACLE_H_
