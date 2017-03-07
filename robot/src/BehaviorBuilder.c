#include <stdlib.h>
#include "Behavior.h"
#include "BehaviorBuilder.h"

struct BehaviorBuilder *BehaviorBuilder_end(void)
{
    struct Flags *defaultFlags = Flags_new();
    struct BehaviorBuilder *pointer = (struct BehaviorBuilder *) malloc(sizeof(struct BehaviorBuilder));
    pointer->goalX = DEFAULT_GOAL_X ;
    pointer->goalY = DEFAULT_GOAL_Y ;
    pointer->goalTheta = DEFAULT_GOAL_THETA ;
    pointer->tolerancesX = X_TOLERANCE_DEFAULT;
    pointer->tolerancesY = Y_TOLERANCE_DEFAULT;
    pointer->tolerancesTheta = THETA_TOLERANCE_DEFAULT;
    pointer->flags = defaultFlags;
    pointer->action = &Behavior_dummyAction;

    return pointer;
}

struct Behavior* BehaviorBuilder_build(struct BehaviorBuilder *behaviorBuilder)
{
    struct Pose *chosenPose = Pose_new(
                                  behaviorBuilder->goalX,
                                  behaviorBuilder->goalY,
                                  behaviorBuilder->goalTheta
                              );
    struct State *chosenGoalState = State_new(chosenPose);

    Flags_copyValuesFrom(chosenGoalState->flags, behaviorBuilder->flags);

    struct Pose *chosenPoseTolerances = Pose_new(
                                            behaviorBuilder->tolerancesX,
                                            behaviorBuilder->tolerancesY,
                                            behaviorBuilder->tolerancesTheta
                                        );
    struct State *chosenTolerances = State_new(chosenPoseTolerances);

    struct Objective *objective = Objective_new(chosenGoalState, chosenTolerances);

    struct Behavior *behavior = Behavior_new(objective);

    Behavior_changeAction(behavior, behaviorBuilder->action);

    Pose_delete(chosenPose);
    Pose_delete(chosenPoseTolerances);
    State_delete(chosenGoalState);
    State_delete(chosenTolerances);
    Objective_delete(objective);

    Flags_delete(behaviorBuilder->flags);
    free(behaviorBuilder);

    return behavior;
}

struct Behavior* BehaviorBuilder_default(void)
{
    return BehaviorBuilder_build(BehaviorBuilder_end());
}
struct BehaviorBuilder* BehaviorBuilder_withGoalX(int goalX, struct BehaviorBuilder *behaviorBuilder)
{
    behaviorBuilder->goalX = goalX;
    return behaviorBuilder;
}

struct BehaviorBuilder* BehaviorBuilder_withGoalY(int goalY, struct BehaviorBuilder *behaviorBuilder)
{
    behaviorBuilder->goalY = goalY;
    return behaviorBuilder;
}

struct BehaviorBuilder* BehaviorBuilder_withGoalTheta(int goalTheta, struct BehaviorBuilder *behaviorBuilder)
{
    behaviorBuilder->goalTheta = goalTheta;
    return behaviorBuilder;
}

struct BehaviorBuilder* BehaviorBuilder_withTolerancesX(int tolerancesX, struct BehaviorBuilder *behaviorBuilder)
{
    behaviorBuilder->tolerancesX = tolerancesX;
    return behaviorBuilder;
}

struct BehaviorBuilder* BehaviorBuilder_withTolerancesY(int tolerancesY, struct BehaviorBuilder *behaviorBuilder)
{
    behaviorBuilder->tolerancesY = tolerancesY;
    return behaviorBuilder;
}

struct BehaviorBuilder* BehaviorBuilder_withTolerancesTheta(int tolerancesTheta,
        struct BehaviorBuilder *behaviorBuilder)
{
    behaviorBuilder->tolerancesTheta = tolerancesTheta;
    return behaviorBuilder;
}

struct BehaviorBuilder* BehaviorBuilder_withFlags(struct Flags *flags,
        struct BehaviorBuilder *behaviorBuilder)
{
    Flags_copyValuesFrom(behaviorBuilder->flags, flags);
    return behaviorBuilder;
}
struct BehaviorBuilder* BehaviorBuilder_withAction(void (*new_action)(struct Robot *),
        struct BehaviorBuilder *behaviorBuilder)
{
    behaviorBuilder->action = new_action;
    return behaviorBuilder;
}

struct BehaviorBuilder* BehaviorBuilder_fromExisting(struct Behavior *existing, struct BehaviorBuilder *behaviorBuilder)
{
    struct Objective *objective = existing->entryConditions;

    struct State *goalState = objective->goalState;
    struct Pose *goalPose = goalState->pose;
    struct Coordinates *goalCoordinates = goalPose->coordinates;
    struct Angle *goalAngle = goalPose->angle;

    struct State *tolerancesState = objective->tolerances;
    struct Pose *tolerancesPose = tolerancesState->pose;
    struct Coordinates *tolerancesCoordinates = tolerancesPose->coordinates;
    struct Angle *tolerancesAngle = tolerancesPose->angle;

    behaviorBuilder = BehaviorBuilder_withGoalX(goalCoordinates->x, behaviorBuilder);
    behaviorBuilder = BehaviorBuilder_withGoalY(goalCoordinates->y, behaviorBuilder);
    behaviorBuilder = BehaviorBuilder_withGoalTheta(goalAngle->theta, behaviorBuilder);
    behaviorBuilder = BehaviorBuilder_withTolerancesX(tolerancesCoordinates->x, behaviorBuilder);
    behaviorBuilder = BehaviorBuilder_withTolerancesY(tolerancesCoordinates->y, behaviorBuilder);
    behaviorBuilder = BehaviorBuilder_withTolerancesTheta(tolerancesAngle->theta, behaviorBuilder);
    behaviorBuilder = BehaviorBuilder_withFlags(goalState->flags, behaviorBuilder);
    behaviorBuilder = BehaviorBuilder_withAction(existing->action, behaviorBuilder);
    return behaviorBuilder;
}
