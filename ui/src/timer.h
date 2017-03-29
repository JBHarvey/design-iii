#ifndef __TIMER
#define __TIMER

#include <gtk/gtk.h>

void Timer_initialize(GtkLabel *label);

void Timer_start(void);

void Timer_stop(void);

void Timer_release(void);

void Timer_redraw(void);

#endif // __TIMER

