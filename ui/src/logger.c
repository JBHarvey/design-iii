#include "logger.h"

/* Constants */

const int DEFAULT_TEXT_BUFFER_MAX_LENGTH = 100;

/* Global variables */

GtkWidget *logger_widget = NULL;
GtkTextBuffer *logger_text_buffer = NULL;

void Logger_initialize(GtkWidget *widget)
{
    logger_widget = widget;
    logger_text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(logger_widget));
}

void Logger_append(const char *text)
{
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(logger_widget), gtk_text_buffer_get_mark(logger_text_buffer, "insert"),
                                 0.0, TRUE, 0.0, 0.0);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text, -1);
}

void Logger_startMessageSectionAndAppend(const char *text)
{
    Logger_append("\n\n|-|-| MESSAGE |-|-|\n\n");
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(logger_widget), gtk_text_buffer_get_mark(logger_text_buffer, "insert"),
                                 0.0, TRUE, 0.0, 0.0);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text, -1);
}

void Logger_startRobotConnectionHandlerSectionAndAppend(const char *text)
{
    Logger_append("\n\n|-|-| ROBOT CONNECTION HANDLER |-|-|\n\n");
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(logger_widget), gtk_text_buffer_get_mark(logger_text_buffer, "insert"),
                                 0.0, TRUE, 0.0, 0.0);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text, -1);
}

void Logger_appendDouble(double value)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "%f", value);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(logger_widget), gtk_text_buffer_get_mark(logger_text_buffer, "insert"),
                                 0.0, TRUE, 0.0, 0.0);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text_buffer, -1);
}

void Logger_appendPoint2D(struct Point2D point)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "(%f, %f)", point.x, point.y);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(logger_widget), gtk_text_buffer_get_mark(logger_text_buffer, "insert"),
                                 0.0, TRUE, 0.0, 0.0);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text_buffer, -1);
}

void Logger_appendPoint3D(struct Point3D point)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "(%f, %f, %f)", point.x, point.y, point.z);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(logger_widget), gtk_text_buffer_get_mark(logger_text_buffer, "insert"),
                                 0.0, TRUE, 0.0, 0.0);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text_buffer, -1);
}

void Logger_appendPlaneEquation(struct Camera *input_camera)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "(%f)x + (%f)y + (%f)z + (%f) = 0", input_camera->camera_extrinsics->a,
            input_camera->camera_extrinsics->b,
            input_camera->camera_extrinsics->c, input_camera->camera_extrinsics->d);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(logger_widget), gtk_text_buffer_get_mark(logger_text_buffer, "insert"),
                                 0.0, TRUE, 0.0, 0.0);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text_buffer, -1);
}