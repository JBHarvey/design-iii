#include <criterion/criterion.h>
#include <time.h>
#include "Timer.h"

struct Timer *timer = NULL;

Test(Timer, creation_destruction)
{
    struct timespec time_before_init;
    struct timespec time_after_init;
    clock_gettime(CLOCK_REALTIME, &time_before_init);
    struct Timer *timer = Timer_new();
    clock_gettime(CLOCK_REALTIME, &time_after_init);

    cr_assert(timer->start_timestamp > time_before_init.tv_nsec);
    cr_assert(timer->start_timestamp < time_after_init.tv_nsec);

    Timer_delete(timer);
}

void setup_timer(void)
{
    timer = Timer_new();
}

void teardown_timer(void)
{
    Timer_delete(timer);
}

Test(Timer, given_aTimer_when_reset_then_theStartTimestampIsAReset
     , .init = setup_timer
     , .fini = teardown_timer)
{
    double initial_start_timestamp = timer->start_timestamp;

    Timer_reset(timer);

    cr_assert(timer->start_timestamp != initial_start_timestamp);
}

Test(Timer, given_aTimer_when_askingForElaspedTime_thenItReturnsTheCorrectTimeInNanoseconds
     , .init = setup_timer
     , .fini = teardown_timer)
{
    double elasped_time = Timer_elapsedTime(timer);

    cr_assert(elasped_time > 0);
}

