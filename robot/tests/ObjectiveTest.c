#include <criterion/criterion.h>
#include <stdio.h>
#include "State.h"
#include "Objective.h"

const int GOAL_X = 10000;       // 1 m
const int GOAL_Y = 10000;       // 1 m
const int GOAL_THETA = 78540;    // Pi/4 rad

struct Pose *pose;
struct Pose *default_pose_tolerances;
struct State *default_tolerances;
struct State *goal_state;
struct Objective *default_objective;

void setup_objective(void)
{
    pose = Pose_new(GOAL_X, GOAL_Y, GOAL_THETA);
    goal_state = State_new(pose);

    default_pose_tolerances = Pose_new(X_TOLERANCE_MOVING, Y_TOLERANCE_MOVING, THETA_TOLERANCE_MOVING);
    default_tolerances = State_new(default_pose_tolerances);
    default_objective  = Objective_new(goal_state, default_tolerances);
}

void teardown_objective(void)
{
    Pose_delete(pose);
    Pose_delete(default_pose_tolerances);
    State_delete(default_tolerances);
    State_delete(goal_state);
    Objective_delete(default_objective);
}

Test(Objective, creation_destruction, .init = setup_objective, .fini = teardown_objective)
{
    struct Pose *max_pose_tolerances = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_MAX);
    struct State *tolerances = State_new(max_pose_tolerances);
    struct Objective *objective = Objective_new(goal_state, tolerances);

    cr_assert(objective->goal_state == goal_state
              && objective->tolerances == tolerances);

    Pose_delete(max_pose_tolerances);
    State_delete(tolerances);
    Objective_delete(objective);
}

Test(Objective, given_maxTolerances_when_checksIfIsReached_then_isReached,
     .init = setup_objective, .fini = teardown_objective)
{
    struct Pose *max_pose_tolerances = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_MAX);
    struct State *max_tolerances = State_new(max_pose_tolerances);
    struct Objective *reachable_objective = Objective_new(goal_state, max_tolerances);

    int reached = Objective_isReached(reachable_objective, goal_state);

    cr_assert(reached == 1);

    Pose_delete(max_pose_tolerances);
    State_delete(max_tolerances);
    Objective_delete(reachable_objective);
}

Test(Objective, given_zeroTolerancesAndExactState_when_checksIfIsReached_then_isReached,
     .init = setup_objective, .fini = teardown_objective)
{
    struct Pose *zero_pose_tolerances = Pose_new(X_TOLERANCE_MIN, Y_TOLERANCE_MIN, THETA_TOLERANCE_MIN);
    struct State *zero_tolerances = State_new(zero_pose_tolerances);
    struct Objective *zero_tolerance_objective = Objective_new(goal_state, zero_tolerances);

    int reached = Objective_isReached(zero_tolerance_objective, goal_state);

    cr_assert(reached == 1);

    Pose_delete(zero_pose_tolerances);
    State_delete(zero_tolerances);
    Objective_delete(zero_tolerance_objective);
}

Test(Objective_DefaultTolerance, given_anOnSpotState_when_checksIfIsReached_then_isReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int reached = Objective_isReached(default_objective, goal_state);

    cr_assert(reached == 1);
}

void assert_reaching_default_objective_with_pose_is(int x, int y, int theta, int expected_result)
{
    struct Pose *current_pose = Pose_new(x, y, theta);
    struct State *current_state = State_new(current_pose);

    int reached = Objective_isReached(default_objective, current_state);

    cr_assert(reached == expected_result,
              "Trying goal : (%d±%d,%d±%d)°(%d±%d) with pose : (%d,%d)°(%d) - \n_should %d (0: NotReach, 1: Reach)",
              default_objective->goal_state->pose->coordinates->x,
              default_objective->tolerances->pose->coordinates->x,
              default_objective->goal_state->pose->coordinates->y,
              default_objective->tolerances->pose->coordinates->y,
              default_objective->goal_state->pose->angle->theta,
              default_objective->tolerances->pose->angle->theta,
              x, y, theta, expected_result);

    Pose_delete(current_pose);
    State_delete(current_state);
}

Test(Objective_DefaultTolerance_Pose_X, given_UnderGoalMinusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int under_goal = GOAL_X - (2 * X_TOLERANCE_MOVING);
    assert_reaching_default_objective_with_pose_is(under_goal, GOAL_Y, GOAL_THETA, 0);
}

