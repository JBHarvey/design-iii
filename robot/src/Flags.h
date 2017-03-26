#ifndef FLAGS_H_
#define FLAGS_H_
#include "Object.h"

struct Flags {
    struct Object *object;
    int start_cycle_signal_received;
    int picture_taken;
    int image_received_by_station;
};

struct Flags *Flags_new(void);
void Flags_delete(struct Flags  *flags);

void Flags_copyValuesFrom(struct Flags *recipient, struct Flags *source);
int Flags_haveTheSameValues(struct Flags *flags, struct Flags *other_flags);

void Flags_setStartCycleSignalReceived(struct Flags *flags, int new_value);
void Flags_setPictureTaken(struct Flags *flags, int new_value);
void Flags_setImageReceivedByStation(struct Flags *flags, int new_value);

#endif // FLAGS_H_
