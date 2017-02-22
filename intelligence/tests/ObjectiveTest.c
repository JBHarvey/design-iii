#include <criterion/criterion.h>
#include <stdio.h>
#include "State.h"
#include "Objective.h"

const int GOAL_X = 10000;       // 1 m
const int GOAL_Y = 10000;       // 1 m
const int GOAL_THETA = 7854;    // Pi/4 rad

static struct State* goalState;
static struct Objective* defaultObjective;

struct Objective* buildADefaultToleranceObjective(void)
{
    struct Pose* defaultPoseTolerances = Pose_new(X_TOLERANCE_DEFAULT, Y_TOLERANCE_DEFAULT, THETA_TOLERANCE_DEFAULT);
    struct State* defaultTolerances = State_new(defaultPoseTolerances);
    struct Objective* defaultToleranceObjective = Objective_new(goalState, defaultTolerances);
    return defaultToleranceObjective;
}

void setup(void)
{

    struct Pose* pose = Pose_new(GOAL_X, GOAL_Y, GOAL_THETA);
    goalState = State_new(pose);
    defaultObjective = buildADefaultToleranceObjective();
}

void teardown(void)
{
    Objective_delete(defaultObjective);
}

struct Objective* givenAZeroToleranceObjective(void)
{
    struct Pose* zeroPoseTolerances = Pose_new(X_TOLERANCE_MIN, Y_TOLERANCE_MIN, THETA_TOLERANCE_MIN);
    struct State* zeroTolerances = State_new(zeroPoseTolerances);
    struct Objective* zeroToleranceObjective = Objective_new(goalState, zeroTolerances);
    return zeroToleranceObjective;
}

struct State* givenAMaxTolerance(void)
{
    struct Pose* maxPoseTolerances = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_MAX);
    struct State* maxTolerances = State_new(maxPoseTolerances);
    return maxTolerances;
}

Test(Objective, creation_destruction, .init = setup, .fini = teardown)
{
    struct State* tolerances = givenAMaxTolerance();
    struct Objective* objective = Objective_new(goalState, tolerances);

    cr_assert(objective->goalState == goalState
              && objective->tolerances == tolerances);
    Objective_delete(objective);
}

Test(Objective, given_maxTolerances_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    struct State* maxTolerances = givenAMaxTolerance();
    struct Objective* reachableObjective = Objective_new(goalState, maxTolerances);

    int reached = Objective_isReached(reachableObjective, goalState);

    cr_assert(reached == 1);

    Objective_delete(reachableObjective);
}

Test(Objective, given_zeroTolerancesAndExactState_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    struct Objective* objective = givenAZeroToleranceObjective();

    int reached = Objective_isReached(objective, goalState);

    cr_assert(reached == 1);

    Objective_delete(objective);
}

Test(Objective_DefaultTolerance, given_anOnSpotState_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    int reached = Objective_isReached(defaultObjective, goalState);

    cr_assert(reached == 1);
}

void assertReachingDefaultObjectiveWithPoseIs(int x, int y, int theta, int expectedResult)
{
    struct Pose* pose = Pose_new(x, y, theta);
    struct State* currentState = State_new(pose);

    int reached = Objective_isReached(defaultObjective, currentState);

    cr_assert(reached == expectedResult);

    State_delete(currentState);
}

Test(Objective_DefaultTolerance_Pose_X, given_UnderGoalMinusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup, .fini = teardown)
{
    int underGoal = GOAL_X - (2 * X_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(underGoal, GOAL_Y, GOAL_THETA, 0);

}

Test(Objective_DefaultTolerance_Pose_X, given_UnderGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    int underGoalWithinTolerance = GOAL_X - (X_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(underGoalWithinTolerance, GOAL_Y, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_X, given_OverGoalPlusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup, .fini = teardown)
{
    int overGoal = GOAL_X + (2 * X_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(overGoal, GOAL_Y, GOAL_THETA, 0);

}

Test(Objective_DefaultTolerance_Pose_X, given_OverGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    int overGoalWithinTolerance = GOAL_X + (X_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(overGoalWithinTolerance, GOAL_Y, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_Y, given_UnderGoalMinusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup, .fini = teardown)
{
    int underGoal = GOAL_Y - (2 * X_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, underGoal, GOAL_THETA, 0);

}

Test(Objective_DefaultTolerance_Pose_Y, given_UnderGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    int underGoalWithinTolerance = GOAL_Y - (Y_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, underGoalWithinTolerance, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_Y, given_OverGoalPlusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup, .fini = teardown)
{
    int overGoal = GOAL_Y + (2 * Y_TOLERANCE_DEFAULT);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, overGoal, GOAL_THETA, 0);

}

Test(Objective_DefaultTolerance_Pose_Y, given_OverGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    int overGoalWithinTolerance = GOAL_Y + (Y_TOLERANCE_DEFAULT / 2);
    assertReachingDefaultObjectiveWithPoseIs(GOAL_X, overGoalWithinTolerance, GOAL_THETA, 1);
}

//TODO: fails if outside THETA tolerance.
