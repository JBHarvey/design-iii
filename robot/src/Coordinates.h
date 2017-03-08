#ifndef COORDINATES_H_
#define COORDINATES_H_

#include "Object.h"

enum CardinalDirection {NORTH, EAST, SOUTH, WEST, CENTER, NORTHEAST, SOUTHEAST, SOUTHWEST, NORTHWEST};

struct Coordinates {
    struct Object *object;
    int x;        /* Unit: 0.1 mm */
    int y;        /* Unit: 0.1 mm */
};

struct Coordinates *Coordinates_zero(void);
struct Coordinates *Coordinates_new(int newX, int newY);
void Coordinates_delete(struct Coordinates *coordinates);

void Coordinates_copyValuesFrom(struct Coordinates *recipient, struct Coordinates *source);
int Coordinates_haveTheSameValues(struct Coordinates *coordinates, struct Coordinates *other_coordinates);

/**
 * The axis being:
 * y
 * |      N     
 * |  NW  |  NE 
 * |      |/    
 * |  W---C---E 
 * |     /|     
 * |  SW  |  SE 
 * |      S     
 * +-------------x
 */
enum CardinalDirection Coordinates_cardinalDirectionTo(struct Coordinates *from, struct Coordinates *to);

int Coordinates_isToTheNorthOf(struct Coordinates *a, struct Coordinates *b);
int Coordinates_isToTheEastOf(struct Coordinates *a, struct Coordinates *b);
int Coordinates_isToTheSouthOf(struct Coordinates *a, struct Coordinates *b);
int Coordinates_isToTheWestOf(struct Coordinates *a, struct Coordinates *b);
int Coordinates_isToTheNorthEastOf(struct Coordinates *a, struct Coordinates *b);
int Coordinates_isToTheSouthEastOf(struct Coordinates *a, struct Coordinates *b);
int Coordinates_isToTheSouthWestOf(struct Coordinates *a, struct Coordinates *b);
int Coordinates_isToTheNorthWestOf(struct Coordinates *a, struct Coordinates *b);

#endif // COORDINATES_H_
