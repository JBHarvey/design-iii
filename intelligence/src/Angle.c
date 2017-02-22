#include <stdlib.h>
#include "Angle.h"


struct Angle* Angle_new(int new_theta)
{
    struct Angle * pointer = (struct Angle *) malloc(sizeof(struct Angle));
    int wrappedTheta = Angle_wrap(new_theta);
    pointer->theta = wrappedTheta;
    return pointer;
}

/**
 * Ensures the angle stays between MINUS_PI and PI.
 */
int Angle_wrap(int theta)
{
    while(theta > PI) {
        theta -= 2 * PI;
    }

    while(theta < MINUS_PI) {
        theta += 2 * PI;
    }

    return theta;
}


void Angle_delete(struct Angle* angle)
{
    free(angle);
}
