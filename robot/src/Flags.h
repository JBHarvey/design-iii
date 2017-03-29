#ifndef FLAGS_H_
#define FLAGS_H_
#include "Object.h"

struct Flags {
    struct Object *object;
    int start_cycle_signal_received;
    int picture_taken;
    int image_received_by_station;
    int planned_trajectory_received_by_station;
    int ready_to_start_received_by_station;
    int ready_to_draw_received_by_station;
    int end_of_cycle_received_by_station;
};

struct Flags *Flags_new(void);
void Flags_delete(struct Flags  *flags);

void Flags_copyValuesFrom(struct Flags *recipient, struct Flags *source);
int Flags_haveTheSameValues(struct Flags *flags, struct Flags *other_flags);

void Flags_setStartCycleSignalReceived(struct Flags *flags, int new_value);
void Flags_setPictureTaken(struct Flags *flags, int new_value);
void Flags_setImageReceivedByStation(struct Flags *flags, int new_value);
void Flags_setPlannedTrajectoryReceivedByStation(struct Flags *flags, int new_value);
void Flags_setReadyToStartReceivedByStation(struct Flags *flags, int new_value);
void Flags_setReadyToDrawReceivedByStation(struct Flags *flags, int new_value);
void Flags_setEndOfCycleReceivedByStation(struct Flags *flags, int new_value);

#endif // FLAGS_H_
