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
    struct CoordinatesSequence *planned_trajectory;
    int was_oriented_before_last_command;
};

struct Navigator *Navigator_new(void);
void Navigator_delete(struct Navigator *navigator);

void Navigator_updateNavigableMap(struct Robot *robot);

int Navigator_isAngleWithinRotationTolerance(int angle);
void Navigator_navigateRobotTowardsGoal(struct Robot *robot);
void Navigator_orientRobotTowardsGoal(struct Robot *robot);

void Navigator_planTowardsAntennaStart(struct Robot *robot);
void Navigator_planTowardsAntennaMiddle(struct Robot *robot);
void Navigator_planOrientationTowardsAntenna(struct Robot *robot);
void Navigator_planFetchingManchesterCode(struct Robot *robot);
void Navigator_planLowerPenForAntennaMark(struct Robot *robot);
void Navigator_planTowardsAntennaMarkEnd(struct Robot *robot);
void Navigator_planRisePenForObstacleCrossing(struct Robot *robot);
void Navigator_planTowardsObstacleZoneEastSide(struct Robot *robot);
void Navigator_planTowardsPaintingZone(struct Robot *robot);
/*
void Navigator_planTowardsAntennaStop(struct Robot *robot);
 */

int Navigator_computeRotationToleranceForPrecisionMovement(int planned_distance);


#endif // NAVIGATOR_H_
