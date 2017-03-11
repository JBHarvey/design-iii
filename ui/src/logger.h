#ifndef __LOGGER
#define __LOGGER

#include <gtk/gtk.h>
#include "opencv2/core/types_c.h"

void Logger_initialize(GtkWidget *widget);

void Logger_append(const char *text);

void Logger_startMessageSectionAndAppend(const char *text);

void Logger_appendDouble(double value);

void Logger_appendCvPoint2D64f(CvPoint2D64f point);

void Logger_appendCvPoint3D64f(CvPoint3D64f point);

#endif // __LOGGER
