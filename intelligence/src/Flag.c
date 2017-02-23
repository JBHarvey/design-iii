#include <stdlib.h>
#include <stdio.h>
#include "Flag.h"


struct Flag * Flag_new(void)
{
    struct Flag * pointer = (struct Flag *) malloc(sizeof(struct Flag));
    pointer->startCycleSignalRecieved = 0;

    return pointer;
}

void Flag_delete(struct Flag * flag)
{
    free(flag);
}

void Flag_copyValuesFrom(struct Flag* recipient, struct Flag* source)
{
    recipient->startCycleSignalRecieved = source->startCycleSignalRecieved;
}


int Flag_areTheSame(struct Flag* flag, struct Flag* otherFlag)
{
    return flag->startCycleSignalRecieved == otherFlag->startCycleSignalRecieved;
}

void Flag_setStartCycleSignalRecieved(struct Flag* flag, int newValue)
{
    flag->startCycleSignalRecieved = newValue;
}
