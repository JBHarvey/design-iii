#ifndef TIMER_H_
#define TIMER_H_

#include "Object.h"
#include <time.h>
#include <stdlib.h>

struct Timer {
    struct Object *object; 
    struct timespec time;
    double start_timestamp;
};

struct Timer *Timer_new(void);

void Timer_delete(struct Timer *timer);

void Timer_reset(struct Timer *timer);

#endif // TIMER_H_
