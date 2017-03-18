#include <stdio.h>
#include "logger.h"

/* Flags */

enum WidgetStatus {UNREALIZED, REALIZED};

/* Constants */

const int DEFAULT_TEXT_BUFFER_MAX_LENGTH = 100;
const int DEFAULT_LOGGER_TEXT_BUFFER_MAX_LENGTH = 10000;

/* Global variables */

GtkWidget *logger_widget = NULL;
GtkTextBuffer *logger_text_buffer = NULL;
char logger_char_text_buffer[DEFAULT_LOGGER_TEXT_BUFFER_MAX_LENGTH];
FILE *log_file = NULL;
enum WidgetStatus logger_widget_status = UNREALIZED;
GMutex logger_mutex;


gboolean loggerMappedEventCallback(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    logger_widget_status = REALIZED;

    return FALSE;
}

void Logger_initialize(GtkWidget *widget)
{
    logger_widget = widget;
    logger_text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(logger_widget));
    strcpy(logger_char_text_buffer, "\n");
    gtk_text_buffer_set_text(logger_text_buffer, logger_char_text_buffer, -1);
    log_file = fopen("station_log.txt", "a+");
    fprintf(log_file, "\n\n\t\t------------ NEW EXECUTION ------------\n");
}

void Logger_finalize(void)
{
    fprintf(log_file, "%s", logger_char_text_buffer);
    fclose(log_file);
}

void Logger_append(const char *text)
{
    g_mutex_lock(&logger_mutex);

    if(logger_text_buffer != NULL) {

        if(strlen(logger_char_text_buffer) + strlen(text) >= DEFAULT_LOGGER_TEXT_BUFFER_MAX_LENGTH) {
            fprintf(log_file, "%s", logger_char_text_buffer);
            strcpy(logger_char_text_buffer, "");
        }

        strcat(logger_char_text_buffer, text);

        if(logger_widget_status == REALIZED) {
            gtk_text_buffer_set_text(logger_text_buffer, logger_char_text_buffer, -1);
        }
    }

    g_mutex_unlock(&logger_mutex);
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
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "%f", value);
    Logger_append(text_buffer);
}

void Logger_appendPoint2D(struct Point2D point)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "(%f, %f)", point.x, point.y);
    Logger_append(text_buffer);
}

void Logger_appendPoint3D(struct Point3D point)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "(%f, %f, %f)", point.x, point.y, point.z);
    Logger_append(text_buffer);
}

void Logger_appendPlaneEquation(struct Camera *input_camera)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "(%f)x + (%f)y + (%f)z + (%f) = 0", input_camera->camera_extrinsics->a,
            input_camera->camera_extrinsics->b,
            input_camera->camera_extrinsics->c, input_camera->camera_extrinsics->d);
    Logger_append(text_buffer);
}
