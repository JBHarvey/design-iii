#include <criterion/criterion.h>
#include <stdio.h>
#include "State.h"
#include "Objective.h"

const int GOAL_X = 10000;       // 1 m
const int GOAL_Y = 10000;       // 1 m
const int GOAL_THETA = 78540;    // Pi/4 rad

struct Pose *pose;
struct Pose *defaultPoseTolerances;
struct State *defaultTolerances;
struct State *goalState;
struct Objective *defaultObjective;
/*
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

Test(Objective, creation_destruction, .init = setupObjective, .fini = teardownObjective)
{
    struct Pose *maxPoseTolerances = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_MAX);
    struct State *tolerances = State_new(maxPoseTolerances);
    struct Objective *objective = Objective_new(goalState, tolerances);

    cr_assert(objective->goalState == goalState
              && objective->tolerances == tolerances);

    Pose_delete(maxPoseTolerances);
    State_delete(tolerances);
    Objective_delete(objective);
}

Test(Objective, given_maxTolerances_when_checksIfIsReached_then_isReached,
     .init = setupObjective, .fini = teardownObjective)
{
    struct Pose *maxPoseTolerances = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_MAX);
    struct State *maxTolerances = State_new(maxPoseTolerances);
    struct Objective *reachableObjective = Objective_new(goalState, maxTolerances);

    int reached = Objective_isReached(reachableObjective, goalState);

    cr_assert(reached == 1);

    Pose_delete(maxPoseTolerances);
    State_delete(maxTolerances);
    Objective_delete(reachableObjective);
}

Test(Objective, given_zeroTolerancesAndExactState_when_checksIfIsReached_then_isReached,
     .init = setupObjective, .fini = teardownObjective)
{
    struct Pose *zeroPoseTolerances = Pose_new(X_TOLERANCE_MIN, Y_TOLERANCE_MIN, THETA_TOLERANCE_MIN);
    struct State *zeroTolerances = State_new(zeroPoseTolerances);
    struct Objective *zeroToleranceObjective = Objective_new(goalState, zeroTolerances);

    int reached = Objective_isReached(zeroToleranceObjective, goalState);

    cr_assert(reached == 1);

    Pose_delete(zeroPoseTolerances);
    State_delete(zeroTolerances);
    Objective_delete(zeroToleranceObjective);
}

Test(Objective_DefaultTolerance, given_anOnSpotState_when_checksIfIsReached_then_isReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int reached = Objective_isReached(defaultObjective, goalState);

    cr_assert(reached == 1);
}

void assertReachingDefaultObjectiveWithPoseIs(int x, int y, int theta, int expectedResult)
{
    struct Pose *currentPose = Pose_new(x, y, theta);
    struct State *currentState = State_new(currentPose);

    int reached = Objective_isReached(defaultObjective, currentState);

    cr_assert(reached == expectedResult);

    Pose_delete(currentPose);
    State_delete(currentState);
}

Test(Objective_DefaultTolerance_Pose_X, given_UnderGoalMinusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int underGoal = GOAL_X - (2 * X_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(underGoal, GOAL_Y, GOAL_THETA, 0);
}

Test(Objective_DefaultTolerance_Pose_X, given_UnderGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int underGoalWithinTolerance = GOAL_X - (X_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(underGoalWithinTolerance, GOAL_Y, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_X, given_OverGoalPlusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int overGoal = GOAL_X + (2 * X_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(overGoal, GOAL_Y, GOAL_THETA, 0);
}

Test(Objective_DefaultTolerance_Pose_X, given_OverGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int overGoalWithinTolerance = GOAL_X + (X_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(overGoalWithinTolerance, GOAL_Y, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_Y, given_UnderGoalMinusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int underGoal = GOAL_Y - (2 * Y_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, underGoal, GOAL_THETA, 0);
}

Test(Objective_DefaultTolerance_Pose_Y, given_UnderGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int underGoalWithinTolerance = GOAL_Y - (Y_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, underGoalWithinTolerance, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_Y, given_OverGoalPlusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int overGoal = GOAL_Y + (2 * Y_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, overGoal, GOAL_THETA, 0);
}

Test(Objective_DefaultTolerance_Pose_Y, given_OverGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int overGoalWithinTolerance = GOAL_Y + (Y_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, overGoalWithinTolerance, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_THETA, given_UnderGoalMinusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int underGoal = GOAL_THETA - (2 * THETA_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, GOAL_Y, underGoal, 0);
}

Test(Objective_DefaultTolerance_Pose_THETA, given_UnderGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int underGoalWithinTolerance = GOAL_THETA - (THETA_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, GOAL_Y, underGoalWithinTolerance, 1);
}

Test(Objective_DefaultTolerance_Pose_THETA, given_OverGoalPlusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int overGoal = GOAL_THETA + (2 * THETA_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, GOAL_Y, overGoal, 0);
}

Test(Objective_DefaultTolerance_Pose_THETA, given_OverGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setupObjective, .fini = teardownObjective)
{
    int overGoalWithinTolerance = GOAL_THETA + (THETA_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, GOAL_Y, overGoalWithinTolerance, 1);
}

Test(Objective_DefaultTolerance, given_anOnSpotStateWithDifferentFlags_when_checksIfIsReached_isNotReached,
     .init = setupObjective, .fini = teardownObjective)
{
    struct Pose *currentPose = Pose_new(GOAL_X, GOAL_Y, GOAL_THETA);
    struct State *currentState = State_new(currentPose);
    Flags_setStartCycleSignalReceived(currentState->flags, 1);

    int reached = Objective_isReached(defaultObjective, currentState);

    cr_assert(reached == 0);

    Pose_delete(currentPose);
    State_delete(currentState);

}
*/
