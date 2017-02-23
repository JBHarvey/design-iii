#include <criterion/criterion.h>
#include <stdio.h>
#include "Flag.h"

static struct Flag* flag;
static struct Flag* otherFlag;
const int ZERO = 0;
const int START_CYCLE = 1;

Test(Flag, creation_destruction)
{
    struct Flag* flag = Flag_new();
    cr_assert(flag->startCycleSignalRecieved == ZERO);
    Flag_delete(flag);
}

void setupFlag(void)
{
    flag = Flag_new();
    otherFlag = Flag_new();
    Flag_setStartCycleSignalRecieved(flag, START_CYCLE);
    Flag_setStartCycleSignalRecieved(otherFlag, START_CYCLE);

}

void teardownFlag(void)
{
    Flag_delete(flag);
    Flag_delete(otherFlag);
}

Test(Flag, given_defaultFlag_when_triesToSetCycleValueToOne_then_cycleValueIsOne
     , .init = setupFlag
     , .fini = teardownFlag)
{
    cr_assert(flag->startCycleSignalRecieved == START_CYCLE);
}

Test(Flag, given_twoFlagWithSameValues_when_comparesThem_then_returnsTrue
     , .init = setupFlag
     , .fini = teardownFlag)
{
    int comparison = Flag_areTheSame(flag, otherFlag);
    cr_assert(comparison == 1);
}

Test(Flag, given_twoFlagWithDifferentValues_when_comparesThem_then_returnsFalse
     , .init = setupFlag
     , .fini = teardownFlag)
{
    Flag_setStartCycleSignalRecieved(flag, ZERO);

    int comparison = Flag_areTheSame(flag, otherFlag);
    cr_assert(comparison == 0);
}

Test(Flag, given_twoFlagWithDifferentValues_when_copied_then_theFlagAreTheSame
     , .init = setupFlag
     , .fini = teardownFlag)
{
    Flag_setStartCycleSignalRecieved(flag, ZERO);

    Flag_copyValuesFrom(flag, otherFlag);

    int comparison = Flag_areTheSame(flag, otherFlag);
    cr_assert(comparison == 1);
}
