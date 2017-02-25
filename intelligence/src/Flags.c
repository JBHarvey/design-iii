#include <stdlib.h>
#include <stdio.h>
#include "Flags.h"


struct Flags *Flags_new(void)
{
    struct Flags *pointer = (struct Flags *) malloc(sizeof(struct Flags));
    pointer->startCycleSignalReceived = 0;

    return pointer;
}

void Flags_delete(struct Flags *flags)
{
    free(flags);
}

void Flags_copyValuesFrom(struct Flags *recipient, struct Flags *source)
{
    recipient->startCycleSignalReceived = source->startCycleSignalReceived;
}


int Flags_haveTheSameValues(struct Flags *flags, struct Flags *otherFlags)
{
    return flags->startCycleSignalReceived == otherFlags->startCycleSignalReceived;
}

void Flags_setStartCycleSignalReceived(struct Flags *flags, int newValue)
{
    flags->startCycleSignalReceived = newValue;
}
