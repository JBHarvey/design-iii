#include <stdlib.h>
#include "Behavior.h"
#include "BehaviorBuilder.h"

struct BehaviorBuilder *BehaviorBuilder_end(void)
{
    struct Flags *default_flags = Flags_new();
    struct BehaviorBuilder *pointer = (struct BehaviorBuilder *) malloc(sizeof(struct BehaviorBuilder));
    pointer->goalX = DEFAULT_GOAL_X ;
    pointer->goalY = DEFAULT_GOAL_Y ;
    pointer->goal_theta = DEFAULT_GOAL_THETA ;
    pointer->tolerancesX = X_TOLERANCE_DEFAULT;
    pointer->tolerancesY = Y_TOLERANCE_DEFAULT;
    pointer->tolerances_theta = THETA_TOLERANCE_DEFAULT;
    pointer->flags = default_flags;
    pointer->action = &Behavior_idle;

    return pointer;
}

struct Behavior* BehaviorBuilder_build(struct BehaviorBuilder *behavior_builder)
{
    struct Pose *chosen_pose = Pose_new(
                                   behavior_builder->goalX,
                                   behavior_builder->goalY,
                                   behavior_builder->goal_theta
                               );
    struct State *chosen_goal_state = State_new(chosen_pose);

    Flags_copyValuesFrom(chosen_goal_state->flags, behavior_builder->flags);

    struct Pose *chosen_pose_tolerances = Pose_new(
            behavior_builder->tolerancesX,
            behavior_builder->tolerancesY,
            behavior_builder->tolerances_theta
                                          );
    struct State *chosen_tolerances = State_new(chosen_pose_tolerances);

    struct Objective *objective = Objective_new(chosen_goal_state, chosen_tolerances);

    struct Behavior *behavior = Behavior_new(objective);

    Behavior_changeAction(behavior, behavior_builder->action);

    Pose_delete(chosen_pose);
    Pose_delete(chosen_pose_tolerances);
    State_delete(chosen_goal_state);
    State_delete(chosen_tolerances);
    Objective_delete(objective);

    Flags_delete(behavior_builder->flags);
    free(behavior_builder);

    return behavior;
}

struct Behavior* BehaviorBuilder_default(void)
{
    return BehaviorBuilder_build(BehaviorBuilder_end());
}

struct BehaviorBuilder* BehaviorBuilder_withFreeEntry(struct BehaviorBuilder *behavior_builder)
{
    behavior_builder = BehaviorBuilder_withFreePoseEntry(behavior_builder);
    behavior_builder = BehaviorBuilder_withFreeFlagsEntry(behavior_builder);
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withFreeFlagsEntry(struct BehaviorBuilder *behavior_builder)
{
    struct Flags *irrelevant = Flags_irrelevant();
    behavior_builder = BehaviorBuilder_withFlags(irrelevant, behavior_builder);
    Flags_delete(irrelevant);
    return behavior_builder;
}


struct BehaviorBuilder* BehaviorBuilder_withFreePoseEntry(struct BehaviorBuilder *behavior_builder)
{
    behavior_builder = BehaviorBuilder_withTolerancesX(X_TOLERANCE_MAX, behavior_builder);
    behavior_builder = BehaviorBuilder_withTolerancesY(Y_TOLERANCE_MAX, behavior_builder);
    behavior_builder = BehaviorBuilder_withTolerancesTheta(THETA_TOLERANCE_MAX, behavior_builder);
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withFreeTrajectoryEntry(struct BehaviorBuilder *behavior_builder)
{
    behavior_builder = BehaviorBuilder_withTolerancesTheta(THETA_TOLERANCE_MAX, behavior_builder);
    behavior_builder = BehaviorBuilder_withFreeFlagsEntry(behavior_builder);
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withFreeOrientationEntry(struct BehaviorBuilder *behavior_builder)
{
    behavior_builder = BehaviorBuilder_withTolerancesX(X_TOLERANCE_MAX, behavior_builder);
    behavior_builder = BehaviorBuilder_withTolerancesY(Y_TOLERANCE_MAX, behavior_builder);
    behavior_builder = BehaviorBuilder_withFreeFlagsEntry(behavior_builder);
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withGoalX(int goalX, struct BehaviorBuilder *behavior_builder)
{
    behavior_builder->goalX = goalX;
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withGoalY(int goalY, struct BehaviorBuilder *behavior_builder)
{
    behavior_builder->goalY = goalY;
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withGoalTheta(int goal_theta, struct BehaviorBuilder *behavior_builder)
{
    behavior_builder->goal_theta = goal_theta;
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withTolerancesX(int tolerancesX, struct BehaviorBuilder *behavior_builder)
{
    behavior_builder->tolerancesX = tolerancesX;
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withTolerancesY(int tolerancesY, struct BehaviorBuilder *behavior_builder)
{
    behavior_builder->tolerancesY = tolerancesY;
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withTolerancesTheta(int tolerances_theta,
        struct BehaviorBuilder *behavior_builder)
{
    behavior_builder->tolerances_theta = tolerances_theta;
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_withFlags(struct Flags *flags,
        struct BehaviorBuilder *behavior_builder)
{
    Flags_copyValuesFrom(behavior_builder->flags, flags);
    return behavior_builder;
}
struct BehaviorBuilder* BehaviorBuilder_withAction(void (*new_action)(struct Robot *),
        struct BehaviorBuilder *behavior_builder)
{
    behavior_builder->action = new_action;
    return behavior_builder;
}

struct BehaviorBuilder* BehaviorBuilder_fromExisting(struct Behavior *existing,
        struct BehaviorBuilder *behavior_builder)
{
    struct Objective *objective = existing->entry_conditions;

    struct State *goal_state = objective->goal_state;
    struct Pose *goal_pose = goal_state->pose;
    struct Coordinates *goal_coordinates = goal_pose->coordinates;
    struct Angle *goal_angle = goal_pose->angle;

    struct State *tolerances_state = objective->tolerances;
    struct Pose *tolerances_pose = tolerances_state->pose;
    struct Coordinates *tolerances_coordinates = tolerances_pose->coordinates;
    struct Angle *tolerances_angle = tolerances_pose->angle;

    behavior_builder = BehaviorBuilder_withGoalX(goal_coordinates->x, behavior_builder);
    behavior_builder = BehaviorBuilder_withGoalY(goal_coordinates->y, behavior_builder);
    behavior_builder = BehaviorBuilder_withGoalTheta(goal_angle->theta, behavior_builder);
    behavior_builder = BehaviorBuilder_withTolerancesX(tolerances_coordinates->x, behavior_builder);
    behavior_builder = BehaviorBuilder_withTolerancesY(tolerances_coordinates->y, behavior_builder);
    behavior_builder = BehaviorBuilder_withTolerancesTheta(tolerances_angle->theta, behavior_builder);
    behavior_builder = BehaviorBuilder_withFlags(goal_state->flags, behavior_builder);
    behavior_builder = BehaviorBuilder_withAction(existing->action, behavior_builder);
    return behavior_builder;
}
