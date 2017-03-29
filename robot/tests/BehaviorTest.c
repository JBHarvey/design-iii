#include <criterion/criterion.h>
#include <stdio.h>
#include "Robot.h"
#include "BehaviorBuilder.h"

struct State *current_state;

void setup_behavior(void)
{
    struct Pose *pose = Pose_new(DEFAULT_GOAL_X, DEFAULT_GOAL_Y, DEFAULT_GOAL_THETA);
    current_state = State_new(pose);
    Pose_delete(pose);
}

void teardown_behavior(void)
{
    State_delete(current_state);
}

Test(Behavior, creation_destruction)
{
    int x = 10000;
    int y = 10000;
    int theta = 78540;
    struct Pose *goal_pose = Pose_new(x, y, theta);
    struct State *goal_state = State_new(goal_pose);
    struct Pose *max_pose_tolerances = Pose_new(X_TOLERANCE_MAX, Y_TOLERANCE_MAX, THETA_TOLERANCE_MAX);
    struct State *tolerances = State_new(max_pose_tolerances);
    Pose_delete(max_pose_tolerances);
    struct Objective *objective = Objective_new(goal_state, tolerances);
    struct Behavior *behavior = Behavior_new(objective);

    cr_assert(behavior->entry_conditions == objective);

    Pose_delete(goal_pose);
    State_delete(tolerances);
    State_delete(goal_state);
    Objective_delete(objective);
    Behavior_delete(behavior);
}

Test(Behavior, given_aDefaultValuedBehaviorAndDefaultValuedState_when_checksIfIsReached_then_isReached
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();

    int is_reached = Behavior_areEntryConditionsReached(behavior, current_state);
    cr_assert(is_reached);

    Behavior_delete(behavior);
}

Test(Behavior, given_aDefaultValuedBehaviorAndOffValuedState_when_checksIfIsReached_then_isNotReached)
{
    struct Pose *off_pose = Pose_new(DEFAULT_GOAL_X, Y_TOLERANCE_MAX, DEFAULT_GOAL_THETA);
    struct State *off_state = State_new(off_pose);
    struct Behavior *behavior = BehaviorBuilder_default();

    int is_reached = Behavior_areEntryConditionsReached(behavior, off_state);
    cr_assert(is_reached == 0);

    Pose_delete(off_pose);
    State_delete(off_state);
    Behavior_delete(behavior);
}

Test(Behavior, given_aBehaviorWithNoChild_when_askedToUpdateBehavior_then_returnsSelf
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *next_behavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, current_state);

    cr_assert_eq(behavior, next_behavior);

    Behavior_delete(behavior);
}

Test(Behavior, given_aBehaviorWithAReachableChild_when_askedToUpdateBehavior_then_returnsChild
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *child = BehaviorBuilder_default();

    Behavior_addChild(behavior, child);

    struct Behavior *next_behavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, current_state);

    cr_assert_eq(child, next_behavior);

    Behavior_delete(behavior);
    Behavior_delete(child);
}

Test(Behavior,
     given_aBehaviorWithAnNoreachableChild_when_askedToUpdateBehavior_then_returnsSelf
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *child = BehaviorBuilder_build(
                                 BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                         BehaviorBuilder_end()));

    Behavior_addChild(behavior, child);

    struct Behavior *next_behavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, current_state);

    cr_assert_eq(behavior, next_behavior);

    Behavior_delete(behavior);
    Behavior_delete(child);
}

Test(Behavior, given_aBehaviorWithOneChild_when_itReceivesANewChild_then_itIsPlacedLast
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *first_behavior = BehaviorBuilder_default();
    struct Behavior *second_behavior = BehaviorBuilder_build(
                                           BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                                   BehaviorBuilder_end()));

    Behavior_addChild(behavior, first_behavior);
    Behavior_addChild(behavior, second_behavior);

    cr_assert_eq(first_behavior->next_sibling, second_behavior);
    cr_assert_eq(second_behavior->next_sibling, second_behavior);

    Behavior_delete(behavior);
    Behavior_delete(first_behavior);
    Behavior_delete(second_behavior);
}

Test(Behavior,
     given_aBehavior_when_askedToAddHimselfToItsChildren_then_NothingHappens
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *child = BehaviorBuilder_build(
                                 BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                         BehaviorBuilder_end()));

    Behavior_addChild(behavior, behavior);
    Behavior_addChild(behavior, child);
    Behavior_addChild(behavior, behavior);

    cr_assert_eq(behavior->first_child, child);
    cr_assert_eq(child->next_sibling, child);

    Behavior_delete(behavior);
    Behavior_delete(child);
}

Test(Behavior_WithTwoChildren,
     given_theFirstChildIsReachable_when_askedToUpdateBehavior_then_returnsFirstChild
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *reachable_child = BehaviorBuilder_default();
    struct Behavior *unreachable_child = BehaviorBuilder_build(
            BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                      BehaviorBuilder_end()));

    Behavior_addChild(behavior, reachable_child);
    Behavior_addChild(behavior, unreachable_child);

    struct Behavior *next_behavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, current_state);

    cr_assert_eq(reachable_child, next_behavior);

    Behavior_delete(behavior);
    Behavior_delete(reachable_child);
    Behavior_delete(unreachable_child);
}

Test(Behavior_WithTwoChildren,
     given_theSecondChildIsReachable_when_askedToUpdateBehavior_then_returnsSecondChild
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    struct Behavior *reachable_child = BehaviorBuilder_default();
    struct Behavior *unreachable_child = BehaviorBuilder_build(
            BehaviorBuilder_withGoalX(X_TOLERANCE_MAX + DEFAULT_GOAL_X,
                                      BehaviorBuilder_end()));

    Behavior_addChild(behavior, unreachable_child);
    Behavior_addChild(behavior, reachable_child);

    struct Behavior *next_behavior = Behavior_fetchFirstReachedChildOrReturnSelf(behavior, current_state);

    cr_assert_eq(reachable_child, next_behavior);

    Behavior_delete(behavior);
    Behavior_delete(reachable_child);
    Behavior_delete(unreachable_child);
}

Test(Behavior, given_aDefaultBehavior_when__then_itsActionIsIdleAction
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Behavior *behavior = BehaviorBuilder_default();
    void (*action)(struct Robot *) = &Behavior_idle;

    cr_assert_eq(behavior->action, action);

    Behavior_delete(behavior);
}

Test(Behavior, given_aDefaultBehavior_when_changesItsAction_then_itsActionIsChanged
     , .init = setup_behavior
     , .fini = teardown_behavior)
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
     , .init = setup_behavior
     , .fini = teardown_behavior)
{
    struct Robot *robot = Robot_new();
    cr_assert(robot->current_state->flags->picture_taken == FALSE);

    struct Behavior *behavior = BehaviorBuilder_build(
                                    BehaviorBuilder_withAction(&BehaviorActionTest,
                                            BehaviorBuilder_end()));

    Behavior_act(behavior, robot);

    cr_assert(robot->current_state->flags->picture_taken == TRUE);

    Behavior_delete(behavior);
    Robot_delete(robot);
}
