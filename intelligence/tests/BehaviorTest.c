#include <criterion/criterion.h>
#include <stdio.h>
#include "BehaviorBuilder.h"

Test(Behavior, creation_destruction)
{
    int x = 10000;
    int y = 10000;
    int theta = 78540;
    struct Pose *goalPose = Pose_new(x, y, theta);
    struct State *goalState = State_new(goalPose);
    struct Pose *maxPoseTolerances = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_MAX);
    struct State *tolerances = State_new(maxPoseTolerances);
    Pose_delete(maxPoseTolerances);
    struct Objective *objective = Objective_new(goalState, tolerances);
    struct Behavior *behavior = Behavior_new(objective);

    cr_assert(behavior->entryConditions == objective);

    Pose_delete(goalPose);
    State_delete(tolerances);
    State_delete(goalState);
    Objective_delete(objective);
    Behavior_delete(behavior);
}

Test(Behavior, given_aDefaultValuedBehaviorAndDefaultValuedState_when_checksIfIsReached_then_isReached)
{
    struct Pose *pose = Pose_new(DEFAULT_GOAL_X, DEFAULT_GOAL_Y, DEFAULT_GOAL_THETA);
    struct State *state = State_new(pose);
    struct Behavior *behavior = BehaviorBuilder_default();

    int isReached = Behavior_areEntryConditionsReached(behavior, state);
    cr_assert(isReached);

    Pose_delete(pose);
    State_delete(state);
    Behavior_delete(behavior);
}

Test(Behavior, given_aDefaultValuedBehaviorAndOffValuedState_when_checksIfIsReached_then_isNotReached)
{
    struct Pose *offPose = Pose_new(DEFAULT_GOAL_X, Y_TOLERANCE_MAX, DEFAULT_GOAL_THETA);
    struct State *offState = State_new(offPose);
    struct Behavior *behavior = BehaviorBuilder_default();

    int isReached = Behavior_areEntryConditionsReached(behavior, offState);
    cr_assert(isReached == 0);

    Pose_delete(offPose);
    State_delete(offState);
    Behavior_delete(behavior);
}
