#include "world_vision_calibration.h"
#include <errno.h>
#include "opencv2/calib3d/calib3d_c.h"

/* Flag definitions */

enum camera_calibration_process_mode {NONE, GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION};

/* Constants */

const int NUMBER_OF_CORNERS_OF_GREEN_SQUARE = 4;
const int NUMBER_OF_ROW_OF_TRANSLATION_VECTOR = 3;
const int NUMBER_OF_COLUMN_OF_TRANSLATION_VECTOR = 1;
const int NUMBER_OF_ROW_OF_ROTATION_VECTOR = 3;
const int NUMBER_OF_COLUMN_OF_ROTATION_VECTOR = 1;
const int 2_DIMENSIONS = 2;
const int 3_DIMENSIONS = 3;
const int DEFAULT_TEXT_BUFFER_MAX_LENGTH = 100;
const int WIDTH_OF_THE_GREEN_SQUARE_IN_MM = 660;

/* Global variables */

extern GtkWidget *logger;
enum camera_calibration_process_mode world_camera_calibration_process_mode = NONE;
CvPoint2D64f image_user_points_defining_the_green_square[NUMBER_OF_CORNERS_OF_GREEN_SQUARE];
CvPoint3D64f world_model_points_defining_the_green_square[NUMBER_OF_CORNERS_OF_GREEN_SQUARE];

/* Event callbacks */

gboolean world_camera_button_press_event_callback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    if(world_camera_calibration_process_mode == GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION) {
        int number_of_user_inputs_gathered = get_amount_of_user_inputs_gathered();

        if(number_of_user_inputs_gathered < NUMBER_OF_CORNERS_OF_GREEN_SQUARE) {
            image_user_points_defining_the_green_square[number_of_user_inputs_gathered] = cvPoint2D64f(((GdkEventButton*)event)->x,
                    ((GdkEventButton*)event)->y);
            gather_user_points_for_camera_pose_computation(number_of_user_inputs_gathered + 1);
        } else {
            return FALSE;
        }
    }

    return TRUE;
}

static gboolean set_camera_matrix_and_distortion_coefficients_from_file(struct camera_intrinsics
        *output_camera_intrinsics, const char *filename)
{
    gboolean is_calibration_file_valid = TRUE;
    CvFileStorage *file_storage = cvOpenFileStorage(filename, NULL, CV_STORAGE_READ, NULL);
    output_camera_intrinsics->camera_matrix = (CvMat*) cvReadByName(file_storage, NULL, "Camera_Matrix", NULL);
    output_camera_intrinsics->distortion_coefficients = (CvMat*) cvReadByName(file_storage, NULL, "Distortion_Coefficients",
            NULL);

    if(output_camera_intrinsics->camera_matrix == NULL || output_camera_intrinsics->distortion_coefficients == NULL) {
        is_calibration_file_valid = FALSE;
    }

    cvReleaseFileStorage(&file_storage);

    return is_calibration_file_valid;
}

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

static int count_the_amount_of_user_points_gathered(void)
{
    int number_of_user_points_gathered = 0;

    for(; number_of_user_points_gathered < NUMBER_OF_CORNERS_OF_GREEN_SQUARE; number_of_user_points_gathered++) {
        if(image_user_points_defining_the_green_square[number_of_user_points_gathered].x == -1
           && image_user_points_defining_the_green_square[number_of_user_points_gathered].y == -1) {
            break;
        } else if(number_of_inputs_gathered == NUMBER_OF_CORNERS_OF_GREEN_SQUARE - 1) {
            number_of_user_points_gathered = NUMBER_OF_CORNERS_OF_GREEN_SQUARE;
            break;
        }
    }

    return number_of_user_points_gathered;
}

static void flatten_array_of_2d_cvpoints(const CvPoint2D64f cvpoint_array[], double output_flattened_array[],
        int number_of_points)
{
    for(int i = 0; i < number_of_points; i++) {
        output_flattened_array[2_DIMENSIONS * i] = cvpoint_array[i].x;
        output_flattened_array[2_DIMENSIONS * i + 1] = cvpoint_array[i].y;
    }
}

static void flatten_array_of_3d_cvpoints(const CvPoint3D64f cvpoint_array[], double output_flattened_array[],
        int number_of_points)
{
    for(int i = 0; i < number_of_points; i++) {
        output_flattened_array[3_DIMENSIONS * i] = cvpoint_array[i].x;
        output_flattened_array[3_DIMENSIONS * i + 1] = cvpoint_array[i].y;
        output_flattened_array[3_DIMENSIONS * i + 2] = cvpoint_array[i].z;
    }
}

static void initialize_camera_extrinsics(struct camera *input_camera)
{
    input_camera->camera_extrinsics->translation_vector = cvCreateMat(NUMBER_OF_ROW_OF_TRANSLATION_VECTOR,
            NUMBER_OF_COLUMN_OF_TRANSLATION_VECTOR, CV_64F);
    input_camera->camera_extrinsics->rotation_vector = cvCreateMat(NUMBER_OF_ROW_OF_ROTATION_VECTOR,
            NUMBER_OF_COLUMN_OF_ROTATION_VECTOR, CV_64F);
}


