#ifndef COORDINATES_H_
#define COORDINATES_H_

#include "Object.h"

struct Coordinates {
    struct Object *object;
    int x;        /* Unit: 0.1 mm */
    int y;        /* Unit: 0.1 mm */
};

struct Coordinates *Coordinates_zero(void);
struct Coordinates *Coordinates_new(int new_x, int new_y);
void Coordinates_delete(struct Coordinates *coordinates);

void Coordinates_copyValuesFrom(struct Coordinates *recipient, struct Coordinates *source);
int Coordinates_haveTheSameValues(struct Coordinates *coordinates, struct Coordinates *otherCoordinates);

#endif // COORDINATES_H_
