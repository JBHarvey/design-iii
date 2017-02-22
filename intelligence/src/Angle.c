#include <stdlib.h>
#include "Angle.h"


struct Angle* Angle_new(int new_theta)
{
    struct Angle * pointer = (struct Angle *) malloc(sizeof(struct Angle));
    pointer->theta = new_theta;
    return pointer;
}

void Angle_delete(struct Angle* angle)
{
    free(angle);
}
