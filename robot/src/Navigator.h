#ifndef NAVIGATOR_H_
#define NAVIGATOR_H_

#include "BehaviorBuilder.h"
#include "Graph.h"
#include "Robot.h"

#define THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_RATIO -2.48597814232
#define THEORICAL_DISTANCE_OVER_ROTATION_TOLERANCE_BASE 13430

struct Navigator {
    struct Object *object;
    struct Map *navigable_map;
    struct Graph *graph;
};

struct Navigator *Navigator_new(void);
void Navigator_delete(struct Navigator *navigator);

void Navigator_updateNavigableMap(struct Robot *robot);
int Navigator_computeRotationToleranceForPrecisionMovement(int planned_distance);

#endif // NAVIGATOR_H_