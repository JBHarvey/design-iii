#include <time.h>
#include <stdlib.h>
#include "Timer.h"

struct Timer *Timer_new(void)
{
    struct Timer *pointer = malloc(sizeof(struct Timer));
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    pointer->current_time = 0.0;
    pointer->start_timestamp = time.tv_nsec;

    return pointer;
}

void Timer_delete(struct Timer *timer)
{
    free(timer);
}
