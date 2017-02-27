#include <criterion/criterion.h>
#include <stdio.h>
#include "Behavior.h"

/*
struct Pose *pose;
struct Pose *defaultPoseTolerances;
struct State *defaultTolerances;
struct State *goalState;
struct Objective *defaultObjective;

void setupObjective(void)
{
    pose = Pose_new(GOAL_X, GOAL_Y, GOAL_THETA);
    goalState = State_new(pose);

    defaultPoseTolerances = Pose_new(X_TOLERANCE_DEFAULT, Y_TOLERANCE_DEFAULT, THETA_TOLERANCE_DEFAULT);
    defaultTolerances = State_new(defaultPoseTolerances);
    defaultObjective  = Objective_new(goalState, defaultTolerances);
}

void teardownObjective(void)
{
    Pose_delete(pose);
    Pose_delete(defaultPoseTolerances);
    State_delete(defaultTolerances);
    State_delete(goalState);
    Objective_delete(defaultObjective);
}
*/
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

    cr_assert(behavior->entryCondition == objective);

    Pose_delete(goalPose);
    State_delete(tolerances);
    State_delete(goalState);
    Objective_delete(objective);
    Behavior_delete(behavior);
}
