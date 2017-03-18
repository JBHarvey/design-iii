#include "timer.h"
#include "logger.h"

/* Constants */

const int NUMBER_OF_SECONDS_IN_A_MINUTE = 60;
const int NUMBER_OF_MILLISECONDS_IN_A_SECOND = 1000;

GtkLabel *timer_label = NULL;
GTimer *timer = NULL;

void Timer_initialize(GtkLabel *label)
{
    timer_label = label;
    timer = g_timer_new();
    g_timer_stop(timer);
}

void Timer_start(void)
{
    g_timer_start(timer);
}

void Timer_release(void)
{
    g_timer_destroy(timer);
    timer = NULL;
}

void Timer_redraw(void)
{
    gchar text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    double elapsed_time = (double) g_timer_elapsed(timer, NULL);
    int minutes_digits, seconds_digits, milliseconds_digits;
    minutes_digits = ((int) elapsed_time) / NUMBER_OF_SECONDS_IN_A_MINUTE;
    seconds_digits = ((int) elapsed_time) - (minutes_digits * NUMBER_OF_SECONDS_IN_A_MINUTE);
    milliseconds_digits = (int)(elapsed_time * NUMBER_OF_MILLISECONDS_IN_A_SECOND) -
                          (int) elapsed_time * NUMBER_OF_MILLISECONDS_IN_A_SECOND;
    sprintf(text_buffer, "%02d:%02d:%03d", minutes_digits, seconds_digits, milliseconds_digits);
    gtk_label_set_label(timer_label, text_buffer);
}
