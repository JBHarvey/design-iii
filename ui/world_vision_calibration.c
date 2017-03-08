#include "world_vision_calibration.h"
#include <errno.h>

/* Global variables */

extern GtkWidget *logger;
CvPoint3D64f green_square_model_points[4] = {cvPoint3D64f(0, 0, 0), cvPoint3D64f(0, 660, 0), cvPoint3D64f(660, 660, 0), cvPoint3D64f(660, 0, 0)};

/* Private functions prototypes */

static void prompt_invalid_calibration_file_error(GtkWidget *widget, const char *filename);
static gboolean set_camera_matrix_and_distortion_coefficients_from_file(struct camera_intrinsics
        *output_camera_intrinsics, const char *filename);

/* Event callbacks */

gboolean world_camera_button_press_event_callback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    printf("%f, ", ((GdkEventButton*)event)->x);
    return TRUE;
}

/* Public functions */

gboolean initialize_camera_matrix_and_distortion_coefficients_from_file(GtkWidget *widget,
        struct camera_intrinsics *output_camera_intrinsics)
{
    gboolean isInitialized;

    do {
        GtkWidget *dialog = gtk_file_chooser_dialog_new("open calibration xml file", GTK_WINDOW(widget),
                            GTK_FILE_CHOOSER_ACTION_OPEN, "cancel", GTK_RESPONSE_CANCEL, "select",
                            GTK_RESPONSE_ACCEPT, NULL);
        GtkFileFilter *filter = gtk_file_filter_new();

        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
        gtk_file_filter_set_name(filter, "xml data file");
        gtk_file_filter_add_mime_type(filter, "application/xml");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

        if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            isInitialized = set_camera_matrix_and_distortion_coefficients_from_file(output_camera_intrinsics, filename);

            if(!isInitialized) {
                prompt_invalid_calibration_file_error(widget, filename);
            }

            g_free(filename);
            gtk_widget_destroy(dialog);
        } else {
            gtk_widget_destroy(dialog);
            return FALSE;
        }
    } while(!isInitialized);

    set_world_camera_status_calibrated();

    return TRUE;
}

// NOTE: gather_user_inputs_for_camera_pose_computation(..., callback below)
gboolean compute_camera_pose_from_user_inputs(struct camera_intrinsics *input_camera_intrinsics)
{
    char text_buffer[100];

    GtkTextBuffer *logger_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(logger));
    gtk_text_buffer_set_text(logger_buffer,
                             "Mouse click on each of the green square corners in order to map them to these 3D world coordinates. \n\0", -1);
    sprintf(text_buffer, "(%f, %f, %f) ->", green_square_model_points[0].x, green_square_model_points[0].y,
            green_square_model_points[0].z);
    gtk_text_buffer_insert_at_cursor(logger_buffer, text_buffer, -1);

    return TRUE;
}

/* Private functions */

static void prompt_invalid_calibration_file_error(GtkWidget *widget, const char *filename)
{
    GtkWidget *errorDialog = gtk_message_dialog_new(GTK_WINDOW(widget),
                             GTK_DIALOG_DESTROY_WITH_PARENT,
                             GTK_MESSAGE_ERROR,
                             GTK_BUTTONS_CLOSE,
                             "Error reading “%s”: %s.\n Please, select a valid XML calibration file.",
                             filename,
                             g_strerror(EINVAL));
    gtk_dialog_run(GTK_DIALOG(errorDialog));
    gtk_widget_destroy(errorDialog);
}

static gboolean set_camera_matrix_and_distortion_coefficients_from_file(struct camera_intrinsics
        *output_camera_intrinsics, const char *filename)
{
    gboolean isCalibrationFileValid = TRUE;
    CvFileStorage *file_storage = cvOpenFileStorage(filename, NULL, CV_STORAGE_READ, NULL);
    output_camera_intrinsics->camera_matrix = (CvMat*) cvReadByName(file_storage, NULL, "Camera_Matrix", NULL);
    output_camera_intrinsics->distortion_coefficients = (CvMat*) cvReadByName(file_storage, NULL, "Distortion_Coefficients",
            NULL);

    if(output_camera_intrinsics->camera_matrix == NULL || output_camera_intrinsics->distortion_coefficients == NULL) {
        isCalibrationFileValid = FALSE;
    }

    cvReleaseFileStorage(&file_storage);

    return isCalibrationFileValid;
}
