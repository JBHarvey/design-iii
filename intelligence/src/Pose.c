#include <stdlib.h>
#include <stdio.h>
#include "Pose.h"


struct Pose * Pose_new(int new_x, int new_y, int new_theta)
{
    struct Pose * pointer = (struct Pose *) malloc(sizeof(struct Pose));
    pointer->x = new_x;
    pointer->y = new_y;
    pointer->theta = new_theta;
    return pointer;
}

void Pose_delete(struct Pose * pose)
{
    free(pose);
}

void Pose_print(struct Pose * pose)
{
    printf("Pose -->  x: %i * 10^(-1) mm\n", pose->x);
    printf("          y: % i * 10 ^ (-1) mm\n", pose->y);
    printf("          theta: % i * 10 ^ (-4) rad", pose->theta);
}
