#ifndef FLAG_H_
#define FLAG_H_
#include "Angle.h"

struct Flag {
    int startCycleSignalRecieved;
};

struct Flag * Flag_new(void);
void Flag_delete(struct Flag * flag);

void Flag_copyValuesFrom(struct Flag* recipient, struct Flag* source);
int Flag_areTheSame(struct Flag* flag, struct Flag* otherFlag);

void Flag_setStartCycleSignalRecieved(struct Flag* flag, int newValue);



#endif // FLAG_H_
