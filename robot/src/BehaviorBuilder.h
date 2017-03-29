#ifndef BEHAVIORBUILDER_H_
#define BEHAVIORBUILDER_H_

#include "Behavior.h"

struct BehaviorBuilder {
    int goalX;
    int goalY;
    int goal_theta;
    int tolerancesX;
    int tolerancesY;
    int tolerances_theta;
    struct Flags *flags;
    void (*action)(struct Robot *);
};

struct BehaviorBuilder *BehaviorBuilder_end(void);
struct Behavior* BehaviorBuilder_build(struct BehaviorBuilder *behavior_builder);
struct Behavior* BehaviorBuilder_default(void);
struct BehaviorBuilder* BehaviorBuilder_withFreeEntry(struct BehaviorBuilder *behavior_builder);
struct BehaviorBuilder* BehaviorBuilder_withGoalX(int goalX, struct BehaviorBuilder *behavior_builder);
struct BehaviorBuilder* BehaviorBuilder_withGoalY(int goalY, struct BehaviorBuilder *behavior_builder);
struct BehaviorBuilder* BehaviorBuilder_withGoalTheta(int goal_theta, struct BehaviorBuilder *behavior_builder);
struct BehaviorBuilder* BehaviorBuilder_withTolerancesX(int tolerancesX, struct BehaviorBuilder *behavior_builder);
struct BehaviorBuilder* BehaviorBuilder_withTolerancesY(int tolerancesY, struct BehaviorBuilder *behavior_builder);
struct BehaviorBuilder* BehaviorBuilder_withTolerancesTheta(int tolerances_theta, struct BehaviorBuilder *behavior_builder);
struct BehaviorBuilder* BehaviorBuilder_withFlags(struct Flags *flags, struct BehaviorBuilder *behavior_builder);
struct BehaviorBuilder* BehaviorBuilder_withAction(void (*new_action)(struct Robot *), struct BehaviorBuilder *behavior_builder);
struct BehaviorBuilder* BehaviorBuilder_fromExisting(struct Behavior *existing, struct BehaviorBuilder *behavior_builder);

#endif // BEHAVIORBUILDER_H_
