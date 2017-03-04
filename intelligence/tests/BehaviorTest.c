#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"
#include "BehaviorBuilder.h"

struct State *currentState;

void setupBehavior(void)
{
    struct Pose *pose = Pose_new(DEFAULT_GOAL_X, DEFAULT_GOAL_Y, DEFAULT_GOAL_THETA);
    currentState = State_new(pose);
    Pose_delete(pose);
}

void teardownBehavior(void)
{
    State_delete(currentState);
}

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

Test(Behavior, given_aDefaultValuedBehaviorAndDefaultValuedState_when_checksIfIsReached_then_isReached
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();

    int isReached = Behavior_areEntryConditionsReached(behavior, currentState);
    cr_assert(isReached);

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

Test(Behavior, given_aBehaviorWithNoChild_when_askedToUpdateBehavior_then_returnsSelf
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *nextBehavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, currentState);

    cr_assert_eq(behavior, nextBehavior);

    Behavior_delete(behavior);
}

Test(Behavior, given_aBehaviorWithAReachableChild_when_askedToUpdateBehavior_then_returnsChild
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *child = BehaviorBuilder_default();

    Behavior_addChild(behavior, child);

    struct Behavior *nextBehavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, currentState);

    cr_assert_eq(child, nextBehavior);

    Behavior_delete(behavior);
    Behavior_delete(child);
}

Test(Behavior,
     given_aBehaviorWithAnNoreachableChild_when_askedToUpdateBehavior_then_returnsSelf
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *child = BehaviorBuilder_build(
                                 BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                         BehaviorBuilder_end()));

    Behavior_addChild(behavior, child);

    struct Behavior *nextBehavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, currentState);

    cr_assert_eq(behavior, nextBehavior);

    Behavior_delete(behavior);
    Behavior_delete(child);
}

Test(Behavior, given_aBehaviorWithOneChild_when_itReceivesANewChild_then_itIsPlacedLast
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *firstBehavior = BehaviorBuilder_default();
    struct Behavior *secondBehavior = BehaviorBuilder_build(
                                          BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                                  BehaviorBuilder_end()));

    Behavior_addChild(behavior, firstBehavior);
    Behavior_addChild(behavior, secondBehavior);

    cr_assert_eq(firstBehavior->nextSibling, secondBehavior);
    cr_assert_eq(secondBehavior->nextSibling, secondBehavior);

    Behavior_delete(behavior);
    Behavior_delete(firstBehavior);
    Behavior_delete(secondBehavior);
}

Test(Behavior,
     given_aBehavior_when_askedToAddHimselfToItsChildren_then_NothingHappens
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *child = BehaviorBuilder_build(
                                 BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                         BehaviorBuilder_end()));

    Behavior_addChild(behavior, behavior);
    Behavior_addChild(behavior, child);
    Behavior_addChild(behavior, behavior);

    cr_assert_eq(behavior->firstChild, child);
    cr_assert_eq(child->nextSibling, child);

    Behavior_delete(behavior);
    Behavior_delete(child);
}

Test(Behavior_WithTwoChildren,
     given_theFirstChildIsReachable_when_askedToUpdateBehavior_then_returnsFirstChild
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *reachableChild = BehaviorBuilder_default();
    struct Behavior *unreachableChild = BehaviorBuilder_build(
                                            BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                                    BehaviorBuilder_end()));

    Behavior_addChild(behavior, reachableChild);
    Behavior_addChild(behavior, unreachableChild);

    struct Behavior *nextBehavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, currentState);

    cr_assert_eq(reachableChild, nextBehavior);

    Behavior_delete(behavior);
    Behavior_delete(reachableChild);
    Behavior_delete(unreachableChild);
}

Test(Behavior_WithTwoChildren,
     given_theSecondChildIsReachable_when_askedToUpdateBehavior_then_returnsSecondChild
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *reachableChild = BehaviorBuilder_default();
    struct Behavior *unreachableChild = BehaviorBuilder_build(
                                            BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                                    BehaviorBuilder_end()));

    Behavior_addChild(behavior, unreachableChild);
    Behavior_addChild(behavior, reachableChild);

    struct Behavior *nextBehavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, currentState);

    cr_assert_eq(reachableChild, nextBehavior);

    Behavior_delete(behavior);
    Behavior_delete(reachableChild);
    Behavior_delete(unreachableChild);
}

Test(Behavior, given_aDefaultBehavior_when__then_itsActionIsDummyAction
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    void (*action)(struct Robot *) = &Behavior_dummyAction;

    cr_assert_eq(behavior->action, action);

    Behavior_delete(behavior);
}

Test(Behavior, given_aDefaultBehavior_when_changesItsAction_then_itsActionIsChanged
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    void (*action)(struct Robot *) = &Robot_takePicture;
    Behavior_changeAction(behavior, action);

    cr_assert_eq(behavior->action, action);

    Behavior_delete(behavior);
}

void BehaviorActionTest(struct Robot *robot)
{
    Robot_takePicture(robot);
}

Test(Behavior, given_aSpecificFunction_when_acts_then_theActionTakesPlace
     , .init = setupBehavior
     , .fini = teardownBehavior)
{
    struct Robot *robot = Robot_new();
    cr_assert(robot->currentState->flags->pictureTaken == FALSE);

    struct Behavior *behavior = BehaviorBuilder_build(
                                    BehaviorBuilder_withAction(&BehaviorActionTest,
                                            BehaviorBuilder_end()));

    Behavior_act(behavior, robot);

    cr_assert(robot->currentState->flags->pictureTaken == TRUE);

    Behavior_delete(behavior);
    Robot_delete(robot);
}
