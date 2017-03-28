#include <criterion/criterion.h>
#include <stdio.h>
#include "Flags.h"

static struct Flags *flags;
static struct Flags *other_flags;
const int ZERO = 0;
const int MINUS_ONE = -1;
const int NAVIGABLE_MAP_IS_READY = 1;
const int READY_TO_START_RECEIVED_BY_STATION = 1;
const int START_CYCLE = 1;
const int PLANNED_TRAJECTORY_RECEIVED_BY_STATION = 1;
const int PICTURE_TAKEN = 1;
const int IMAGE_RECEIVED_BY_STATION = 1;
const int READY_TO_DRAW_RECEIVED_BY_STATION = 1;
const int END_OF_CYCLE_RECEIVED_BY_STATION = 1;

Test(Flags, creation_destruction)
{
    struct Flags *flags = Flags_new();
    cr_assert(flags->navigable_map_is_ready == ZERO);
    cr_assert(flags->ready_to_start_received_by_station == ZERO);
    cr_assert(flags->start_cycle_signal_received == ZERO);
    cr_assert(flags->planned_trajectory_received_by_station == ZERO);
    cr_assert(flags->picture_taken == ZERO);
    cr_assert(flags->image_received_by_station == ZERO);
    cr_assert(flags->ready_to_draw_received_by_station == ZERO);
    cr_assert(flags->end_of_cycle_received_by_station == ZERO);
    Flags_delete(flags);
}

void setup_flags(void)
{
    flags = Flags_new();
    other_flags = Flags_new();

    Flags_setNavigableMapIsReady(flags, NAVIGABLE_MAP_IS_READY);
    Flags_setReadyToStartReceivedByStation(flags, READY_TO_START_RECEIVED_BY_STATION);
    Flags_setStartCycleSignalReceived(flags, START_CYCLE);
    Flags_setPlannedTrajectoryReceivedByStation(flags, PLANNED_TRAJECTORY_RECEIVED_BY_STATION);
    Flags_setPictureTaken(flags, PLANNED_TRAJECTORY_RECEIVED_BY_STATION);
    Flags_setImageReceivedByStation(flags, IMAGE_RECEIVED_BY_STATION);
    Flags_setReadyToDrawReceivedByStation(flags, READY_TO_DRAW_RECEIVED_BY_STATION);
    Flags_setEndOfCycleReceivedByStation(flags, END_OF_CYCLE_RECEIVED_BY_STATION);

    Flags_setNavigableMapIsReady(flags, NAVIGABLE_MAP_IS_READY);
    Flags_setReadyToStartReceivedByStation(other_flags, READY_TO_START_RECEIVED_BY_STATION);
    Flags_setStartCycleSignalReceived(other_flags, START_CYCLE);
    Flags_setPlannedTrajectoryReceivedByStation(other_flags, PLANNED_TRAJECTORY_RECEIVED_BY_STATION);
    Flags_setPictureTaken(other_flags, PLANNED_TRAJECTORY_RECEIVED_BY_STATION);
    Flags_setImageReceivedByStation(other_flags, IMAGE_RECEIVED_BY_STATION);
    Flags_setReadyToDrawReceivedByStation(other_flags, READY_TO_DRAW_RECEIVED_BY_STATION);
    Flags_setEndOfCycleReceivedByStation(other_flags, END_OF_CYCLE_RECEIVED_BY_STATION);
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
    Flags_setNavigableMapIsReady(flags, ZERO);
    Flags_setReadyToStartReceivedByStation(flags, ZERO);
    Flags_setStartCycleSignalReceived(flags, ZERO);
    Flags_setPlannedTrajectoryReceivedByStation(flags, ZERO);
    Flags_setPictureTaken(flags, ZERO);
    Flags_setImageReceivedByStation(flags, ZERO);
    Flags_setReadyToDrawReceivedByStation(flags, ZERO);
    Flags_setEndOfCycleReceivedByStation(flags, ZERO);

    int comparison = Flags_haveTheSameValues(flags, other_flags);
    cr_assert(comparison == 0);
}