gboolean compute_camera_pose_from_user_points(struct camera *input_camera)
{
    int enough_user_points_gathered = count_the_amount_of_user_points_gathered() < NUMBER_OF_CORNERS_OF_GREEN_SQUARE;

    if(enough_user_points_gathered) {
        g_idle_add((GSourceFunc) compute_camera_pose_from_user_points, (gpointer) input_camera);
        return FALSE;
    } else {
        double 3d_world_points_array[NUMBER_OF_CORNERS_OF_GREEN_SQUARE * 3_DIMENSIONS];
        double 2d_image_points_array[NUMBER_OF_CORNERS_OF_GREEN_SQUARE * 2_DIMENSIONS];
        flatten_array_of_3d_cvpoints(world_model_points_defining_the_green_square, 3d_world_points_array,
                                     NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
        flatten_array_of_2d_cvpoints(image_user_points_defining_the_green_square, 2d_image_points_array,
                                     NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
        CvMat 3d_world_points, 2d_image_points;
        CvMat * 3d_world_points_ptr = cvInitMatHeader( & 3d_world_points, NUMBER_OF_CORNERS_OF_GREEN_SQUARE, 3_DIMENSIONS,
                                      CV_64FC1,
                                      3d_world_points_array, CV_AUTOSTEP);
        CvMat * 2d_image_points_ptr = cvInitMatHeader( & 2d_image_points, NUMBER_OF_CORNERS_OF_GREEN_SQUARE, 2_DIMENSIONS,
                                      CV_64FC1,
                                      2d_image_points_array, CV_AUTOSTEP);

        initialize_camera_extrinsics(input_camera);
        cvFindExtrinsicCameraParams2(3d_world_points_ptr, 2d_image_points_ptr, input_camera->camera_intrinsics->camera_matrix,
                                     input_camera->camera_intrinsics->distortion_coefficients, input_camera->camera_extrinsics->rotation_vector,
                                     input_camera->camera_extrinsics->translation_vector, 0);
        return FALSE; // Even if it succeeds, return FALSE in order to remove this function from the g_idle state.
    }
}

gboolean gather_user_points_for_camera_pose_computation(int input_index)
{
    char text_buffer[DEFAULT_TEXT_BUFFER_MAX_LENGTH];
    GtkTextBuffer *logger_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(logger));

    switch(input_index) {
        case 0:
            for(int i = 0; i < NUMBER_OF_CORNERS_OF_GREEN_SQUARE; i++) {
                image_user_points_defining_the_green_square[i] = cvPoint2D64f(-1, -1);
            }

            world_model_points_defining_the_green_square[0] = cvPoint3D64f(0, 0, 0);
            world_model_points_defining_the_green_square[1] = cvPoint3D64f(0, WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0);
            world_model_points_defining_the_green_square[2] = cvPoint3D64f(WIDTH_OF_THE_GREEN_SQUARE_IN_MM,
                    WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0);
            world_model_points_defining_the_green_square[3] = cvPoint3D64f(WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0, 0);

            gtk_text_buffer_insert_at_cursor(logger_buffer,
                                             "Mouse click on each of the green square corners in order to map them to these 3D world coordinates. \n\0", -1);
            sprintf(text_buffer, "(%f, %f, %f) ->", world_model_points_defining_the_green_square[0].x,
                    world_model_points_defining_the_green_square[0].y,
                    world_model_points_defining_the_green_square[0].z);
            gtk_text_buffer_insert_at_cursor(logger_buffer, text_buffer, -1);
            world_camera_calibration_process_mode = GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION;
            break;

        case NUMBER_OF_CORNERS_OF_GREEN_SQUARE:
            sprintf(text_buffer, " (%f, %f), \n", image_user_points_defining_the_green_square[input_index - 1].x,
                    image_user_points_defining_the_green_square[input_index - 1].y);
            gtk_text_buffer_insert_at_cursor(logger_buffer, text_buffer, -1);
            gtk_text_buffer_insert_at_cursor(logger_buffer, "Completed ! \n\0", -1);
            world_camera_calibration_process_mode = NONE;
            break;

        default:
            sprintf(text_buffer, " (%f, %f), \n", image_user_points_defining_the_green_square[input_index - 1].x,
                    image_user_points_defining_the_green_square[input_index - 1].y);
            gtk_text_buffer_insert_at_cursor(logger_buffer, text_buffer, -1);
            sprintf(text_buffer, "(%f, %f, %f) ->", world_model_points_defining_the_green_square[input_index].x,
                    world_model_points_defining_the_green_square[input_index].y,
                    world_model_points_defining_the_green_square[input_index].z);
            gtk_text_buffer_insert_at_cursor(logger_buffer, text_buffer, -1);
            break;
    }

    return TRUE;
}

