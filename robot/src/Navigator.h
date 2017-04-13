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
    struct Angle *trajectory_start_angle;
};

struct Navigator *Navigator_new(void);
void Navigator_delete(struct Navigator *navigator);

void Navigator_updateNavigableMap(struct Robot *robot);

int Navigator_isAngleWithinCapTolerance(int angle, int current_speed, int angular_tolerance);
void Navigator_stopMovement(struct Robot *robot);
void Navigator_computeTrajectoryStartAngle(struct Robot *robot);
void Navigator_navigateRobotTowardsGoal(struct Robot *robot);
void Navigator_orientRobotTowardsGoal(struct Robot *robot);

void Navigator_planTowardsAntennaMiddle(struct Robot *robot);
void Navigator_planOrientationTowardsAntenna(struct Robot *robot);
void Navigator_planStopMotionBeforeFetchingManchester(struct Robot *robot);
void Navigator_planFetchingManchesterCode(struct Robot *robot);
void Navigator_planLowerPenForAntennaMark(struct Robot *robot);
void Navigator_planTowardsAntennaMarkEnd(struct Robot *robot);
void Navigator_planRisePenForObstacleCrossing(struct Robot *robot);
void Navigator_planTowardsObstacleZoneEastSide(struct Robot *robot);
void Navigator_planTowardsPaintingZone(struct Robot *robot);
void Navigator_planTowardsPainting(struct Robot *robot);
void Navigator_planOrientationTowardsPainting(struct Robot *robot);
void Navigator_planStopMotionBeforePicture(struct Robot *robot);
void Navigator_planLightingGreenLedBeforePicture(struct Robot *robot);
void Navigator_planTakingPicture(struct Robot *robot);
void Navigator_planTowardsObstacleZoneWestSide(struct Robot *robot);
void Navigator_planUpdateMapBeforeGoingToDrawingZone(struct Robot *robot);
void Navigator_planTowardsDrawingZone(struct Robot *robot);
void Navigator_planTowardsCenterOfDrawingZone(struct Robot *robot);
void Navigator_planToTellReadyToDraw(struct Robot *robot);
void Navigator_planTowardsDrawingStart(struct Robot *robot);
void Navigator_planLowerPenBeforeDrawing(struct Robot *robot);
void Navigator_planDrawing(struct Robot *robot);
void Navigator_planRisePenBeforeGoingToAntennaStop(struct Robot *robot);
void Navigator_planTowardsAntennaStop(struct Robot *robot);
void Navigator_planStopMotionForEndOfCycle(struct Robot *robot);
void Navigator_planEndOfCycleAndSendSignal(struct Robot *robot);
void Navigator_planLightingRedLedUntilNewCycle(struct Robot *robot);
void Navigator_planUpdateMapForNewCycle(struct Robot *robot);

int Navigator_computeRotationToleranceForPrecisionMovement(int planned_distance);


#endif // NAVIGATOR_H_
