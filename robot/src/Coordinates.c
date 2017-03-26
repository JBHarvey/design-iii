#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Coordinates.h"


struct Coordinates *Coordinates_zero(void)
{
    return Coordinates_new(0, 0);
}

struct Coordinates *Coordinates_new(int newX, int newY)
{
    struct Object *new_object = Object_new();
    struct Coordinates *pointer = (struct Coordinates *) malloc(sizeof(struct Coordinates));

    pointer->object = new_object;

    pointer->x = newX;
    pointer->y = newY;

    return pointer;
}

void Coordinates_delete(struct Coordinates *coordinates)
{
    Object_removeOneReference(coordinates->object);

    if(Object_canBeDeleted(coordinates->object)) {
        Object_delete(coordinates->object);
        free(coordinates);
    }
}

void Coordinates_copyValuesFrom(struct Coordinates *recipient, struct Coordinates *source)
{
    recipient->x = source->x;
    recipient->y = source->y;
}

int Coordinates_haveTheSameValues(struct Coordinates *coordinates, struct Coordinates *other_coordinates)
{
    return(
              coordinates->x == other_coordinates->x &&
              coordinates->y == other_coordinates->y
          );
}

int Coordinates_isToTheNorthOf(struct Coordinates *a, struct Coordinates *b)
{
    return (a->y > b-> y);
}

int Coordinates_isToTheEastOf(struct Coordinates *a, struct Coordinates *b)
{
    return (a->x > b-> x);
}

int Coordinates_isToTheSouthOf(struct Coordinates *a, struct Coordinates *b)
{
    return (a->y < b-> y);
}

int Coordinates_isToTheWestOf(struct Coordinates *a, struct Coordinates *b)
{
    return (a->x < b-> x);
}

int Coordinates_isToTheNorthEastOf(struct Coordinates *a, struct Coordinates *b)
{
    return (Coordinates_isToTheNorthOf(a, b) &&
            Coordinates_isToTheEastOf(a, b));
}

int Coordinates_isToTheSouthEastOf(struct Coordinates *a, struct Coordinates *b)
{
    return (Coordinates_isToTheSouthOf(a, b) &&
            Coordinates_isToTheEastOf(a, b));
}

int Coordinates_isToTheSouthWestOf(struct Coordinates *a, struct Coordinates *b)
{
    return (Coordinates_isToTheSouthOf(a, b) &&
            Coordinates_isToTheWestOf(a, b));
}

int Coordinates_isToTheNorthWestOf(struct Coordinates *a, struct Coordinates *b)
{
    return (Coordinates_isToTheNorthOf(a, b) &&
            Coordinates_isToTheWestOf(a, b));
}

enum CardinalDirection Coordinates_cardinalDirectionTo(struct Coordinates *from, struct Coordinates *to)
{
    enum CardinalDirection direction;

    if(Coordinates_isToTheNorthEastOf(to, from)) {
        direction = NORTHEAST;
    } else if(Coordinates_isToTheSouthEastOf(to, from)) {
        direction = SOUTHEAST;
    } else if(Coordinates_isToTheSouthWestOf(to, from)) {
        direction = SOUTHWEST;
    } else if(Coordinates_isToTheNorthWestOf(to, from)) {
        direction = NORTHWEST;
    } else if(Coordinates_isToTheNorthOf(to, from)) {
        direction = NORTH;
    } else if(Coordinates_isToTheEastOf(to, from)) {
        direction = EAST;
    } else if(Coordinates_isToTheSouthOf(to, from)) {
        direction = SOUTH;
    } else if(Coordinates_isToTheWestOf(to, from)) {
        direction = WEST;
    } else {
        direction = CENTER;
    }

    return direction;
}

int Coordinates_distanceBetween(struct Coordinates *origin, struct Coordinates *destination)
{
    int distance_x = abs(origin->x - destination->x);
    int distance_y = abs(origin->y - destination->y);
    int distance = sqrt(pow(distance_x, 2) + pow(distance_y, 2));
    return distance;
}

int Coordinates_distanceFromOrigin(struct Coordinates *destination)
{
    struct Coordinates *origin = Coordinates_zero();
    int distance = Coordinates_distanceBetween(origin, destination);
    Coordinates_delete(origin);
    return distance;
}

void Coordinates_rotateOfAngleAround(struct Coordinates *toRotate, struct Angle *angle,
                                     struct Coordinates *rotationOrigin)
{
    int distance = Coordinates_distanceBetween(toRotate, rotationOrigin);
    int delta_x = toRotate->x - rotationOrigin->x;
    int delta_y = toRotate->y - rotationOrigin->y;
    double preexisting_theta = atan2(delta_y, delta_x);
    double theta = angle->theta * ANGLE_BASE_UNIT + preexisting_theta;
    int new_x = ceil(rotationOrigin->x + (distance * cos(theta)));
    int new_y = ceil(rotationOrigin->y + (distance * sin(theta)));
    toRotate->x = new_x;
    toRotate->y = new_y;
}

void Coordinates_rotateOfAngle(struct Coordinates *toRotate, struct Angle *angle)
{
    struct Coordinates *zero = Coordinates_zero();
    Coordinates_rotateOfAngleAround(toRotate, angle, zero);
    Coordinates_delete(zero);
}

int Coordinates_computeMeanX(struct Coordinates *a, struct Coordinates *b)
{
    int mean_x = (a->x + b->x) / 2;
    return mean_x;
}

int Coordinates_computeMeanY(struct Coordinates *a, struct Coordinates *b)
{
    int mean_y = (a->y + b->y) / 2;
    return mean_y;
}

void Coordinates_translateOf(struct Coordinates *toTranslate, struct Coordinates *delta)
{
    toTranslate->x += delta->x;
    toTranslate->y += delta->y;
}

void Coordinates_scaleOf(struct Coordinates *toScale, int scale_factor)
{
    toScale->x *= scale_factor;
    toScale->y *= scale_factor;
}
