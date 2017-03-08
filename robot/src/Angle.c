#include <stdlib.h>
#include "Angle.h"


/**
*Ensures the angle stays between MINUS_PI and PI.
*/
static int Angle_wrap(int theta)
{
    while(theta > PI) {
        theta -= 2 * PI;
    }

    while(theta <= MINUS_PI) {
        theta += 2 * PI;
    }

    return theta;
}

struct Angle *Angle_new(int new_theta)
{
    struct Object *new_object = Object_new();
    struct Angle *pointer = (struct Angle *) malloc(sizeof(struct Angle));
    int wrapped_theta = Angle_wrap(new_theta);

    pointer->object = new_object;
    pointer->theta = wrapped_theta;

    return pointer;
}

void Angle_delete(struct Angle *angle)
{
    Object_removeOneReference(angle->object);

    if(Object_canBeDeleted(angle->object)) {
        Object_delete(angle->object);
        free(angle);
    }
}

int Angle_smallestAngleBetween(struct Angle *alpha, struct Angle *beta)
{
    int wrapped_alpha = Angle_wrap(alpha->theta);
    int wrapped_beta = Angle_wrap(beta->theta);
    int distance = 0;

    if((wrapped_alpha > 0 && wrapped_beta > 0)
       || (wrapped_alpha < 0 && wrapped_beta < 0)) {
        distance = abs(wrapped_alpha - wrapped_beta);
    } else {
        distance = abs(wrapped_beta) + abs(wrapped_alpha);

        if(distance > PI) {
            distance = 2 * PI - distance;
        }
    }

    return Angle_wrap(distance);
}


enum RotationDirection Angle_fetchOptimalRotationDirection(struct Angle *goal, struct Angle *current)
{
    int goal_value = Angle_wrap(goal->theta);
    int current_value = Angle_wrap(current->theta);

    if(goal_value == current_value) {
        return STOP_TURNING;
    }

    int distance = Angle_smallestAngleBetween(goal, current);

    if(Angle_wrap(goal_value - distance) == current_value) {
        return ANTICLOCKWISE;
    }

    return CLOCKWISE;

}

