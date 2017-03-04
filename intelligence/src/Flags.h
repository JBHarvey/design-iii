#ifndef FLAGS_H_
#define FLAGS_H_
#include "Object.h"

#define TRUE 1
#define FALSE 0
#define IRRELEVANT -1

struct Flags {
    struct Object *object;
    int startCycleSignalReceived;
    int pictureTaken;
};

struct Flags *Flags_new(void);
void Flags_delete(struct Flags  *flags);

void Flags_copyValuesFrom(struct Flags *recipient, struct Flags *source);
int Flags_haveTheSameValues(struct Flags *flags, struct Flags *otherFlags);

void Flags_setStartCycleSignalReceived(struct Flags *flags, int newValue);
void Flags_setPictureTaken(struct Flags *flags, int newValue);



#endif // FLAGS_H_
