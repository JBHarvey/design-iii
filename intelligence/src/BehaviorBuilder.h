#ifndef BEHAVIORBUILDER_H_
#define BEHAVIORBUILDER_H_

#include "Behavior.h"

#define DEFAULT_GOAL_X 0
#define DEFAULT_GOAL_Y 0
#define DEFAULT_GOAL_THETA 0

struct BehaviorBuilder {
    int goalX;
    int goalY;
    int goalTheta;
    int tolerancesX;
    int tolerancesY;
    int tolerancesTheta;
    struct Flags *flags;
};

struct BehaviorBuilder *BehaviorBuilder_end(void);
struct Behavior* BehaviorBuilder_build(struct BehaviorBuilder *behaviorBuilder);
struct Behavior* BehaviorBuilder_default(void);
struct BehaviorBuilder* BehaviorBuilder_withGoalX(int goalX, struct BehaviorBuilder *behaviorBuilder);
struct BehaviorBuilder* BehaviorBuilder_withGoalY(int goalY, struct BehaviorBuilder *behaviorBuilder);
struct BehaviorBuilder* BehaviorBuilder_withGoalTheta(int goalTheta, struct BehaviorBuilder *behaviorBuilder);
struct BehaviorBuilder* BehaviorBuilder_withTolerancesX(int tolerancesX, struct BehaviorBuilder *behaviorBuilder);
struct BehaviorBuilder* BehaviorBuilder_withTolerancesY(int tolerancesY, struct BehaviorBuilder *behaviorBuilder);
struct BehaviorBuilder* BehaviorBuilder_withTolerancesTheta(int tolerancesTheta, struct BehaviorBuilder *behaviorBuilder);
struct BehaviorBuilder* BehaviorBuilder_withFlags(struct Flags *flags, struct BehaviorBuilder *behaviorBuilder);
struct BehaviorBuilder* BehaviorBuilder_fromExisting(struct Behavior *existing, struct BehaviorBuilder *behaviorBuilder);

#endif // BEHAVIORBUILDER_H_
