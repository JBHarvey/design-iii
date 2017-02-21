#include <criterion/criterion.h>
#include <stdio.h>
#include "State.h"
#include "Objective.h"

const int AIMED_X = 10000;       // 1 m
const int AIMED_Y = 10000;       // 1 m
const int AIMED_THETA = 7854;    // Pi/4 rad

static struct State* aimedState;
static struct Objective* defaultObjective;

struct Objective* buildADefaultToleranceObjective(void)
{
    struct Pose* defaultPoseTolerances = Pose_new(X_TOLERANCE_DEFAULT, Y_TOLERANCE_DEFAULT, THETA_TOLERANCE_DEFAULT);
    struct State* defaultTolerances = State_new(defaultPoseTolerances);
    struct Objective* defaultToleranceObjective = Objective_new(aimedState, defaultTolerances);
    return defaultToleranceObjective;
}

void setup(void)
{

    struct Pose* pose = Pose_new(AIMED_X, AIMED_Y, AIMED_THETA);
    aimedState = State_new(pose);
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
    struct Objective* zeroToleranceObjective = Objective_new(aimedState, zeroTolerances);
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
    struct Objective* objective = Objective_new(aimedState, tolerances);

    cr_assert(objective->aimedState == aimedState
              && objective->tolerances == tolerances);
    Objective_delete(objective);
}

Test(Objective, given_maxTolerances_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    struct State* maxTolerances = givenAMaxTolerance();
    struct Objective* reachableObjective = Objective_new(aimedState, maxTolerances);

    int reached = Objective_isReached(reachableObjective, aimedState);

    cr_assert(reached == 1);

    Objective_delete(reachableObjective);
}

Test(Objective, given_zeroTolerancesAndExactState_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    struct Objective* objective = givenAZeroToleranceObjective();

    int reached = Objective_isReached(objective, aimedState);

    cr_assert(reached == 1);

    Objective_delete(objective);
}

Test(Objective_DefaultTolerance, given_anOnSpotState_when_checksIfIsReached_then_isReached,
     .init = setup, .fini = teardown)
{
    int reached = Objective_isReached(defaultObjective, aimedState);

    cr_assert(reached == 1);
}
