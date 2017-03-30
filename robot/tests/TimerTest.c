#include <criterion/criterion.h>
#include <time.h>
#include "Timer.h"

Test(Timer, creation_destruction)
{
    struct timespec time_before_init;
    struct timespec time_after_init;
    clock_gettime(CLOCK_REALTIME, &time_before_init);
    struct Timer *timer = Timer_new();
    clock_gettime(CLOCK_REALTIME, &time_after_init);

    cr_assert(timer->current_time == 0.0);
    cr_assert(timer->start_timestamp > time_before_init.tv_nsec);
    cr_assert(timer->start_timestamp < time_after_init.tv_nsec);

    Timer_delete(timer);
}
