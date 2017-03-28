#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"
#include "BehaviorBuilder.h"

int validate_behavior_has_parameters(struct Behavior *built_behavior, int goalX, int goalY, int goal_theta,
                                     int tolerancesX,
                                     int tolerancesY, int tolerances_theta, struct Flags *goal_flags, void (*action)(struct Robot *))
{
    struct Objective *objective = built_behavior->entry_conditions;

    struct State *goal_state = objective->goal_state;
    struct Pose *goal_pose = goal_state->pose;
    struct Angle *goal_angle = goal_pose->angle;

    struct State *tolerances_state = objective->tolerances;
    struct Pose *tolerances_pose = tolerances_state->pose;
    struct Angle *tolerances_angle = tolerances_pose->angle;

    int are_equal = goal_pose->coordinates->x == goalX
                    && goal_pose->coordinates->y == goalY
                    && goal_angle->theta == goal_theta
                    && tolerances_pose->coordinates->x == tolerancesX
                    && tolerances_pose->coordinates->y == tolerancesY
                    && tolerances_angle->theta == tolerances_theta
                    && Flags_haveTheSameValues(goal_state->flags, goal_flags)
                    && built_behavior->action == action;
    return are_equal;
}

Test(BehaviorBuilder, given_noExtraParameters_when_buildingABehavior_then_returnsBehaviorWithDefaultObjective)
{
    struct Behavior *built_behavior = BehaviorBuilder_default();

    struct Flags* comparison_flags = Flags_new();

    int are_equal = validate_behavior_has_parameters(
                        built_behavior,
                        DEFAULT_GOAL_X,
                        DEFAULT_GOAL_Y,
                        DEFAULT_GOAL_THETA,
                        X_TOLERANCE_DEFAULT,
                        Y_TOLERANCE_DEFAULT,
                        THETA_TOLERANCE_DEFAULT,
                        comparison_flags,
                        &Behavior_idle);

    cr_assert(are_equal);

    Behavior_delete(built_behavior);
    Flags_delete(comparison_flags);
}

Test(BehaviorBuilder, given_aSpecificXGoalValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificX = 150;
    struct Behavior *built_behavior = BehaviorBuilder_build(
                                          BehaviorBuilder_withGoalX(specificX,
                                                  BehaviorBuilder_end()));

    struct Flags* comparison_flags = Flags_new();

    int are_equal = validate_behavior_has_parameters(
                        built_behavior,
                        specificX,
                        DEFAULT_GOAL_Y,
                        DEFAULT_GOAL_THETA,
                        X_TOLERANCE_DEFAULT,
                        Y_TOLERANCE_DEFAULT,
                        THETA_TOLERANCE_DEFAULT,
                        comparison_flags,
                        &Behavior_idle);

    cr_assert(are_equal);

    Behavior_delete(built_behavior);
    Flags_delete(comparison_flags);
}

Test(BehaviorBuilder, given_aSpecificYGoalValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificY = 1550;
    struct Behavior *built_behavior = BehaviorBuilder_build(
                                          BehaviorBuilder_withGoalY(specificY,
                                                  BehaviorBuilder_end()));

    struct Flags* comparison_flags = Flags_new();

    int are_equal = validate_behavior_has_parameters(
                        built_behavior,
                        DEFAULT_GOAL_X,
                        specificY,
                        DEFAULT_GOAL_THETA,
                        X_TOLERANCE_DEFAULT,
                        Y_TOLERANCE_DEFAULT,
                        THETA_TOLERANCE_DEFAULT,
                        comparison_flags,
                        &Behavior_idle);

    cr_assert(are_equal);

    Behavior_delete(built_behavior);
    Flags_delete(comparison_flags);
}

Test(BehaviorBuilder, given_aSpecificThetaGoalValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specific_theta = 2;
    struct Behavior *built_behavior = BehaviorBuilder_build(
                                          BehaviorBuilder_withGoalTheta(specific_theta,
                                                  BehaviorBuilder_end()));

    struct Flags* comparison_flags = Flags_new();

    int are_equal = validate_behavior_has_parameters(
                        built_behavior,
                        DEFAULT_GOAL_X,
                        DEFAULT_GOAL_Y,
                        specific_theta,
                        X_TOLERANCE_DEFAULT,
                        Y_TOLERANCE_DEFAULT,
                        THETA_TOLERANCE_DEFAULT,
                        comparison_flags,
                        &Behavior_idle);

    cr_assert(are_equal);

    Behavior_delete(built_behavior);
    Flags_delete(comparison_flags);
}

Test(BehaviorBuilder, given_aSpecificXToleranceValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificX = 101010;
    struct Behavior *built_behavior = BehaviorBuilder_build(
                                          BehaviorBuilder_withTolerancesX(specificX,
                                                  BehaviorBuilder_end()));

    struct Flags* comparison_flags = Flags_new();

    int are_equal = validate_behavior_has_parameters(
                        built_behavior,
                        DEFAULT_GOAL_X,
                        DEFAULT_GOAL_Y,
                        DEFAULT_GOAL_THETA,
                        specificX,
                        Y_TOLERANCE_DEFAULT,
                        THETA_TOLERANCE_DEFAULT,
                        comparison_flags,
                        &Behavior_idle);

    cr_assert(are_equal);

    Behavior_delete(built_behavior);
    Flags_delete(comparison_flags);
}

