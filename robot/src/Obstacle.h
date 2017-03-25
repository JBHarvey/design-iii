#ifndef OBSTACLE_H_
#define OBSTACLE_H_

#include "Coordinates.h"

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
struct Obstacle *Obstacle_retrieveNorthern(struct Obstacle *a, struct Obstacle *b);
struct Obstacle *Obstacle_retrieveSouthern(struct Obstacle *a, struct Obstacle *b);

// TODO: Test these 2 functions and add them to the graph
//int Obstacle_areOverlapping(struct Obstacle *a, struct Obstacle *b);
int Obstacle_areOverlappingInX(struct Obstacle *a, struct Obstacle *b);
int Obstacle_areOverlappingInY(struct Obstacle *a, struct Obstacle *b);
int Obstacle_isCoordinateFree(struct Obstacle *obstacle, struct Coordinates *coordinates);
#endif // OBSTACLE_H_
