#include <stdlib.h>
#include "Angle.h"


/**
* Ensures the angle stays between MINUS_PI and PI.
*/
int Angle_wrap(int theta)
{
    while(theta > PI) {
        theta -= 2 * PI;
    }

    while(theta <= MINUS_PI) {
        theta += 2 * PI;
    }

    return theta;
}

struct Angle* Angle_new(int new_theta)
{
    struct Angle * pointer = (struct Angle *) malloc(sizeof(struct Angle));
    int wrappedTheta = Angle_wrap(new_theta);
    pointer->theta = wrappedTheta;
    return pointer;
}

void Angle_delete(struct Angle* angle)
{
    free(angle);
}

int Angle_smallestAngleBetween(struct Angle* alpha, struct Angle* beta)
{
    int wrappedAlpha = Angle_wrap(alpha->theta);
    int wrappedBeta = Angle_wrap(beta->theta);
    int distance = 0;

    if((wrappedAlpha > 0 && wrappedBeta > 0)
       || (wrappedAlpha < 0 && wrappedBeta < 0)) {
        distance = abs(wrappedAlpha - wrappedBeta);
    } else {
        distance = abs(wrappedBeta) + abs(wrappedAlpha);

        if(distance > PI) {
            distance = 2 * PI - distance;
        }
    }

    return Angle_wrap(distance);
}


enum RotationDirection Angle_fetchRotationDirectionToReduceDistanceBetween(struct Angle* goal, struct Angle* current)
{
    int goalValue = Angle_wrap(goal->theta);
    int currentValue = Angle_wrap(current->theta);

    if(goalValue == currentValue) {
        return STOP_TURNING;
    }

    int distance = Angle_smallestAngleBetween(goal, current);

    if(Angle_wrap(goalValue - distance) == currentValue) {
        return ANTICLOCKWISE;
    }

    return CLOCKWISE;

}