Test(BehaviorBuilder, given_aSpecificYToleranceValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificY = 42;
    struct Behavior *built_behavior = BehaviorBuilder_build(
                                          BehaviorBuilder_withTolerancesY(specificY,
                                                  BehaviorBuilder_end()));

    struct Flags* comparison_flags = Flags_new();

    int are_equal = validate_behavior_has_parameters(
                        built_behavior,
                        DEFAULT_GOAL_X,
                        DEFAULT_GOAL_Y,
                        DEFAULT_GOAL_THETA,
                        X_TOLERANCE_DEFAULT,
                        specificY,
                        THETA_TOLERANCE_DEFAULT,
                        comparison_flags,
                        &Behavior_idle);

    cr_assert(are_equal);

    Behavior_delete(built_behavior);
    Flags_delete(comparison_flags);
}

Test(BehaviorBuilder, given_aSpecificThetaToleranceValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specific_theta = PI / 2;
    struct Behavior *built_behavior = BehaviorBuilder_build(
                                          BehaviorBuilder_withTolerancesTheta(specific_theta,
                                                  BehaviorBuilder_end()));

    struct Flags* comparison_flags = Flags_new();

    int are_equal = validate_behavior_has_parameters(
                        built_behavior,
                        DEFAULT_GOAL_X,
                        DEFAULT_GOAL_Y,
                        DEFAULT_GOAL_THETA,
                        X_TOLERANCE_DEFAULT,
                        Y_TOLERANCE_DEFAULT,
                        specific_theta,
                        comparison_flags,
                        &Behavior_idle);

    cr_assert(are_equal);

    Behavior_delete(built_behavior);
    Flags_delete(comparison_flags);
}

Test(BehaviorBuilder, given_specificFlagsValues_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    struct Flags* comparison_flags = Flags_new();
    Flags_setStartCycleSignalReceived(comparison_flags, 1);
    struct Behavior *built_behavior = BehaviorBuilder_build(
                                          BehaviorBuilder_withFlags(comparison_flags,
                                                  BehaviorBuilder_end()));


    int are_equal = validate_behavior_has_parameters(
                        built_behavior,
                        DEFAULT_GOAL_X,
                        DEFAULT_GOAL_Y,
                        DEFAULT_GOAL_THETA,
                        X_TOLERANCE_DEFAULT,
                        Y_TOLERANCE_DEFAULT,
                        THETA_TOLERANCE_DEFAULT,
                        comparison_flags,
                        &Behavior_idle);

    cr_assert(are_equal);

    Behavior_delete(built_behavior);
    Flags_delete(comparison_flags);
}

void BehaviorBuilderTestAction(struct Robot *robot) {}

Test(BehaviorBuilder, given_specificFunctionAddress_when_buildingABehavior_then_theBehaviorFunctionIsCorrectlyChanged)
{
    struct Behavior *built_behavior = BehaviorBuilder_build(
                                          BehaviorBuilder_withAction(&BehaviorBuilderTestAction,
                                                  BehaviorBuilder_end()));


    cr_assert(built_behavior->action == &BehaviorBuilderTestAction);

    Behavior_delete(built_behavior);
}

Test(BehaviorBuilder, given_anExistingBehavior_when_buildingABehavior_then_returnsBehaviorWithSameValues)
{
    int specificX = 150;
    int specificY = 1550;
    int specific_theta = 2;
    int specificTolerancesX = 101010;
    int specificTolerancesY = 42;
    int specific_tolerances_theta = PI / 2;
    struct Flags* comparison_flags = Flags_new();
    Flags_setStartCycleSignalReceived(comparison_flags, 1);

    struct Behavior *base_behavior = BehaviorBuilder_build(
                                         BehaviorBuilder_withGoalX(specificX,
                                                 BehaviorBuilder_withGoalY(specificY,
                                                         BehaviorBuilder_withGoalTheta(specific_theta,
                                                                 BehaviorBuilder_withTolerancesX(specificTolerancesX,
                                                                         BehaviorBuilder_withTolerancesY(specificTolerancesY,
                                                                                 BehaviorBuilder_withTolerancesTheta(specific_tolerances_theta,
                                                                                         BehaviorBuilder_withFlags(comparison_flags,
                                                                                                 BehaviorBuilder_withAction(&BehaviorBuilderTestAction,
                                                                                                         BehaviorBuilder_end())))))))));
    struct Behavior *built_behavior = BehaviorBuilder_build(
                                          BehaviorBuilder_fromExisting(base_behavior,
                                                  BehaviorBuilder_end()));

    int are_equal = validate_behavior_has_parameters(
                        built_behavior,
                        specificX,
                        specificY,
                        specific_theta,
                        specificTolerancesX,
                        specificTolerancesY,
                        specific_tolerances_theta,
                        comparison_flags,
                        &BehaviorBuilderTestAction);

    cr_assert(are_equal);

    Behavior_delete(base_behavior);
    Behavior_delete(built_behavior);
    Flags_delete(comparison_flags);
}
