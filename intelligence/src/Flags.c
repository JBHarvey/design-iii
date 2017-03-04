#include <stdlib.h>
#include <stdio.h>
#include "Flags.h"


struct Flags *Flags_new(void)
{
    struct Object *new_object = Object_new();
    struct Flags *pointer = (struct Flags *) malloc(sizeof(struct Flags));

    pointer->object = new_object;

    pointer->startCycleSignalReceived = FALSE;
    pointer->pictureTaken = FALSE;

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
    recipient->pictureTaken = source->pictureTaken;
}


int Flags_haveTheSameValues(struct Flags *flags, struct Flags *otherFlags)
{
    return
        (flags->startCycleSignalReceived == otherFlags->startCycleSignalReceived
         || flags->startCycleSignalReceived == IRRELEVANT || otherFlags->startCycleSignalReceived == IRRELEVANT)
        && (flags->pictureTaken == otherFlags->pictureTaken
            || flags->pictureTaken == IRRELEVANT || otherFlags->pictureTaken == IRRELEVANT);
}

void Flags_setStartCycleSignalReceived(struct Flags *flags, int newValue)
{
    flags->startCycleSignalReceived = newValue;
}

void Flags_setPictureTaken(struct Flags *flags, int newValue)
{
    flags->pictureTaken = newValue;
}
