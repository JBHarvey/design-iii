#include <stdlib.h>
#include <stdio.h>
#include "Flags.h"


struct Flags *Flags_new(void)
{
    struct Object *new_object = Object_new();
    struct Flags *pointer = malloc(sizeof(struct Flags));

    pointer->object = new_object;

    pointer->start_cycle_signal_received = FALSE;
    pointer->picture_taken = FALSE;
    pointer->image_received_by_station = FALSE;
    pointer->planned_trajectory_received_by_station = FALSE;
    pointer->ready_to_start_received_by_station = FALSE;
    pointer->ready_to_draw_received_by_station = FALSE;
    pointer->end_of_cycle_received_by_station = FALSE;

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

void Flags_copyValuesFrom(struct Flags *recipient, struct Flags *source)
{
    recipient->start_cycle_signal_received = source->start_cycle_signal_received;
    recipient->picture_taken = source->picture_taken;
    recipient->image_received_by_station = source->image_received_by_station;
    recipient->planned_trajectory_received_by_station = source->planned_trajectory_received_by_station;
    recipient->ready_to_start_received_by_station = source->ready_to_start_received_by_station;
    recipient->ready_to_draw_received_by_station = source->ready_to_draw_received_by_station;
    recipient->end_of_cycle_received_by_station = source->end_of_cycle_received_by_station;
}


int Flags_haveTheSameValues(struct Flags *flags, struct Flags *other_flags)
{
    return
        (flags->start_cycle_signal_received == other_flags->start_cycle_signal_received
         || flags->start_cycle_signal_received == IRRELEVANT || other_flags->start_cycle_signal_received == IRRELEVANT)
        && (flags->picture_taken == other_flags->picture_taken
            || flags->picture_taken == IRRELEVANT || other_flags->picture_taken == IRRELEVANT)
        && (flags->image_received_by_station == other_flags->image_received_by_station
            || flags->image_received_by_station == IRRELEVANT || other_flags->image_received_by_station == IRRELEVANT)
        && (flags->planned_trajectory_received_by_station == other_flags->planned_trajectory_received_by_station
            || flags->planned_trajectory_received_by_station == IRRELEVANT
            || other_flags->planned_trajectory_received_by_station == IRRELEVANT)
        && (flags->ready_to_start_received_by_station == other_flags->ready_to_start_received_by_station
            || flags->ready_to_start_received_by_station == IRRELEVANT
            || other_flags->ready_to_start_received_by_station == IRRELEVANT)
        && (flags->ready_to_draw_received_by_station == other_flags->ready_to_draw_received_by_station
            || flags->ready_to_draw_received_by_station == IRRELEVANT
            || other_flags->ready_to_draw_received_by_station == IRRELEVANT)
        && (flags->end_of_cycle_received_by_station == other_flags->end_of_cycle_received_by_station
            || flags->end_of_cycle_received_by_station == IRRELEVANT || other_flags->end_of_cycle_received_by_station == IRRELEVANT
           );
}

void Flags_setStartCycleSignalReceived(struct Flags *flags, int new_value)
{
    flags->start_cycle_signal_received = new_value;
}

void Flags_setPictureTaken(struct Flags *flags, int new_value)
{
    flags->picture_taken = new_value;
}

void Flags_setImageReceivedByStation(struct Flags *flags, int new_value)
{
    flags->image_received_by_station = new_value;
}

void Flags_setPlannedTrajectoryReceivedByStation(struct Flags *flags, int new_value)
{
    flags->planned_trajectory_received_by_station = new_value;
}

void Flags_setReadyToStartReceivedByStation(struct Flags *flags, int new_value)
{
    flags->ready_to_start_received_by_station = new_value;
}

void Flags_setReadyToDrawReceivedByStation(struct Flags *flags, int new_value)
{
    flags->ready_to_draw_received_by_station = new_value;
}

void Flags_setEndOfCycleReceivedByStation(struct Flags *flags, int new_value)
{
    flags->end_of_cycle_received_by_station = new_value;
}

