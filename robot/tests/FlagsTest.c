#include <criterion/criterion.h>
#include <stdio.h>
#include "Flags.h"

static struct Flags *flags;
static struct Flags *other_flags;
const int ZERO = 0;
const int START_CYCLE = 1;
const int PICTURE_TAKEN = 1;
const int IMAGE_RECEIVED_BY_STATION = 1;
const int MINUS_ONE = -1;

Test(Flags, creation_destruction)
{
    struct Flags *flags = Flags_new();
    cr_assert(flags->start_cycle_signal_received == ZERO);
    cr_assert(flags->picture_taken == ZERO);
    Flags_delete(flags);
}

void setup_flags(void)
{
    flags = Flags_new();
    other_flags = Flags_new();
    Flags_setStartCycleSignalReceived(flags, START_CYCLE);
    Flags_setStartCycleSignalReceived(other_flags, START_CYCLE);

}

void teardown_flags(void)
{
    Flags_delete(flags);
    Flags_delete(other_flags);
}

Test(Flags, given_twoFlagsWithSameValues_when_comparesThem_then_returnsTrue
     , .init = setup_flags
     , .fini = teardown_flags)
{
    int comparison = Flags_haveTheSameValues(flags, other_flags);
    cr_assert(comparison == 1);
}

Test(Flags, given_twoFlagsWithDifferentValues_when_comparesThem_then_returnsFalse
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setStartCycleSignalReceived(flags, ZERO);

    int comparison = Flags_haveTheSameValues(flags, other_flags);
    cr_assert(comparison == 0);
}

Test(Flags, given_twoFlagsWithDifferentValues_when_oneHasMinusOnesInItsValuesAndTheyAreCompared_then_returnsTrue
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setStartCycleSignalReceived(flags, MINUS_ONE);

    int comparison = Flags_haveTheSameValues(flags, other_flags);
    cr_assert(comparison == 1);
}

Test(Flags, given_twoFlagsWithDifferentValues_when_copied_then_theFlagsHaveTheSameValues
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setStartCycleSignalReceived(flags, ZERO);

    Flags_copyValuesFrom(flags, other_flags);

    int comparison = Flags_haveTheSameValues(flags, other_flags);
    cr_assert(comparison == 1);
}

Test(Flags_flag, given_defaultFlags_when_triesToSetStartCycleValueToOne_then_startCycleValueIsOne
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setStartCycleSignalReceived(flags, START_CYCLE);
    cr_assert(flags->start_cycle_signal_received == START_CYCLE);
}

Test(Flags_flag, given_defaultFlags_when_triesToSetPictureTakenToOne_then_pictureTakenIsOne
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setPictureTaken(flags, PICTURE_TAKEN);
    cr_assert(flags->picture_taken == PICTURE_TAKEN);
}

Test(Flags_flag, given_defaultFlags_when_triesToSetImageReceivedByStationToOne_then_imageReceivedByStationIsOne
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setImageReceivedByStation(flags, IMAGE_RECEIVED_BY_STATION);
    cr_assert(flags->image_received_by_station == IMAGE_RECEIVED_BY_STATION);
}

