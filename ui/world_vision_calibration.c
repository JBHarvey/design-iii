#include "world_vision_calibration.h"
#include "logger.h"
#include <errno.h>
#include "opencv2/calib3d/calib3d_c.h"

/* Flag definitions */

enum CameraCalibrationProcessMode {NONE, GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION};

/* Constants */

const int NUMBER_OF_CORNERS_OF_GREEN_SQUARE = 4;
const int NUMBER_OF_ROW_OF_TRANSLATION_VECTOR = 3;
const int NUMBER_OF_COLUMN_OF_TRANSLATION_VECTOR = 1;
const int NUMBER_OF_ROW_OF_ROTATION_VECTOR = 3;
const int NUMBER_OF_COLUMN_OF_ROTATION_VECTOR = 1;
const int TWO_DIMENSIONS = 2;
const int TRHEE_DIMENSIONS = 3;
const int WIDTH_OF_THE_GREEN_SQUARE_IN_MM = 660;

/* Global variables */

enum CameraCalibrationProcessMode world_camera_calibration_process_mode = NONE;
CvPoint2D64f image_user_points_defining_the_green_square[NUMBER_OF_CORNERS_OF_GREEN_SQUARE];
CvPoint3D64f world_model_points_defining_the_green_square[NUMBER_OF_CORNERS_OF_GREEN_SQUARE];

static int countTheAmountOfUserPointsGathered(void)
{
    int number_of_user_points_gathered = 0;

    for(; number_of_user_points_gathered < NUMBER_OF_CORNERS_OF_GREEN_SQUARE; number_of_user_points_gathered++) {
        if(image_user_points_defining_the_green_square[number_of_user_points_gathered].x == -1
           && image_user_points_defining_the_green_square[number_of_user_points_gathered].y == -1) {
            break;
        } else if(number_of_user_points_gathered == NUMBER_OF_CORNERS_OF_GREEN_SQUARE - 1) {
            number_of_user_points_gathered = NUMBER_OF_CORNERS_OF_GREEN_SQUARE;
            break;
        }
    }

    return number_of_user_points_gathered;
}

gboolean worldCameraButtonPressEventCallback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    if(world_camera_calibration_process_mode == GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION) {
        int number_of_user_points_gathered = countTheAmountOfUserPointsGathered();

        if(number_of_user_points_gathered < NUMBER_OF_CORNERS_OF_GREEN_SQUARE) {
            image_user_points_defining_the_green_square[number_of_user_points_gathered] = cvPoint2D64f(((GdkEventButton*)event)->x,
                    ((GdkEventButton*)event)->y);
            WorldVisionCalibration_gatherUserPointsForCameraPoseComputation(number_of_user_points_gathered + 1);
        } else {
            return FALSE;
        }
    }

    return TRUE;
}

static gboolean setCameraMatrixAndDistortionCoefficientsFromFile(struct CameraIntrinsics
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

static void promptInvalidCalibrationFileError(GtkWidget *widget, const char *filename)
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

gboolean WorldVisionCalibration_initializeCameraMatrixAndDistortionCoefficientsFromFile(GtkWidget *widget,
        struct CameraIntrinsics *output_camera_intrinsics)
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
            isInitialized = setCameraMatrixAndDistortionCoefficientsFromFile(output_camera_intrinsics, filename);

            if(!isInitialized) {
                promptInvalidCalibrationFileError(widget, filename);
            }

            g_free(filename);
            gtk_widget_destroy(dialog);
        } else {
            gtk_widget_destroy(dialog);
            return FALSE;
        }
    } while(!isInitialized);

    WorldVision_setWorldCameraStatusCalibrated();

    return TRUE;
}

static void flattenArrayOfTwoDimensionsCvPoints(const CvPoint2D64f cvpoint_array[],
        double output_flattened_array[],
        int number_of_points)
{
    for(int i = 0; i < number_of_points; i++) {
        output_flattened_array[TWO_DIMENSIONS * i] = cvpoint_array[i].x;
        output_flattened_array[TWO_DIMENSIONS * i + 1] = cvpoint_array[i].y;
    }
}

static void flattenArrayOfThreeDimensionsCvPoints(const CvPoint3D64f cvpoint_array[],
        double output_flattened_array[],
        int number_of_points)
{
    for(int i = 0; i < number_of_points; i++) {
        output_flattened_array[TRHEE_DIMENSIONS * i] = cvpoint_array[i].x;
        output_flattened_array[TRHEE_DIMENSIONS * i + 1] = cvpoint_array[i].y;
        output_flattened_array[TRHEE_DIMENSIONS * i + 2] = cvpoint_array[i].z;
    }
}

