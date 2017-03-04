#include <criterion/criterion.h>
#include <stdio.h>
#include "Flags.h"

static struct Flags *flags;
static struct Flags *otherFlags;
const int ZERO = 0;
const int START_CYCLE = 1;
const int PICTURE_TAKEN = 1;
const int MINUS_ONE = -1;

Test(Flags, creation_destruction)
{
    struct Flags *flags = Flags_new();
    cr_assert(flags->startCycleSignalReceived == ZERO);
    cr_assert(flags->pictureTaken == ZERO);
    Flags_delete(flags);
}

void setupFlags(void)
{
    flags = Flags_new();
    otherFlags = Flags_new();
    Flags_setStartCycleSignalReceived(flags, START_CYCLE);
    Flags_setStartCycleSignalReceived(otherFlags, START_CYCLE);

}

void teardownFlags(void)
{
    Flags_delete(flags);
    Flags_delete(otherFlags);
}

Test(Flags, given_twoFlagsWithSameValues_when_comparesThem_then_returnsTrue
     , .init = setupFlags
     , .fini = teardownFlags)
{
    int comparison = Flags_haveTheSameValues(flags, otherFlags);
    cr_assert(comparison == 1);
}

Test(Flags, given_twoFlagsWithDifferentValues_when_comparesThem_then_returnsFalse
     , .init = setupFlags
     , .fini = teardownFlags)
{
    Flags_setStartCycleSignalReceived(flags, ZERO);

    int comparison = Flags_haveTheSameValues(flags, otherFlags);
    cr_assert(comparison == 0);
}

Test(Flags, given_twoFlagsWithDifferentValues_when_oneHasMinusOnesInItsValuesAndTheyAreCompared_then_returnsTrue
     , .init = setupFlags
     , .fini = teardownFlags)
{
    Flags_setStartCycleSignalReceived(flags, MINUS_ONE);

    int comparison = Flags_haveTheSameValues(flags, otherFlags);
    cr_assert(comparison == 1);
}

Test(Flags, given_twoFlagsWithDifferentValues_when_copied_then_theFlagsHaveTheSameValues
     , .init = setupFlags
     , .fini = teardownFlags)
{
    Flags_setStartCycleSignalReceived(flags, ZERO);

    Flags_copyValuesFrom(flags, otherFlags);

    int comparison = Flags_haveTheSameValues(flags, otherFlags);
    cr_assert(comparison == 1);
}

Test(Flags_flag, given_defaultFlags_when_triesToSetStartCycleValueToOne_then_startCycleValueIsOne
     , .init = setupFlags
     , .fini = teardownFlags)
{
    Flags_setStartCycleSignalReceived(flags, START_CYCLE);
    cr_assert(flags->startCycleSignalReceived == START_CYCLE);
}

Test(Flags_flag, given_defaultFlags_when_triesToSetPictureTakenToOne_then_pictureTakenIsOne
     , .init = setupFlags
     , .fini = teardownFlags)
{
    Flags_setPictureTaken(flags, PICTURE_TAKEN);
    cr_assert(flags->pictureTaken == PICTURE_TAKEN);
}

