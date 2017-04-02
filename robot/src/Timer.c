#include "Timer.h"

#define NUMBER_OF_NANOSECONDS_IN_A_SECOND 1.0e9

struct Timer *Timer_new(void)
{
    struct Timer *pointer = malloc(sizeof(struct Timer));
    struct Object *new_object = Object_new();
    pointer->object = new_object;
    clock_gettime(CLOCK_REALTIME, &(pointer->time));
    pointer->start_timestamp = pointer->time.tv_sec * NUMBER_OF_NANOSECONDS_IN_A_SECOND + pointer->time.tv_nsec;

    return pointer;
}

void Timer_delete(struct Timer *timer)
{
    Object_removeOneReference(timer->object);

    if(Object_canBeDeleted(timer->object)) {

        Object_delete(timer->object);
        free(timer);
    }

}

void Timer_reset(struct Timer *timer)
{
    clock_gettime(CLOCK_REALTIME, &(timer->time));
    timer->start_timestamp = timer->time.tv_sec * NUMBER_OF_NANOSECONDS_IN_A_SECOND + timer->time.tv_nsec;
}

double Timer_elapsedTime(struct Timer *timer)
{
    clock_gettime(CLOCK_REALTIME, &(timer->time));
    return (timer->time.tv_sec * NUMBER_OF_NANOSECONDS_IN_A_SECOND + timer->time.tv_nsec) - timer->start_timestamp;
}

int Timer_isTimePassed(struct Timer *timer, int seconds)
{
    return Timer_elapsedTime(timer) >= seconds * NUMBER_OF_NANOSECONDS_IN_A_SECOND;
}
