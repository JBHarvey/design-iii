#include <criterion/criterion.h>
#include <stdio.h>
#include "Pose.h"
#include "State.h"

const int SOME_X = 13256;
const int SOME_Y = 521651;
const int SOME_THETA = 11611;

struct State *state;
struct Pose *pose;

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

Test(State, given__when_aStateIsCreated_then_itHasAnAllZeroFlags
     , .init = setupState
     , .fini = teardownState)
{
    struct Flags *flags = Flags_new();

    int comparison = Flags_areTheSame(flags, state->flags);
    cr_assert(comparison);

    Flags_delete(flags);
}

Test(State, given_aState_when_updateFlagssFromAnotherState_then_theFlagsValuesOfTheStateAreCopied
     , .init = setupState
     , .fini = teardownState)
{
    struct Flags *flags = Flags_new();
    Flags_setStartCycleSignalRecieved(flags, 1);

    struct State *otherState = State_new(pose);
    Flags_copyValuesFrom(otherState->flags, flags);

    State_updateFlagsValuesFrom(state, otherState);

    int comparison = Flags_areTheSame(flags, state->flags);
    cr_assert(comparison);

    Flags_delete(flags);
    State_delete(otherState);
}
