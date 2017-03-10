#include "world_vision_calibration.h"
#include "logger.h"
#include "point_types.h"
#include <errno.h>
#include <math.h>
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
struct Point2DSet *image_point_set_defining_the_green_square = NULL;
struct Point3DSet *object_point_set_defining_the_green_square = NULL;

static void initializePointsArraysForCameraPoseComputation(void)
{
    PointTypes_initializePoint2DSet(image_point_set_defining_the_green_square, NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
    PointTypes_initializePoint3DSet(object_point_set_defining_the_green_square, NUMBER_OF_CORNERS_OF_GREEN_SQUARE);

    PointTypes_addPointToPoint3DSet(object_point_set_defining_the_green_square, PointTypes_createPoint3D(0, 0, 0));
    PointTypes_addPointToPoint3DSet(object_point_set_defining_the_green_square, PointTypes_createPoint3D(0,
                                    WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0));
    PointTypes_addPointToPoint3DSet(object_point_set_defining_the_green_square,
                                    PointTypes_createPoint3D(WIDTH_OF_THE_GREEN_SQUARE_IN_MM, WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0));
    PointTypes_addPointToPoint3DSet(object_point_set_defining_the_green_square,
                                    PointTypes_createPoint3D(WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0, 0));
}

static gboolean gatherUserPointsForCameraPoseComputation(int point_index)
{
    switch(point_index) {
        case 0:
            initializePointsArraysForCameraPoseComputation();
            Logger_startMessageSectionAndAppend("Mouse click on each of the green square corners in order to map them to these 3D world coordinates. \n");
            Logger_appendPoint3D(PointTypes_getPointFromPoint3DSet(object_point_set_defining_the_green_square, 0));
            Logger_append(" -> ");
            world_camera_calibration_process_mode = GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION;
            break;

        case NUMBER_OF_CORNERS_OF_GREEN_SQUARE:
            Logger_appendPoint2D(PointTypes_getPointFromPoint2DSet(image_point_set_defining_the_green_square, point_index - 1));
            Logger_append(".");
            Logger_startMessageSectionAndAppend("Camera pose computation completed.");
            world_camera_calibration_process_mode = NONE;
            break;

        default:
            Logger_appendPoint2D(PointTypes_getPointFromPoint2DSet(image_point_set_defining_the_green_square, point_index - 1));
            Logger_append(", \n");
            Logger_appendPoint3D(PointTypes_getPointFromPoint3DSet(object_point_set_defining_the_green_square, point_index));
            Logger_append(" -> ");
            break;
    }

    return TRUE;
}

gboolean worldCameraButtonPressEventCallback(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    if(world_camera_calibration_process_mode == GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION) {
        int number_of_user_points_gathered = PointTypes_getNumberOfPointStoredInPoint2DSet(
                image_point_set_defining_the_green_square);
        int not_enough_user_points_gathered = number_of_user_points_gathered < NUMBER_OF_CORNERS_OF_GREEN_SQUARE;

        if(not_enough_user_points_gathered) {
            PointTypes_addPointToPoint2DSet(image_point_set_defining_the_green_square,
                                            PointTypes_createPoint2D(((GdkEventButton*)event)->x * WORLD_CAMERA_WIDTH / (double) gtk_widget_get_allocated_width(
                                                    widget),
                                                    ((GdkEventButton*)event)->y * WORLD_CAMERA_HEIGHT / (double) gtk_widget_get_allocated_height(widget)));
            gatherUserPointsForCameraPoseComputation(number_of_user_points_gathered + 1);
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
        struct Camera *output_camera)
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
            isInitialized = setCameraMatrixAndDistortionCoefficientsFromFile(output_camera->camera_intrinsics, filename);

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

    WorldVision_setWorldCameraCaptureModeUndistorted();
    output_camera->camera_status = INTRINSICALLY_CALIBRATED;
    Logger_startMessageSectionAndAppend("Camera intrinsic parameters loaded from file.");

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

static double checkReprojectionErrorOnCameraPose(CvMat *three_dimensions_world_points,
        CvMat *two_dimensions_image_points, struct Camera *input_camera)
{
    CvMat projected_points;
    double initialization_array[NUMBER_OF_CORNERS_OF_GREEN_SQUARE * 2] = {0};
    CvMat *projected_points_ptr = cvInitMatHeader(&projected_points, NUMBER_OF_CORNERS_OF_GREEN_SQUARE, TWO_DIMENSIONS,
                                  CV_64FC1, initialization_array, CV_AUTOSTEP);
    cvProjectPoints2(three_dimensions_world_points, input_camera->camera_extrinsics->rotation_vector,
                     input_camera->camera_extrinsics->translation_vector, input_camera->camera_intrinsics->camera_matrix,
                     input_camera->camera_intrinsics->distortion_coefficients, projected_points_ptr, NULL, NULL, NULL, NULL, NULL, 0);
    double reprojection_error = 0.0;

    for(int i = 0; i < NUMBER_OF_CORNERS_OF_GREEN_SQUARE; i++) {
        reprojection_error += pow(CV_MAT_ELEM(projected_points, double, i, 0) -
                                  CV_MAT_ELEM(*two_dimensions_image_points, double, i, 0), 2.0) +
                              pow(CV_MAT_ELEM(projected_points, double, i, 1) -
                                  CV_MAT_ELEM(*two_dimensions_image_points, double, i, 1), 2.0);
    }

    reprojection_error = sqrt(reprojection_error / NUMBER_OF_CORNERS_OF_GREEN_SQUARE);

    return reprojection_error;
}

static void createObjectAndImagePointsMatrix(CvMat *output_world_points_mat, CvMat *output_image_points_mat)
{
    double *three_dimensions_world_points_array = malloc(sizeof(double) * NUMBER_OF_CORNERS_OF_GREEN_SQUARE *
            TRHEE_DIMENSIONS);
    double *two_dimensions_image_points_array = malloc(sizeof(double) * NUMBER_OF_CORNERS_OF_GREEN_SQUARE *
            TWO_DIMENSIONS);
    flattenArrayOfThreeDimensionsCvPoints(object_point_set_defining_the_green_square,
                                          three_dimensions_world_points_array,
                                          NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
    flattenArrayOfTwoDimensionsCvPoints(image_point_set_defining_the_green_square, two_dimensions_image_points_array,
                                        NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
    cvSetData(output_world_points_mat, three_dimensions_world_points_array, CV_AUTOSTEP);
    cvSetData(output_image_points_mat, two_dimensions_image_points_array, CV_AUTOSTEP);

}

static gboolean computeCameraPoseFromUserPoints(struct Camera *input_camera)
{
    int not_enough_user_points_gathered = countTheAmountOfUserPointsGathered() < NUMBER_OF_CORNERS_OF_GREEN_SQUARE;

    if(not_enough_user_points_gathered) {
        g_idle_add((GSourceFunc) computeCameraPoseFromUserPoints, (gpointer) input_camera);
        return FALSE;
    } else {
        CvMat *three_dimensions_world_points = cvCreateMatHeader(NUMBER_OF_CORNERS_OF_GREEN_SQUARE, TRHEE_DIMENSIONS,
                                               CV_64FC1);
        CvMat *two_dimensions_image_points = cvCreateMatHeader(NUMBER_OF_CORNERS_OF_GREEN_SQUARE, TWO_DIMENSIONS,
                                             CV_64FC1);
        createObjectAndImagePointsMatrix(three_dimensions_world_points, two_dimensions_image_points);
        printf("\n DEBUG: %f", CV_MAT_ELEM(*two_dimensions_image_points, double, 0, 0));
        initializeCameraExtrinsics(input_camera);
        cvFindExtrinsicCameraParams2(three_dimensions_world_points, two_dimensions_image_points,
                                     input_camera->camera_intrinsics->camera_matrix,
                                     input_camera->camera_intrinsics->distortion_coefficients, input_camera->camera_extrinsics->rotation_vector,
                                     input_camera->camera_extrinsics->translation_vector, 0);

        double reprojection_error = checkReprojectionErrorOnCameraPose(three_dimensions_world_points,
                                    two_dimensions_image_points, input_camera);
        Logger_append("\n Reprojection error on camera pose: ");
        Logger_appendDouble(reprojection_error);
        input_camera->camera_status = CALIBRATED;

        uchar *data;
        cvGetRawData((CvArr*) three_dimensions_world_points, &data, NULL, NULL);
        free(&data);
        cvGetRawData((CvArr*) two_dimensions_image_points, &data, NULL, NULL);
        free(&data);

        cvReleaseMat(&three_dimensions_world_points);
        cvReleaseMat(&two_dimensions_image_points);

        return FALSE; // Even if it succeeds, return FALSE in order to remove this function from the g_idle state.
    }
}

static gboolean computePlaneEquation(struct Camera *input_camera)
{
    int not_enough_user_points_gathered = countTheAmountOfUserPointsGathered() < NUMBER_OF_CORNERS_OF_GREEN_SQUARE;

    if(not_enough_user_points_gathered || input_camera->camera_status != CALIBRATED) {
        g_idle_add((GSourceFunc) computePlaneEquation, (gpointer) input_camera);
        return FALSE;
    } else {
        return FALSE; // Even if it succeeds, return FALSE in order to remove this function from the g_idle state.
    }
}

gboolean WorldVisionCalibration_calibrate(struct Camera * input_camera)
{
    gatherUserPointsForCameraPoseComputation(0);
    computeCameraPoseFromUserPoints(input_camera);
    computePlaneEquation(input_camera);

    return TRUE;
}

