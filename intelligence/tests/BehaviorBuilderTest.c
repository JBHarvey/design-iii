#include <criterion/criterion.h>
#include <stdio.h>
#include "BehaviorBuilder.h"

int validateBehaviorHasParameters(struct Behavior *builtBehavior, int goalX, int goalY, int goalTheta, int tolerancesX,
                                  int tolerancesY, int tolerancesTheta, struct Flags *goalFlags)
{
    struct Objective *objective = builtBehavior->entryConditions;

    struct State *goalState = objective->goalState;
    struct Pose *goalPose = goalState->pose;
    struct Angle *goalAngle = goalPose->angle;

    struct State *tolerancesState = objective->tolerances;
    struct Pose *tolerancesPose = tolerancesState->pose;
    struct Angle *tolerancesAngle = tolerancesPose->angle;

    int areEqual = goalPose->x == goalX
                   && goalPose->y == goalY
                   && goalAngle->theta == goalTheta
                   && tolerancesPose->x == tolerancesX
                   && tolerancesPose->y == tolerancesY
                   && tolerancesAngle->theta == tolerancesTheta
                   && Flags_haveTheSameValues(goalState->flags, goalFlags);
    return areEqual;
}

Test(BehaviorBuilder, given_noExtraParameters_when_buildingABehavior_then_returnsBehaviorWithDefaultObjective)
{
    struct Behavior *builtBehavior = BehaviorBuilder_default();

    struct Flags* comparisonFlags = Flags_new();

    int areEqual = validateBehaviorHasParameters(
                       builtBehavior,
                       DEFAULT_GOAL_X,
                       DEFAULT_GOAL_Y,
                       DEFAULT_GOAL_THETA,
                       X_TOLERANCE_DEFAULT,
                       Y_TOLERANCE_DEFAULT,
                       THETA_TOLERANCE_DEFAULT,
                       comparisonFlags);

    cr_assert(areEqual);

    Behavior_delete(builtBehavior);
    Flags_delete(comparisonFlags);
}

Test(BehaviorBuilder, given_aSpecificXGoalValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificX = 150;
    struct Behavior *builtBehavior = BehaviorBuilder_build(
                                         BehaviorBuilder_withGoalX(specificX,
                                                 BehaviorBuilder_end()));

    struct Flags* comparisonFlags = Flags_new();

    int areEqual = validateBehaviorHasParameters(
                       builtBehavior,
                       specificX,
                       DEFAULT_GOAL_Y,
                       DEFAULT_GOAL_THETA,
                       X_TOLERANCE_DEFAULT,
                       Y_TOLERANCE_DEFAULT,
                       THETA_TOLERANCE_DEFAULT,
                       comparisonFlags);

    cr_assert(areEqual);

    Behavior_delete(builtBehavior);
    Flags_delete(comparisonFlags);
}

Test(BehaviorBuilder, given_aSpecificYGoalValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificY = 1550;
    struct Behavior *builtBehavior = BehaviorBuilder_build(
                                         BehaviorBuilder_withGoalY(specificY,
                                                 BehaviorBuilder_end()));

    struct Flags* comparisonFlags = Flags_new();

    int areEqual = validateBehaviorHasParameters(
                       builtBehavior,
                       DEFAULT_GOAL_X,
                       specificY,
                       DEFAULT_GOAL_THETA,
                       X_TOLERANCE_DEFAULT,
                       Y_TOLERANCE_DEFAULT,
                       THETA_TOLERANCE_DEFAULT,
                       comparisonFlags);

    cr_assert(areEqual);

    Behavior_delete(builtBehavior);
    Flags_delete(comparisonFlags);
}

Test(BehaviorBuilder, given_aSpecificThetaGoalValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificTheta = 2;
    struct Behavior *builtBehavior = BehaviorBuilder_build(
                                         BehaviorBuilder_withGoalTheta(specificTheta,
                                                 BehaviorBuilder_end()));

    struct Flags* comparisonFlags = Flags_new();

    int areEqual = validateBehaviorHasParameters(
                       builtBehavior,
                       DEFAULT_GOAL_X,
                       DEFAULT_GOAL_Y,
                       specificTheta,
                       X_TOLERANCE_DEFAULT,
                       Y_TOLERANCE_DEFAULT,
                       THETA_TOLERANCE_DEFAULT,
                       comparisonFlags);

    cr_assert(areEqual);

    Behavior_delete(builtBehavior);
    Flags_delete(comparisonFlags);
}

