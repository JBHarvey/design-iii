#include <criterion/criterion.h>
#include <stdio.h>
#include "State.h"
#include "Objective.h"


static struct State* aimedState;

void setup(void)
{
    const int SOME_X = 10000;       // 1 m
    const int SOME_Y = 10000;       // 1 m
    const int SOME_THETA = 7854;    // Pi/4 rad

    struct Pose* pose = Pose_new(SOME_X, SOME_Y, SOME_THETA);
    aimedState = State_new(pose);
}

void teardown(void)
{
    State_delete(aimedState);
}

struct State* givenAZeroTolerance(void)
{
    struct Pose* zeroPoseTolerances = Pose_new(X_TOLERANCE_MIN, Y_TOLERANCE_MIN, THETA_TOLERANCE_MIN);
    struct State* zeroTolerances = State_new(zeroPoseTolerances);
    return zeroTolerances;
}

struct State* givenADefaultTolerance(void)
{
    struct Pose* defaultPoseTolerances = Pose_new(X_TOLERANCE_DEFAULT, Y_TOLERANCE_DEFAULT, THETA_TOLERANCE_DEFAULT);
    struct State* defaultTolerances = State_new(defaultPoseTolerances);
    return defaultTolerances;
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
    struct Objective* objective = Objective_new(aimedState, tolerances);

    cr_assert(objective->aimedState == aimedState
              && objective->tolerances == tolerances);
    Objective_delete(objective);
}

Test(Objective, given_maxTolerances_when_validatesIfObjectiveIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    struct State* maxTolerances = givenAMaxTolerance();
    struct Objective* reachableObjective = Objective_new(aimedState, maxTolerances);

    int reached = Objective_isReached(reachableObjective, aimedState);

    cr_assert(reached == 1);
    Objective_delete(reachableObjective);
}
