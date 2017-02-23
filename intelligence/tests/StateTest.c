#include <criterion/criterion.h>
#include <stdio.h>
#include "Pose.h"
#include "State.h"

const int SOME_X = 13256;
const int SOME_Y = 521651;
const int SOME_THETA = 11611;

struct State* state;
struct Pose* pose;

void setupState(void)
{
    pose = Pose_new(SOME_X, SOME_Y, SOME_THETA);
    state = State_new(pose);
}

void teardownState(void)
{
    State_delete(state);
}

Test(State, creation_destruction
     , .init = setupState
     , .fini = teardownState)
{
    cr_assert(state->pose == pose);
}

Test(State, given__when_aStateIsCreated_then_itHasAnAllZeroFlag
     , .init = setupState
     , .fini = teardownState)
{
    struct Flag* flag = Flag_new();

    int comparison = Flag_areTheSame(flag, state->flag);
    cr_assert(comparison);

    Flag_delete(flag);
}

Test(State, given_aState_when_updateFlagsFromAnotherState_then_theFlagValuesOfTheStateAreCopied
     , .init = setupState
     , .fini = teardownState)
{
    struct Flag* flag = Flag_new();
    Flag_setStartCycleSignalRecieved(flag, 1);

    struct State* otherState = State_new(pose);
    Flag_copyValuesFrom(otherState->flag, flag);

    State_updateFlagValuesFrom(state, otherState);

    int comparison = Flag_areTheSame(flag, state->flag);
    cr_assert(comparison);

    Flag_delete(flag);
    State_delete(otherState);
}
