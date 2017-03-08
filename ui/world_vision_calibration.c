#include "world_vision_calibration.h"
#include <errno.h>
#include "opencv2/calib3d/calib3d_c.h"

/* Flag definitions */

enum camera_calibration_process_mode {NONE, GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION};

/* Constants */

const int GREEN_SQUARE_NUMBER_OF_CORNERS = 4;

/* Global variables */

extern GtkWidget *logger;
enum camera_calibration_process_mode world_camera_calibration_process_mode = NONE;
CvPoint2D64f green_square_image_user_points[GREEN_SQUARE_NUMBER_OF_CORNERS];
CvPoint3D64f green_square_world_model_points[GREEN_SQUARE_NUMBER_OF_CORNERS];

/* Private functions prototypes */

static void prompt_invalid_calibration_file_error(GtkWidget *widget, const char *filename);
static gboolean set_camera_matrix_and_distortion_coefficients_from_file(struct camera_intrinsics
        *output_camera_intrinsics, const char *filename);
static int get_amount_of_user_inputs_gathered(void);
static void flatten_array_of_3d_cvpoints(const CvPoint3D64f cvpoint_array[], double output_flattened_array[],
        int number_of_points);
static void flatten_array_of_2d_cvpoints(const CvPoint2D64f cvpoint_array[], double output_flattened_array[],
        int number_of_points);

/* Event callbacks */

gboolean world_camera_button_press_event_callback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    if(world_camera_calibration_process_mode == GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION) {
        int number_of_user_inputs_gathered = get_amount_of_user_inputs_gathered();

        if(number_of_user_inputs_gathered < GREEN_SQUARE_NUMBER_OF_CORNERS) {
            green_square_image_user_points[number_of_user_inputs_gathered] = cvPoint2D64f(((GdkEventButton*)event)->x,
                    ((GdkEventButton*)event)->y);
            gather_user_inputs_for_camera_pose_computation(number_of_user_inputs_gathered + 1);
        } else {
            return FALSE;
        }
    }

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

gboolean compute_camera_pose_from_user_inputs(struct camera* input_camera)
{
    if(get_amount_of_user_inputs_gathered() < GREEN_SQUARE_NUMBER_OF_CORNERS) {
        g_idle_add((GSourceFunc) compute_camera_pose_from_user_inputs, (gpointer) input_camera);
        return FALSE;
    } else {
        input_camera->camera_extrinsics->translation_vector = cvCreateMat(3, 1, CV_64F);
        input_camera->camera_extrinsics->rotation_vector = cvCreateMat(3, 1, CV_64F);
        double object_points_array[GREEN_SQUARE_NUMBER_OF_CORNERS * 3];
        double image_points_array[GREEN_SQUARE_NUMBER_OF_CORNERS * 2];
        flatten_array_of_3d_cvpoints(green_square_world_model_points, object_points_array, GREEN_SQUARE_NUMBER_OF_CORNERS);
        flatten_array_of_2d_cvpoints(green_square_image_user_points, image_points_array, GREEN_SQUARE_NUMBER_OF_CORNERS);
        CvMat object_points, image_points;
        CvMat *object_points_ptr = cvInitMatHeader(&object_points, GREEN_SQUARE_NUMBER_OF_CORNERS, 3, CV_64FC1,
                                   object_points_array, CV_AUTOSTEP);
        CvMat *image_points_ptr = cvInitMatHeader(&image_points, GREEN_SQUARE_NUMBER_OF_CORNERS, 2, CV_64FC1,
                                  image_points_array, CV_AUTOSTEP);
        cvFindExtrinsicCameraParams2(object_points_ptr, image_points_ptr, input_camera->camera_intrinsics->camera_matrix,
                                     input_camera->camera_intrinsics->distortion_coefficients, input_camera->camera_extrinsics->rotation_vector,
                                     input_camera->camera_extrinsics->translation_vector, 0);
        return FALSE;
    }
}

