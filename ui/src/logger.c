#include <stdio.h>
#include "logger.h"

/* Constants */

const int DEFAULT_TEXT_BUFFER_MAX_LENGTH = 100;

/* Global variables */

GtkWidget *logger_widget = NULL;
FILE *log_file = NULL;

void Logger_initialize(GtkWidget *widget)
{
    logger_widget = widget;
    log_file = fopen("station_log.txt", "a+");
    fprintf(log_file, "\n\n\t\t------------ NEW EXECUTION ------------\n");
}

static gboolean append(char *text)
{
    GtkTextBuffer *logger_text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(logger_widget));
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(logger_widget), gtk_text_buffer_get_mark(logger_text_buffer, "insert"), 0.0,
                                 TRUE, 0.0, 0.0);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text, -1);
    fprintf(log_file, "%s", text);

    return FALSE;
}

void Logger_finalize(void)
{
    fprintf(log_file, "\n\n\n");
    fclose(log_file);
}

void Logger_append(const char *text)
{
    g_idle_add((GSourceFunc) append, (gpointer) text);
}

static gboolean _dynamicAppend(char *text)
{
    GtkTextBuffer *logger_text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(logger_widget));
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(logger_widget), gtk_text_buffer_get_mark(logger_text_buffer, "insert"), 0.0,
                                 TRUE, 0.0, 0.0);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text, -1);
    fprintf(log_file, "%s", text);
    free(text);

    return FALSE;
}

static void dynamicAppend(char *text)
{
    g_idle_add((GSourceFunc) _dynamicAppend, (gpointer) text);
}


void Logger_startMessageSectionAndAppend(const char *text)
{
    Logger_append("\n\n|-|-| MESSAGE |-|-|\n\n");
    Logger_append(text);
}

void Logger_startRobotConnectionHandlerSectionAndAppend(const char *text)
{
    Logger_append("\n\n|-|-| ROBOT CONNECTION HANDLER |-|-|\n\n");
    Logger_append(text);
}

void Logger_appendDouble(double value)
{
    char *text_buffer = malloc(sizeof(char) * DEFAULT_TEXT_BUFFER_MAX_LENGTH);
    sprintf(text_buffer, "%f", value);
    dynamicAppend(text_buffer);
}

void Logger_appendPoint2D(struct Point2D point)
{
    char *text_buffer = malloc(sizeof(char) * DEFAULT_TEXT_BUFFER_MAX_LENGTH);
    sprintf(text_buffer, "(%f, %f)", point.x, point.y);
    dynamicAppend(text_buffer);
}

void Logger_appendPoint3D(struct Point3D point)
{
    char *text_buffer = malloc(sizeof(char) * DEFAULT_TEXT_BUFFER_MAX_LENGTH);
    sprintf(text_buffer, "(%f, %f, %f)", point.x, point.y, point.z);
    dynamicAppend(text_buffer);
}

void Logger_appendPlaneEquation(struct Camera *input_camera)
{
    char *text_buffer = malloc(sizeof(char) * DEFAULT_TEXT_BUFFER_MAX_LENGTH);
    sprintf(text_buffer, "(%f)x + (%f)y + (%f)z + (%f) = 0", input_camera->camera_extrinsics->a,
            input_camera->camera_extrinsics->b,
            input_camera->camera_extrinsics->c, input_camera->camera_extrinsics->d);
    dynamicAppend(text_buffer);
}
