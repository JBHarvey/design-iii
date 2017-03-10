#ifndef __LOGGER
#define __LOGGER

#include <gtk/gtk.h>
#include "point_types.h"

void Logger_initialize(GtkWidget *widget);

void Logger_append(const char *text);

void Logger_startMessageSectionAndAppend(const char *text);

void Logger_appendDouble(double value);

void Logger_appendPoint2D(struct Point2D point);

void Logger_appendPoint3D(struct Point3D point);

#endif // __LOGGER
