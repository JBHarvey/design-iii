#include <errno.h>
#include <math.h>
#include "opencv2/calib3d/calib3d_c.h"

#include "world_vision_calibration.h"
#include "logger.h"

/* Flag definitions */

enum CameraCalibrationProcessMode {NONE, GET_USER_MOUSE_CLICKS_FOR_CAMERA_POSE_COMPUTATION};

/* Constants */

const int NUMBER_OF_CORNERS_OF_GREEN_SQUARE = 4;
const int TWO_DIMENSIONS = 2;
const int TRHEE_DIMENSIONS = 3;
const int WIDTH_OF_THE_GREEN_SQUARE_IN_MM = 660;

/* Global variables */

enum CameraCalibrationProcessMode world_camera_calibration_process_mode = NONE;
struct Point2DSet *image_point_set_defining_the_green_square;
struct Point3DSet *object_point_set_defining_the_green_square;

static void initializePointsArraysForCameraPoseComputation(void)
{
    image_point_set_defining_the_green_square = PointTypes_initializePoint2DSet(NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
    object_point_set_defining_the_green_square = PointTypes_initializePoint3DSet(NUMBER_OF_CORNERS_OF_GREEN_SQUARE);


    PointTypes_addPointToPoint3DSet(object_point_set_defining_the_green_square, PointTypes_createPoint3D(0, 0, 0));
    PointTypes_addPointToPoint3DSet(object_point_set_defining_the_green_square, PointTypes_createPoint3D(0,
                                    WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0));
    PointTypes_addPointToPoint3DSet(object_point_set_defining_the_green_square,
                                    PointTypes_createPoint3D(WIDTH_OF_THE_GREEN_SQUARE_IN_MM, WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0));
    PointTypes_addPointToPoint3DSet(object_point_set_defining_the_green_square,
                                    PointTypes_createPoint3D(WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0, 0));
}

static gboolean gatherUserPointsForCalibration(int point_index)
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
        int enough_user_points_gathered = PointTypes_isPoint2DSetFull(image_point_set_defining_the_green_square);

        if(!enough_user_points_gathered) {
            PointTypes_addPointToPoint2DSet(image_point_set_defining_the_green_square,
                                            PointTypes_createPoint2D(((GdkEventButton*)event)->x * WORLD_CAMERA_WIDTH
                                                    / (double) gtk_widget_get_allocated_width(widget),
                                                    ((GdkEventButton*)event)->y * WORLD_CAMERA_HEIGHT
                                                    / (double) gtk_widget_get_allocated_height(widget)));
            gatherUserPointsForCalibration(number_of_user_points_gathered + 1);
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

    output_camera->camera_status = INTRINSICALLY_CALIBRATED;
    Logger_startMessageSectionAndAppend("Camera intrinsic parameters loaded from file.");

    return TRUE;
}

static double checkReprojectionErrorOnCameraPose(struct Camera *input_camera)
{
    double reprojection_error = 0.0;
    struct Point2DSet *projected_points = NULL;
    projected_points = PointTypes_initializePoint2DSet(NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
    cvProjectPoints2(object_point_set_defining_the_green_square->vector_of_points,
                     input_camera->camera_extrinsics->rotation_vector,
                     input_camera->camera_extrinsics->translation_vector, input_camera->camera_intrinsics->camera_matrix,
                     input_camera->camera_intrinsics->distortion_coefficients, projected_points->vector_of_points, NULL,
                     NULL, NULL, NULL, NULL, 0);

    for(int i = 0; i < NUMBER_OF_CORNERS_OF_GREEN_SQUARE; i++) {
        struct Point2D projected_point = PointTypes_getPointFromPoint2DSet(projected_points, i);
        struct Point2D image_point = PointTypes_getPointFromPoint2DSet(image_point_set_defining_the_green_square, i);
        reprojection_error += pow(projected_point.x - image_point.x, 2.0) + pow(projected_point.y - image_point.y, 2.0);
    }

    reprojection_error = sqrt(reprojection_error / NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
    PointTypes_releasePoint2DSet(projected_points);

    return reprojection_error;
}

static gboolean computeCameraPoseFromUserPoints(struct Camera *input_camera)
{
    int enough_user_points_gathered = PointTypes_isPoint2DSetFull(
                                          image_point_set_defining_the_green_square);

    if(!enough_user_points_gathered) {
        g_idle_add((GSourceFunc) computeCameraPoseFromUserPoints, (gpointer) input_camera);
        return FALSE;
    } else {
        cvFindExtrinsicCameraParams2(object_point_set_defining_the_green_square->vector_of_points,
                                     image_point_set_defining_the_green_square->vector_of_points,
                                     input_camera->camera_intrinsics->camera_matrix,
                                     input_camera->camera_intrinsics->distortion_coefficients,
                                     input_camera->camera_extrinsics->rotation_vector,
                                     input_camera->camera_extrinsics->translation_vector, 0);

        double reprojection_error = checkReprojectionErrorOnCameraPose(input_camera);
        Logger_append("\n Reprojection error on camera pose: ");
        Logger_appendDouble(reprojection_error);
        input_camera->camera_status = INTRINSICALLY_AND_EXTRINSICALLY_CALIBRATED;

        return FALSE; // Even if it succeeds, return FALSE in order to remove this function from the g_idle state.
    }
}

static gboolean computePlaneEquation(struct Camera *input_camera)
{
    int enough_user_points_gathered = PointTypes_isPoint2DSetFull(image_point_set_defining_the_green_square);

    if(!enough_user_points_gathered || input_camera->camera_status != INTRINSICALLY_AND_EXTRINSICALLY_CALIBRATED) {
        g_idle_add((GSourceFunc) computePlaneEquation, (gpointer) input_camera);
        return FALSE;
    } else {
        struct Point3D transformed_origin_point = PointTypes_transformPoint3D(
                    PointTypes_getPointFromPoint3DSet(object_point_set_defining_the_green_square, 0),
                    input_camera->camera_extrinsics->rotation_vector, input_camera->camera_extrinsics->translation_vector);
        struct Point3D transformed_y_axis_point = PointTypes_transformPoint3D(
                    PointTypes_getPointFromPoint3DSet(object_point_set_defining_the_green_square, 1),
                    input_camera->camera_extrinsics->rotation_vector, input_camera->camera_extrinsics->translation_vector);
        struct Point3D transformed_x_axis_point = PointTypes_transformPoint3D(
                    PointTypes_getPointFromPoint3DSet(object_point_set_defining_the_green_square, 3),
                    input_camera->camera_extrinsics->rotation_vector, input_camera->camera_extrinsics->translation_vector);
        struct Point3D y_axis_vector = PointTypes_setPoint3DOrigin(transformed_origin_point,
                                       transformed_y_axis_point);
        struct Point3D x_axis_vector = PointTypes_setPoint3DOrigin(transformed_origin_point,
                                       transformed_x_axis_point);
        struct Point3D normal_vector = PointTypes_point3DCrossProduct(y_axis_vector, x_axis_vector);
        double normal_vector_norm = PointTypes_getNormOfPoint3D(normal_vector);
        input_camera->camera_extrinsics->a = normal_vector.x / normal_vector_norm;
        input_camera->camera_extrinsics->b = normal_vector.y / normal_vector_norm;
        input_camera->camera_extrinsics->c = normal_vector.z / normal_vector_norm;
        input_camera->camera_extrinsics->d = -(normal_vector.x * transformed_origin_point.x +
                                               normal_vector.y * transformed_origin_point.y + normal_vector.z *
                                               transformed_origin_point.z) / normal_vector_norm;
        Logger_startMessageSectionAndAppend("Plane equation: ");
        Logger_appendPlaneEquation(input_camera);
        input_camera->camera_status = FULLY_CALIBRATED;
        return FALSE; // Even if it succeeds, return FALSE in order to remove this function from the g_idle state.
    }
}

static gboolean releaseGatheredUserPointsForCalibration(struct Camera *input_camera)
{
    int enough_user_points_gathered = PointTypes_isPoint2DSetFull(image_point_set_defining_the_green_square);

    if(!enough_user_points_gathered || input_camera->camera_status != FULLY_CALIBRATED) {
        g_idle_add((GSourceFunc) releaseGatheredUserPointsForCalibration, (gpointer) input_camera);
        return FALSE;
    } else {
        PointTypes_releasePoint2DSet(image_point_set_defining_the_green_square);
        PointTypes_releasePoint3DSet(object_point_set_defining_the_green_square);
        return FALSE; // Even if it succeeds, return FALSE in order to remove this function from the g_idle state.
    }
}

gboolean WorldVisionCalibration_calibrate(struct Camera * input_camera)
{
    gatherUserPointsForCalibration(0);
    computeCameraPoseFromUserPoints(input_camera);
    computePlaneEquation(input_camera);
    releaseGatheredUserPointsForCalibration(input_camera);

    return TRUE;
}

static struct Point3D compute3DPointOnPlaneInCameraFrame(struct Point2D image_coordinates,
        struct Camera *input_camera)
{
    double fx = cvmGet(input_camera->camera_intrinsics->camera_matrix, 0, 0);
    double fy = cvmGet(input_camera->camera_intrinsics->camera_matrix, 1, 1);
    double cx = cvmGet(input_camera->camera_intrinsics->camera_matrix, 0, 2);
    double cy = cvmGet(input_camera->camera_intrinsics->camera_matrix, 1, 2);
    struct Point2D normalized_image_coordinates = PointTypes_createPoint2D((image_coordinates.x - cx) / fx,
            (image_coordinates.y - cy) / fy);
    double scale_factor = -input_camera->camera_extrinsics->d / (input_camera->camera_extrinsics->a *
                          normalized_image_coordinates.x + input_camera->camera_extrinsics->b * normalized_image_coordinates.y +
                          input_camera->camera_extrinsics->c);
    struct Point3D point = PointTypes_createPoint3D(scale_factor * normalized_image_coordinates.x,
                           scale_factor * normalized_image_coordinates.y, scale_factor);

    return point;
}

struct Point3D WorldVisionCalibration_convertImageCoordinatesToWorldCoordinates(struct Point2D image_coordinates,
        struct Camera *input_camera)
{
    struct Point3D result_in_camera_frame = compute3DPointOnPlaneInCameraFrame(image_coordinates, input_camera);
    struct Point3D result = PointTypes_transformInversePoint3D(result_in_camera_frame,
                            input_camera->camera_extrinsics->rotation_vector,
                            input_camera->camera_extrinsics->translation_vector);

    return result;
}