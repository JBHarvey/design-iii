#include <stdlib.h>
#include "Navigator.h"

struct Navigator *Navigator_new(void)
{
    struct Object *new_object = Object_new();
    struct Map *new_navigable_map = NULL;
    struct Graph *new_graph = Graph_new();
    struct Navigator *pointer =  malloc(sizeof(struct Navigator));

    pointer->object = new_object;
    pointer->navigable_map = new_navigable_map;
    pointer->graph = new_graph;

    return pointer;
}

void Navigator_delete(struct Navigator *navigator)
{
    Object_removeOneReference(navigator->object);

    if(Object_canBeDeleted(navigator->object)) {
        Object_delete(navigator->object);

        if(navigator->navigable_map != NULL) {
            Map_delete(navigator->navigable_map);
        }

        Graph_delete(navigator->graph);

        free(navigator);
    }
}

void Navigator_updateNavigableMap(struct Robot *robot)
{

}

int Navigator_computeRotationToleranceForPrecisionMovement(int planned_distance)
{
    int value = (int)(planned_distance * THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_RATIO) +
                THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_BASE;
    return value;
}
