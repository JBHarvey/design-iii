#include <stdlib.h>
#include <stdio.h>
#include "Coordinates.h"


struct Coordinates *Coordinates_zero(void)
{
    return Coordinates_new(0, 0);
}

struct Coordinates *Coordinates_new(int new_x, int new_y)
{
    struct Object *new_object = Object_new();
    struct Coordinates *pointer = (struct Coordinates *) malloc(sizeof(struct Coordinates));

    pointer->object = new_object;

    pointer->x = new_x;
    pointer->y = new_y;

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

int Coordinates_haveTheSameValues(struct Coordinates *coordinates, struct Coordinates *otherCoordinates)
{
    return(
              coordinates->x == otherCoordinates->x &&
              coordinates->y == otherCoordinates->y
          );
}
