#ifndef COORDINATES_H_
#define COORDINATES_H_

#include "Angle.h"
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

int Coordinates_distanceBetween(struct Coordinates *origin, struct Coordinates *destination);
int Coordinates_distanceFromOrigin(struct Coordinates *destination);

void Coordinates_rotateOfAngleAround(struct Coordinates *toRotate, struct Angle *angle, struct Coordinates *rotationOrigin);
void Coordinates_rotateOfAngle(struct Coordinates *toRotate, struct Angle *angle);
void Coordinates_translateOf(struct Coordinates *toTranslate, struct Coordinates *delta);
void Coordinates_scaleOf(struct Coordinates *toScale, int scale_factor);

int Coordinates_computeMeanX(struct Coordinates *a, struct Coordinates *b);
int Coordinates_computeMeanY(struct Coordinates *a, struct Coordinates *b);
struct Angle *Coordinates_angleBetween(struct Coordinates *a, struct Coordinates *b);
#endif // COORDINATES_H_