Test(Objective_DefaultTolerance_Pose_X, given_UnderGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int under_goal_within_tolerance = GOAL_X - (X_TOLERANCE_MOVING / 2);
    assert_reaching_default_objective_with_pose_is(under_goal_within_tolerance, GOAL_Y, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_X, given_OverGoalPlusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int over_goal = GOAL_X + (2 * X_TOLERANCE_MOVING);
    assert_reaching_default_objective_with_pose_is(over_goal, GOAL_Y, GOAL_THETA, 0);
}

Test(Objective_DefaultTolerance_Pose_X, given_OverGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int over_goal_within_tolerance = GOAL_X + (X_TOLERANCE_MOVING / 2);
    assert_reaching_default_objective_with_pose_is(over_goal_within_tolerance, GOAL_Y, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_Y, given_UnderGoalMinusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int under_goal = GOAL_Y - (2 * Y_TOLERANCE_MOVING);
    assert_reaching_default_objective_with_pose_is(GOAL_X, under_goal, GOAL_THETA, 0);
}

Test(Objective_DefaultTolerance_Pose_Y, given_UnderGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int under_goal_within_tolerance = GOAL_Y - (Y_TOLERANCE_MOVING / 2);
    assert_reaching_default_objective_with_pose_is(GOAL_X, under_goal_within_tolerance, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_Y, given_OverGoalPlusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int over_goal = GOAL_Y + (2 * Y_TOLERANCE_MOVING);
    assert_reaching_default_objective_with_pose_is(GOAL_X, over_goal, GOAL_THETA, 0);
}

Test(Objective_DefaultTolerance_Pose_Y, given_OverGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int over_goal_within_tolerance = GOAL_Y + (Y_TOLERANCE_MOVING / 2);
    assert_reaching_default_objective_with_pose_is(GOAL_X, over_goal_within_tolerance, GOAL_THETA, 1);
}

Test(Objective_DefaultTolerance_Pose_THETA, given_UnderGoalMinusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int under_goal = GOAL_THETA - (2 * THETA_TOLERANCE_MOVING);
    assert_reaching_default_objective_with_pose_is(GOAL_X, GOAL_Y, under_goal, 0);
}

Test(Objective_DefaultTolerance_Pose_THETA, given_UnderGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int under_goal_within_tolerance = GOAL_THETA - (THETA_TOLERANCE_MOVING / 2);
    assert_reaching_default_objective_with_pose_is(GOAL_X, GOAL_Y, under_goal_within_tolerance, 1);
}

Test(Objective_DefaultTolerance_Pose_THETA, given_OverGoalPlusTolerance_when_checksIfIsReached_then_isNotReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int over_goal = GOAL_THETA + (2 * THETA_TOLERANCE_MOVING);
    assert_reaching_default_objective_with_pose_is(GOAL_X, GOAL_Y, over_goal, 0);
}

Test(Objective_DefaultTolerance_Pose_THETA, given_OverGoalButInsideTolerance_when_checksIfIsReached_then_isReached,
     .init = setup_objective, .fini = teardown_objective)
{
    int over_goal_within_tolerance = GOAL_THETA + (THETA_TOLERANCE_MOVING / 2);
    assert_reaching_default_objective_with_pose_is(GOAL_X, GOAL_Y, over_goal_within_tolerance, 1);
}

Test(Objective_DefaultTolerance, given_anOnSpotStateWithDifferentFlags_when_checksIfIsReached_isNotReached,
     .init = setup_objective, .fini = teardown_objective)
{
    struct Pose *current_pose = Pose_new(GOAL_X, GOAL_Y, GOAL_THETA);
    struct State *current_state = State_new(current_pose);
    Flags_setStartCycleSignalReceived(current_state->flags, 1);

    int reached = Objective_isReached(default_objective, current_state);

    cr_assert(reached == 0);

    Pose_delete(current_pose);
    State_delete(current_state);

}

Test(Objective,
     given_aDefaultGoalWithMinusOneValuesInItsFlags_when_checksIfIsReached_theCorrespondingFlagsAreNotConsidered,
     .init = setup_objective, .fini = teardown_objective)
{
    struct Pose *current_pose = Pose_new(GOAL_X, GOAL_Y, GOAL_THETA);
    struct State *current_state = State_new(current_pose);
    Flags_setStartCycleSignalReceived(goal_state->flags, -1);

    int reached = Objective_isReached(default_objective, current_state);

    cr_assert(reached == 1);

    Pose_delete(current_pose);
    State_delete(current_state);

}

