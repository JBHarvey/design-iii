#include <stdlib.h>
#include <stdio.h>
#include "Flags.h"


struct Flags *Flags_new(void)
{
    struct Object *new_object = Object_new();
    struct Flags *pointer = (struct Flags *) malloc(sizeof(struct Flags));

    pointer->object = new_object;

    pointer->startCycleSignalReceived = 0;

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