Test(Flags, given_twoFlagsWithDifferentValues_when_oneHasMinusOnesInItsValuesAndTheyAreCompared_then_returnsTrue
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setNavigableMapIsReady(flags, MINUS_ONE);
    Flags_setReadyToStartReceivedByStation(flags, MINUS_ONE);
    Flags_setStartCycleSignalReceived(flags, MINUS_ONE);
    Flags_setPlannedTrajectoryReceivedByStation(flags, MINUS_ONE);
    Flags_setPictureTaken(flags, MINUS_ONE);
    Flags_setImageReceivedByStation(flags, MINUS_ONE);
    Flags_setReadyToDrawReceivedByStation(flags, MINUS_ONE);
    Flags_setEndOfCycleReceivedByStation(flags, MINUS_ONE);

    int comparison = Flags_haveTheSameValues(flags, other_flags);
    cr_assert(comparison == 1);
}

Test(Flags, given_twoFlagsWithDifferentValues_when_copied_then_theFlagsHaveTheSameValues
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setNavigableMapIsReady(flags, ZERO);
    Flags_setReadyToStartReceivedByStation(flags, ZERO);
    Flags_setStartCycleSignalReceived(flags, ZERO);
    Flags_setPlannedTrajectoryReceivedByStation(flags, ZERO);
    Flags_setPictureTaken(flags, ZERO);
    Flags_setImageReceivedByStation(flags, ZERO);
    Flags_setReadyToDrawReceivedByStation(flags, ZERO);
    Flags_setEndOfCycleReceivedByStation(flags, ZERO);

    Flags_copyValuesFrom(flags, other_flags);

    int comparison = Flags_haveTheSameValues(flags, other_flags);
    cr_assert(comparison == 1);
}

Test(Flags_flag,
     given_defaultFlags_when_triesToSetNavigableMapIsReadyToOne_then_navigableMapIsReadyIsOne
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setNavigableMapIsReady(flags, NAVIGABLE_MAP_IS_READY);
    cr_assert(flags->navigable_map_is_ready == NAVIGABLE_MAP_IS_READY);
}

Test(Flags_flag,
     given_defaultFlags_when_triesToSetReadyToStartReceivedByStationToOne_then_readyToStartReceivedByStationIsOne
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setReadyToStartReceivedByStation(flags, READY_TO_START_RECEIVED_BY_STATION);
    cr_assert(flags->ready_to_start_received_by_station == READY_TO_START_RECEIVED_BY_STATION);
}

Test(Flags_flag, given_defaultFlags_when_triesToSetStartCycleValueToOne_then_startCycleValueIsOne
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setStartCycleSignalReceived(flags, START_CYCLE);
    cr_assert(flags->start_cycle_signal_received == START_CYCLE);
}

Test(Flags_flag,
     given_defaultFlags_when_triesToSetPlannedTrajectoryReceivedByStationToOne_then_plannedTrajectoryReceivedByStationIsOne
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setPlannedTrajectoryReceivedByStation(flags, PLANNED_TRAJECTORY_RECEIVED_BY_STATION);
    cr_assert(flags->planned_trajectory_received_by_station == PLANNED_TRAJECTORY_RECEIVED_BY_STATION);
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

Test(Flags_flag,
     given_defaultFlags_when_triesToSetReadyToDrawReceivedByStationToOne_then_readyToStartReceivedByStationIsOne
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setReadyToDrawReceivedByStation(flags, READY_TO_DRAW_RECEIVED_BY_STATION);
    cr_assert(flags->ready_to_draw_received_by_station == READY_TO_DRAW_RECEIVED_BY_STATION);
}

Test(Flags_flag,
     given_defaultFlags_when_triesToSetEndOfCycleReceivedByStationToOne_then_endOfCycleReceivedByStationIsOne
     , .init = setup_flags
     , .fini = teardown_flags)
{
    Flags_setEndOfCycleReceivedByStation(flags, END_OF_CYCLE_RECEIVED_BY_STATION);
    cr_assert(flags->end_of_cycle_received_by_station == END_OF_CYCLE_RECEIVED_BY_STATION);
}
