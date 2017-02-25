#ifndef ANGLE_H_
#define ANGLE_H_
#include "Object.h"

#define PI 314160
#define MINUS_PI -314160

enum RotationDirection {ANTICLOCKWISE, CLOCKWISE, STOP_TURNING};
/*
 * This module exists to facilitate the manipulation of angle in this program
 * It is important to note that the default used unit is:
 *      0.00001 rad
 *      and is between ]-Pi, Pi],
 *      in integer
 */
struct Angle {
    struct Object *object;
    int theta;
};

struct Angle *Angle_new(int new_theta);
void Angle_delete(struct Angle *angle);
int Angle_smallestAngleBetween(struct Angle *alpha, struct Angle *beta);
enum RotationDirection Angle_fetchOptimalRotationDirection(struct Angle *goal, struct Angle *current);

#endif // ANGLE_H_
