#include <stdlib.h>
#include <stdio.h>
#include "Flags.h"


struct Flags *Flags_new(void)
{
    struct Object *new_object = Object_new();
    struct Flags *pointer = malloc(sizeof(struct Flags));

    pointer->object = new_object;

    pointer->navigable_map_is_ready = FALSE;
    pointer->ready_to_start_received_by_station = FALSE;
    pointer->start_cycle_signal_received = FALSE;
    pointer->planned_trajectory_received_by_station = FALSE;
    pointer->picture_taken = FALSE;
    pointer->image_received_by_station = FALSE;
    pointer->ready_to_draw_received_by_station = FALSE;
    pointer->end_of_cycle_received_by_station = FALSE;
    pointer->manchester_code_received = FALSE;

    return pointer;
}

void Flags_delete(struct Flags *flags)
{
    Object_removeOneReference(flags->object);

    if(Object_canBeDeleted(flags->object)) {
        Object_delete(flags->object);
        free(flags);
    }
}

struct Flags *Flags_irrelevant(void)
{
    struct Flags *irrelevant = Flags_new();
    Flags_setNavigableMapIsReady(irrelevant, IRRELEVANT);
    Flags_setReadyToStartReceivedByStation(irrelevant, IRRELEVANT);
    Flags_setStartCycleSignalReceived(irrelevant, IRRELEVANT);
    Flags_setPlannedTrajectoryReceivedByStation(irrelevant, IRRELEVANT);
    Flags_setPictureTaken(irrelevant, IRRELEVANT);
    Flags_setImageReceivedByStation(irrelevant, IRRELEVANT);
    Flags_setReadyToDrawReceivedByStation(irrelevant, IRRELEVANT);
    Flags_setEndOfCycleReceivedByStation(irrelevant, IRRELEVANT);
    Flags_setManchesterCodeReceived(irrelevant, IRRELEVANT);

    return irrelevant;
}

void Flags_copyValuesFrom(struct Flags *recipient, struct Flags *source)
{
    recipient->navigable_map_is_ready = source->navigable_map_is_ready;
    recipient->ready_to_start_received_by_station = source->ready_to_start_received_by_station;
    recipient->start_cycle_signal_received = source->start_cycle_signal_received;
    recipient->planned_trajectory_received_by_station = source->planned_trajectory_received_by_station;
    recipient->picture_taken = source->picture_taken;
    recipient->image_received_by_station = source->image_received_by_station;
    recipient->ready_to_draw_received_by_station = source->ready_to_draw_received_by_station;
    recipient->end_of_cycle_received_by_station = source->end_of_cycle_received_by_station;
    recipient->manchester_code_received = source->manchester_code_received;
}


int Flags_haveTheSameValues(struct Flags *flags, struct Flags *other_flags)
{
    return
        (flags->navigable_map_is_ready == other_flags->navigable_map_is_ready || flags->navigable_map_is_ready == IRRELEVANT
         || other_flags->navigable_map_is_ready == IRRELEVANT)
        && (flags->ready_to_start_received_by_station == other_flags->ready_to_start_received_by_station
            || flags->ready_to_start_received_by_station == IRRELEVANT
            || other_flags->ready_to_start_received_by_station == IRRELEVANT)
        && (flags->start_cycle_signal_received == other_flags->start_cycle_signal_received
            || flags->start_cycle_signal_received == IRRELEVANT || other_flags->start_cycle_signal_received == IRRELEVANT)
        && (flags->planned_trajectory_received_by_station == other_flags->planned_trajectory_received_by_station
            || flags->planned_trajectory_received_by_station == IRRELEVANT
            || other_flags->planned_trajectory_received_by_station == IRRELEVANT)
        && (flags->picture_taken == other_flags->picture_taken || flags->picture_taken == IRRELEVANT
            || other_flags->picture_taken == IRRELEVANT)
        && (flags->image_received_by_station == other_flags->image_received_by_station
            || flags->image_received_by_station == IRRELEVANT || other_flags->image_received_by_station == IRRELEVANT)
        && (flags->ready_to_draw_received_by_station == other_flags->ready_to_draw_received_by_station
            || flags->ready_to_draw_received_by_station == IRRELEVANT
            || other_flags->ready_to_draw_received_by_station == IRRELEVANT)
        && (flags->end_of_cycle_received_by_station == other_flags->end_of_cycle_received_by_station
            || flags->end_of_cycle_received_by_station == IRRELEVANT || other_flags->end_of_cycle_received_by_station == IRRELEVANT)
        && (flags->manchester_code_received == other_flags->manchester_code_received
            || flags->manchester_code_received == IRRELEVANT
            || other_flags->manchester_code_received == IRRELEVANT
           );
}

void Flags_setNavigableMapIsReady(struct Flags *flags, int new_value)
{
    flags->navigable_map_is_ready = new_value;
}

void Flags_setReadyToStartReceivedByStation(struct Flags *flags, int new_value)
{
    flags->ready_to_start_received_by_station = new_value;
}

void Flags_setStartCycleSignalReceived(struct Flags *flags, int new_value)
{
    flags->start_cycle_signal_received = new_value;
}

void Flags_setPlannedTrajectoryReceivedByStation(struct Flags *flags, int new_value)
{
    flags->planned_trajectory_received_by_station = new_value;
}

void Flags_setPictureTaken(struct Flags *flags, int new_value)
{
    flags->picture_taken = new_value;
}

void Flags_setImageReceivedByStation(struct Flags *flags, int new_value)
{
    flags->image_received_by_station = new_value;
}

void Flags_setReadyToDrawReceivedByStation(struct Flags *flags, int new_value)
{
    flags->ready_to_draw_received_by_station = new_value;
}

void Flags_setEndOfCycleReceivedByStation(struct Flags *flags, int new_value)
{
    flags->end_of_cycle_received_by_station = new_value;
}

void Flags_setManchesterCodeReceived(struct Flags *flags, int new_value)
{
    flags->manchester_code_received = new_value;
}
