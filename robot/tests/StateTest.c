#include <criterion/criterion.h>
#include <stdio.h>
#include "Pose.h"
#include "State.h"

const int STATE_X = 13256;
const int STATE_Y = 521651;
const int STATE_THETA = 11611;

struct State *state;
struct Pose *pose;

void setup_state(void)
{
    pose = Pose_new(STATE_X, STATE_Y, STATE_THETA);
    state = State_new(pose);
}

void teardown_state(void)
{
    State_delete(state);
    Pose_delete(pose);
}

Test(State, creation_destruction
     , .init = setup_state
     , .fini = teardown_state)
{
    cr_assert(state->pose == pose);
}

Test(State, given__when_aStateIsCreated_then_itHasAnAllZeroFlags
     , .init = setup_state
     , .fini = teardown_state)
{
    struct Flags *flags = Flags_new();

    int comparison = Flags_haveTheSameValues(flags, state->flags);
    cr_assert(comparison);

    Flags_delete(flags);
}

Test(State, given_aState_when_updateFlagssFromAnotherState_then_theFlagsValuesOfTheStateAreCopied
     , .init = setup_state
     , .fini = teardown_state)
{
    struct Flags *flags = Flags_new();
    Flags_setStartCycleSignalReceived(flags, 1);

    struct State *other_state = State_new(pose);
    Flags_copyValuesFrom(other_state->flags, flags);

    State_updateFlagsValuesFrom(state, other_state);

    int comparison = Flags_haveTheSameValues(flags, state->flags);
    cr_assert(comparison);

    Flags_delete(flags);
    State_delete(other_state);
}