gboolean gather_user_inputs_for_camera_pose_computation(int input_index)
{
    char text_buffer[100];
    GtkTextBuffer *logger_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(logger));

    switch(input_index) {
        case 0:
            for(int i = 0; i < GREEN_SQUARE_NUMBER_OF_CORNERS; i++) {
                green_square_image_user_points[i] = cvPoint2D64f(-1, -1);
            }

            green_square_world_model_points[0] = cvPoint3D64f(0, 0, 0);
            green_square_world_model_points[1] = cvPoint3D64f(0, 660, 0);
            green_square_world_model_points[2] = cvPoint3D64f(660, 660, 0);
            green_square_world_model_points[3] = cvPoint3D64f(660, 0, 0);

            gtk_text_buffer_insert_at_cursor(logger_buffer,
                                             "Mouse click on each of the green square corners in order to map them to these 3D world coordinates. \n\0", -1);
            sprintf(text_buffer, "(%f, %f, %f) ->", green_square_world_model_points[0].x, green_square_world_model_points[0].y,
                    green_square_world_model_points[0].z);
            gtk_text_buffer_insert_at_cursor(logger_buffer, text_buffer, -1);
            world_camera_calibration_process_mode = GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION;
            break;

        case GREEN_SQUARE_NUMBER_OF_CORNERS:
            sprintf(text_buffer, " (%f, %f), \n", green_square_image_user_points[input_index - 1].x,
                    green_square_image_user_points[input_index - 1].y);
            gtk_text_buffer_insert_at_cursor(logger_buffer, text_buffer, -1);
            gtk_text_buffer_insert_at_cursor(logger_buffer, "Completed ! \n\0", -1);
            world_camera_calibration_process_mode = NONE;
            break;

        default:
            sprintf(text_buffer, " (%f, %f), \n", green_square_image_user_points[input_index - 1].x,
                    green_square_image_user_points[input_index - 1].y);
            gtk_text_buffer_insert_at_cursor(logger_buffer, text_buffer, -1);
            sprintf(text_buffer, "(%f, %f, %f) ->", green_square_world_model_points[input_index].x,
                    green_square_world_model_points[input_index].y,
                    green_square_world_model_points[input_index].z);
            gtk_text_buffer_insert_at_cursor(logger_buffer, text_buffer, -1);
            break;

    }

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

static int get_amount_of_user_inputs_gathered(void)
{
    int number_of_inputs_gathered = 0;

    for(; number_of_inputs_gathered < GREEN_SQUARE_NUMBER_OF_CORNERS; number_of_inputs_gathered++) {
        if(green_square_image_user_points[number_of_inputs_gathered].x == -1
           && green_square_image_user_points[number_of_inputs_gathered].y == -1) {
            break;
        } else if(number_of_inputs_gathered == GREEN_SQUARE_NUMBER_OF_CORNERS - 1) {
            number_of_inputs_gathered = GREEN_SQUARE_NUMBER_OF_CORNERS;
            break;
        }
    }

    return number_of_inputs_gathered;
}

static void flatten_array_of_2d_cvpoints(const CvPoint2D64f cvpoint_array[], double output_flattened_array[],
        int number_of_points)
{
    for(int i = 0; i < number_of_points; i++) {
        output_flattened_array[2 * i] = cvpoint_array[i].x;
        output_flattened_array[2 * i + 1] = cvpoint_array[i].y;
    }
}

static void flatten_array_of_3d_cvpoints(const CvPoint3D64f cvpoint_array[], double output_flattened_array[],
        int number_of_points)
{
    for(int i = 0; i < number_of_points; i++) {
        output_flattened_array[3 * i] = cvpoint_array[i].x;
        output_flattened_array[3 * i + 1] = cvpoint_array[i].y;
        output_flattened_array[3 * i + 2] = cvpoint_array[i].z;
    }
}