Test(BehaviorBuilder, given_aSpecificXToleranceValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificX = 101010;
    struct Behavior *builtBehavior = BehaviorBuilder_build(
                                         BehaviorBuilder_withTolerancesX(specificX,
                                                 BehaviorBuilder_end()));

    struct Flags* comparisonFlags = Flags_new();

    int areEqual = validateBehaviorHasParameters(
                       builtBehavior,
                       DEFAULT_GOAL_X,
                       DEFAULT_GOAL_Y,
                       DEFAULT_GOAL_THETA,
                       specificX,
                       Y_TOLERANCE_DEFAULT,
                       THETA_TOLERANCE_DEFAULT,
                       comparisonFlags);

    cr_assert(areEqual);

    Behavior_delete(builtBehavior);
    Flags_delete(comparisonFlags);
}

Test(BehaviorBuilder, given_aSpecificYToleranceValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificY = 42;
    struct Behavior *builtBehavior = BehaviorBuilder_build(
                                         BehaviorBuilder_withTolerancesY(specificY,
                                                 BehaviorBuilder_end()));

    struct Flags* comparisonFlags = Flags_new();

    int areEqual = validateBehaviorHasParameters(
                       builtBehavior,
                       DEFAULT_GOAL_X,
                       DEFAULT_GOAL_Y,
                       DEFAULT_GOAL_THETA,
                       X_TOLERANCE_DEFAULT,
                       specificY,
                       THETA_TOLERANCE_DEFAULT,
                       comparisonFlags);

    cr_assert(areEqual);

    Behavior_delete(builtBehavior);
    Flags_delete(comparisonFlags);
}

Test(BehaviorBuilder, given_aSpecificThetaToleranceValue_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    int specificTheta = PI / 2;
    struct Behavior *builtBehavior = BehaviorBuilder_build(
                                         BehaviorBuilder_withTolerancesTheta(specificTheta,
                                                 BehaviorBuilder_end()));

    struct Flags* comparisonFlags = Flags_new();

    int areEqual = validateBehaviorHasParameters(
                       builtBehavior,
                       DEFAULT_GOAL_X,
                       DEFAULT_GOAL_Y,
                       DEFAULT_GOAL_THETA,
                       X_TOLERANCE_DEFAULT,
                       Y_TOLERANCE_DEFAULT,
                       specificTheta,
                       comparisonFlags);

    cr_assert(areEqual);

    Behavior_delete(builtBehavior);
    Flags_delete(comparisonFlags);
}

Test(BehaviorBuilder, given_specificFlagsValues_when_buildingABehavior_then_onlyTheRequiredValueIsChanged)
{
    struct Flags* comparisonFlags = Flags_new();
    Flags_setStartCycleSignalReceived(comparisonFlags, 1);
    struct Behavior *builtBehavior = BehaviorBuilder_build(
                                         BehaviorBuilder_withFlags(comparisonFlags,
                                                 BehaviorBuilder_end()));


    int areEqual = validateBehaviorHasParameters(
                       builtBehavior,
                       DEFAULT_GOAL_X,
                       DEFAULT_GOAL_Y,
                       DEFAULT_GOAL_THETA,
                       X_TOLERANCE_DEFAULT,
                       Y_TOLERANCE_DEFAULT,
                       THETA_TOLERANCE_DEFAULT,
                       comparisonFlags);

    cr_assert(areEqual);

    Behavior_delete(builtBehavior);
    Flags_delete(comparisonFlags);
}

Test(BehaviorBuilder, given_anExistingBehavior_when_buildingABehavior_then_returnsBehaviorWithSameValues)
{
    int specificX = 150;
    int specificY = 1550;
    int specificTheta = 2;
    int specificTolerancesX = 101010;
    int specificTolerancesY = 42;
    int specificTolerancesTheta = PI / 2;
    struct Flags* comparisonFlags = Flags_new();
    Flags_setStartCycleSignalReceived(comparisonFlags, 1);

    struct Behavior *baseBehavior = BehaviorBuilder_build(
                                        BehaviorBuilder_withGoalX(specificX,
                                                BehaviorBuilder_withGoalY(specificY,
                                                        BehaviorBuilder_withGoalTheta(specificTheta,
                                                                BehaviorBuilder_withTolerancesX(specificTolerancesX,
                                                                        BehaviorBuilder_withTolerancesY(specificTolerancesY,
                                                                                BehaviorBuilder_withTolerancesTheta(specificTolerancesTheta,
                                                                                        BehaviorBuilder_withFlags(comparisonFlags,
                                                                                                BehaviorBuilder_end()))))))));
    struct Behavior *builtBehavior = BehaviorBuilder_build(
                                         BehaviorBuilder_fromExisting(baseBehavior,
                                                 BehaviorBuilder_end()));

    int areEqual = validateBehaviorHasParameters(
                       builtBehavior,
                       specificX,
                       specificY,
                       specificTheta,
                       specificTolerancesX,
                       specificTolerancesY,
                       specificTolerancesTheta,
                       comparisonFlags);

    cr_assert(areEqual);

    Behavior_delete(baseBehavior);
    Behavior_delete(builtBehavior);
    Flags_delete(comparisonFlags);
}
