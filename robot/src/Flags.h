#ifndef FLAGS_H_
#define FLAGS_H_
#include "Object.h"

struct Flags {
    struct Object *object;
    int navigable_map_is_ready;
    int ready_to_start_received_by_station;
    int start_cycle_signal_received;
    int planned_trajectory_received_by_station;
    int picture_taken;
    int image_received_by_station;
    int ready_to_draw_received_by_station;
    int end_of_cycle_received_by_station;
};

struct Flags *Flags_new(void);
void Flags_delete(struct Flags  *flags);
struct Flags *Flags_irrelevant(void);

void Flags_copyValuesFrom(struct Flags *recipient, struct Flags *source);
int Flags_haveTheSameValues(struct Flags *flags, struct Flags *other_flags);

void Flags_setNavigableMapIsReady(struct Flags *flags, int new_value);
void Flags_setReadyToStartReceivedByStation(struct Flags *flags, int new_value);
void Flags_setStartCycleSignalReceived(struct Flags *flags, int new_value);
void Flags_setPlannedTrajectoryReceivedByStation(struct Flags *flags, int new_value);
void Flags_setPictureTaken(struct Flags *flags, int new_value);
void Flags_setImageReceivedByStation(struct Flags *flags, int new_value);
void Flags_setReadyToDrawReceivedByStation(struct Flags *flags, int new_value);
void Flags_setEndOfCycleReceivedByStation(struct Flags *flags, int new_value);

#endif // FLAGS_H_
