#include <stdlib.h>
#include "ManchesterCode.h"

struct ManchesterCode *ManchesterCode_new()
{
    struct Object *new_object = Object_new();
    int new_painting_number  = 0;
    int new_scale_factor  = TIMES_TWO;
    enum CardinalDirection new_orientation = EAST;

    struct ManchesterCode *pointer = malloc(sizeof(struct ManchesterCode));

    pointer->object = new_object;
    pointer->painting_number = new_painting_number;
    pointer->scale_factor = new_scale_factor;
    pointer->orientation = new_orientation;

    return pointer;
}

void ManchesterCode_delete(struct ManchesterCode *manchester_code)
{
    Object_removeOneReference(manchester_code->object);

    if(Object_canBeDeleted(manchester_code->object)) {
        Object_delete(manchester_code->object);
        free(manchester_code);
    }
}

void ManchesterCode_updateCodeValues(struct ManchesterCode *manchester_code, int new_painting_number,
                                     int new_scale_factor, enum CardinalDirection new_orientation)
{
    manchester_code->painting_number = new_painting_number;
    manchester_code->scale_factor = new_scale_factor;
    manchester_code->orientation = new_orientation;
}