static void initializeCameraExtrinsics(struct Camera *input_camera)
{
    input_camera->camera_extrinsics->translation_vector = cvCreateMat(NUMBER_OF_ROW_OF_TRANSLATION_VECTOR,
            NUMBER_OF_COLUMN_OF_TRANSLATION_VECTOR, CV_64F);
    input_camera->camera_extrinsics->rotation_vector = cvCreateMat(NUMBER_OF_ROW_OF_ROTATION_VECTOR,
            NUMBER_OF_COLUMN_OF_ROTATION_VECTOR, CV_64F);
}


gboolean WorldVisionCalibration_computeCameraPoseFromUserPoints(struct Camera *input_camera)
{
    int enough_user_points_gathered = countTheAmountOfUserPointsGathered() < NUMBER_OF_CORNERS_OF_GREEN_SQUARE;

    if(enough_user_points_gathered) {
        g_idle_add((GSourceFunc) WorldVisionCalibration_computeCameraPoseFromUserPoints, (gpointer) input_camera);
        return FALSE;
    } else {
        double three_dimensions_world_points_array[NUMBER_OF_CORNERS_OF_GREEN_SQUARE * TRHEE_DIMENSIONS];
        double two_dimensions_image_points_array[NUMBER_OF_CORNERS_OF_GREEN_SQUARE * TWO_DIMENSIONS];
        flattenArrayOfThreeDimensionsCvPoints(world_model_points_defining_the_green_square,
                                              three_dimensions_world_points_array,
                                              NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
        flattenArrayOfTwoDimensionsCvPoints(image_user_points_defining_the_green_square, two_dimensions_image_points_array,
                                            NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
        CvMat three_dimensions_world_points, two_dimensions_image_points;
        CvMat * three_dimensions_world_points_ptr = cvInitMatHeader(&three_dimensions_world_points,
                NUMBER_OF_CORNERS_OF_GREEN_SQUARE, TRHEE_DIMENSIONS,
                CV_64FC1,
                three_dimensions_world_points_array, CV_AUTOSTEP);
        CvMat * two_dimensions_image_points_ptr = cvInitMatHeader(&two_dimensions_image_points,
                NUMBER_OF_CORNERS_OF_GREEN_SQUARE, TWO_DIMENSIONS,
                CV_64FC1,
                two_dimensions_image_points_array, CV_AUTOSTEP);

        initializeCameraExtrinsics(input_camera);
        cvFindExtrinsicCameraParams2(three_dimensions_world_points_ptr, two_dimensions_image_points_ptr,
                                     input_camera->camera_intrinsics->camera_matrix,
                                     input_camera->camera_intrinsics->distortion_coefficients, input_camera->camera_extrinsics->rotation_vector,
                                     input_camera->camera_extrinsics->translation_vector, 0);
        return FALSE; // Even if it succeeds, return FALSE in order to remove this function from the g_idle state.
    }
}

static void initializePointsArraysForCameraPoseComputation(void)
{
    for(int i = 0; i < NUMBER_OF_CORNERS_OF_GREEN_SQUARE; i++) {
        image_user_points_defining_the_green_square[i] = cvPoint2D64f(-1, -1);
    }

    world_model_points_defining_the_green_square[0] = cvPoint3D64f(0, 0, 0);
    world_model_points_defining_the_green_square[1] = cvPoint3D64f(0, WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0);
    world_model_points_defining_the_green_square[2] = cvPoint3D64f(WIDTH_OF_THE_GREEN_SQUARE_IN_MM,
            WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0);
    world_model_points_defining_the_green_square[3] = cvPoint3D64f(WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0, 0);
}

gboolean WorldVisionCalibration_gatherUserPointsForCameraPoseComputation(int input_index)
{
    switch(input_index) {
        case 0:
            initializePointsArraysForCameraPoseComputation();
            Logger_startMessageSectionAndAppend("Mouse click on each of the green square corners in order to map them to these 3D world coordinates. \n");
            Logger_appendCvPoint3D64f(world_model_points_defining_the_green_square[0]);
            Logger_append(" -> ");
            world_camera_calibration_process_mode = GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION;
            break;

        case NUMBER_OF_CORNERS_OF_GREEN_SQUARE:
            Logger_appendCvPoint2D64f(image_user_points_defining_the_green_square[input_index - 1]);
            Logger_append(".\n Completed !");
            world_camera_calibration_process_mode = NONE;
            break;

        default:
            Logger_appendCvPoint2D64f(image_user_points_defining_the_green_square[input_index - 1]);
            Logger_append(", \n");
            Logger_appendCvPoint3D64f(world_model_points_defining_the_green_square[input_index]);
            Logger_append(" -> ");
            break;
    }

    return TRUE;
}

