#ifndef TIMER_H_
#define TIMER_H_

struct Timer {
    double current_time;
    double start_timestamp;
};

struct Timer *Timer_new(void);

void Timer_delete(struct Timer *timer);

#endif // TIMER_H_
