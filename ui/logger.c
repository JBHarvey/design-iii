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

void Logger_appendCvPoint2D64f(CvPoint2D64f point)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "(%f, %f)", point.x, point.y);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text_buffer, -1);
}

void Logger_appendCvPoint3D64f(CvPoint3D64f point)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    sprintf(text_buffer, "(%f, %f, %f)", point.x, point.y, point.z);
    gtk_text_buffer_insert_at_cursor(logger_text_buffer, text_buffer, -1);
}
