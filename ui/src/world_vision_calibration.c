#include <errno.h>
#include <math.h>
#include "opencv2/calib3d/calib3d_c.h"

#include "world_vision_calibration.h"
#include "world_vision_detection.h"
#include "logger.h"

/* Constants */

const int NUMBER_OF_CORNERS_OF_GREEN_SQUARE = 4;
const int NUMBER_OF_CORNERS_OF_THE_TABLE = 4;
const int TWO_DIMENSIONS = 2;
const int TRHEE_DIMENSIONS = 3;
const int WIDTH_OF_THE_TABLE_IN_MM = 1109;
const int LENGTH_OF_THE_TABLE_IN_MM = 2306;
const int WIDTH_OF_THE_GREEN_SQUARE_IN_MM = 660;

/* Global variables */

extern GMutex world_vision_camera_intrinsics_mutex;

struct Point2DSet *image_point_set;
struct Point3DSet *object_point_set;

static void initializeTablePointsArraysForCameraPoseComputation(void)
{
    image_point_set = WorldVisionDetection_getDetectedTableCorners();
    object_point_set = PointTypes_initializePoint3DSet(NUMBER_OF_CORNERS_OF_THE_TABLE);

    PointTypes_addPointToPoint3DSet(object_point_set, PointTypes_createPoint3D(0, 0, 0));
    PointTypes_addPointToPoint3DSet(object_point_set, PointTypes_createPoint3D(0, WIDTH_OF_THE_TABLE_IN_MM, 0));
    PointTypes_addPointToPoint3DSet(object_point_set, PointTypes_createPoint3D(LENGTH_OF_THE_TABLE_IN_MM,
                                    WIDTH_OF_THE_TABLE_IN_MM, 0));
    PointTypes_addPointToPoint3DSet(object_point_set, PointTypes_createPoint3D(LENGTH_OF_THE_TABLE_IN_MM, 0, 0));
}

static void initializeGreenSquarePointsArraysForCameraPoseComputation(void)
{
    image_point_set = WorldVisionDetection_getDetectedGreenSquareCorners();
    object_point_set = PointTypes_initializePoint3DSet(NUMBER_OF_CORNERS_OF_GREEN_SQUARE);

    PointTypes_addPointToPoint3DSet(object_point_set, PointTypes_createPoint3D(0, 0, 0));
    PointTypes_addPointToPoint3DSet(object_point_set, PointTypes_createPoint3D(0, WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0));
    PointTypes_addPointToPoint3DSet(object_point_set, PointTypes_createPoint3D(WIDTH_OF_THE_GREEN_SQUARE_IN_MM,
                                    WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0));
    PointTypes_addPointToPoint3DSet(object_point_set, PointTypes_createPoint3D(WIDTH_OF_THE_GREEN_SQUARE_IN_MM, 0, 0));
}

static gboolean setCameraMatrixAndDistortionCoefficientsFromFile(struct CameraIntrinsics
        *output_camera_intrinsics, const char *filename)
{
    gboolean is_calibration_file_valid = TRUE;
    CvFileStorage *file_storage = cvOpenFileStorage(filename, NULL, CV_STORAGE_READ, NULL);

    g_mutex_lock(&world_vision_camera_intrinsics_mutex);

    output_camera_intrinsics->camera_matrix = (CvMat*) cvReadByName(file_storage, NULL, "Camera_Matrix", NULL);
    output_camera_intrinsics->distortion_coefficients = (CvMat*) cvReadByName(file_storage, NULL, "Distortion_Coefficients",
            NULL);

    if(output_camera_intrinsics->camera_matrix == NULL || output_camera_intrinsics->distortion_coefficients == NULL) {
        is_calibration_file_valid = FALSE;
    }

    g_mutex_unlock(&world_vision_camera_intrinsics_mutex);

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
    projected_points = PointTypes_initializePoint2DSet(NUMBER_OF_CORNERS_OF_THE_TABLE);
    cvProjectPoints2(object_point_set->vector_of_points,
                     input_camera->camera_extrinsics->rotation_vector,
                     input_camera->camera_extrinsics->translation_vector, input_camera->camera_intrinsics->camera_matrix,
                     input_camera->camera_intrinsics->distortion_coefficients, projected_points->vector_of_points, NULL,
                     NULL, NULL, NULL, NULL, 0);

    for(int i = 0; i < NUMBER_OF_CORNERS_OF_GREEN_SQUARE; i++) {
        struct Point2D projected_point = PointTypes_getPointFromPoint2DSet(projected_points, i);
        struct Point2D image_point = PointTypes_getPointFromPoint2DSet(image_point_set, i);
        reprojection_error += pow(projected_point.x - image_point.x, 2.0) + pow(projected_point.y - image_point.y, 2.0);
    }

    reprojection_error = sqrt(reprojection_error / NUMBER_OF_CORNERS_OF_GREEN_SQUARE);
    PointTypes_releasePoint2DSet(projected_points);

    return reprojection_error;
}

static void computeCameraPose(struct Camera *input_camera)
{
    cvFindExtrinsicCameraParams2(object_point_set->vector_of_points,
                                 image_point_set->vector_of_points,
                                 input_camera->camera_intrinsics->camera_matrix,
                                 input_camera->camera_intrinsics->distortion_coefficients,
                                 input_camera->camera_extrinsics->rotation_vector,
                                 input_camera->camera_extrinsics->translation_vector, 0);
}

static void computePlaneEquation(struct Camera *input_camera)
{
    struct Point3D transformed_origin_point = PointTypes_transformPoint3D(
                PointTypes_getPointFromPoint3DSet(object_point_set, 0),
                input_camera->camera_extrinsics->rotation_vector, input_camera->camera_extrinsics->translation_vector);
    struct Point3D transformed_y_axis_point = PointTypes_transformPoint3D(
                PointTypes_getPointFromPoint3DSet(object_point_set, 1),
                input_camera->camera_extrinsics->rotation_vector, input_camera->camera_extrinsics->translation_vector);
    struct Point3D transformed_x_axis_point = PointTypes_transformPoint3D(
                PointTypes_getPointFromPoint3DSet(object_point_set, 3),
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
}

static void releasePointsUsedForCalibration(void)
{
    PointTypes_releasePoint2DSet(image_point_set);
    PointTypes_releasePoint3DSet(object_point_set);
}

void WorldVisionCalibration_calibrateWithTableCorners(struct Camera * input_camera)
{
    double reprojection_error;

    while(!WorldVisionDetection_detectTableCorners(input_camera)) {

        while(gtk_events_pending()) {
            gtk_main_iteration();
        }
    }

    initializeTablePointsArraysForCameraPoseComputation();
    computeCameraPose(input_camera);
    reprojection_error = checkReprojectionErrorOnCameraPose(input_camera);
    computePlaneEquation(input_camera);
    releasePointsUsedForCalibration();
    input_camera->camera_status = FULLY_CALIBRATED;
    Logger_startMessageSectionAndAppend("Camera pose computation completed.");
    Logger_append("\nReprojection error on camera pose: ");
    Logger_appendDouble(reprojection_error);
    Logger_append("\nPlane equation: ");
    Logger_appendPlaneEquation(input_camera);
}

void WorldVisionCalibration_calibrateWithGreenSquareCorners(struct Camera *input_camera)
{
    double reprojection_error;

    while(!WorldVisionDetection_detectGreenSquareCorners(input_camera)) {
        while(gtk_events_pending()) {
            gtk_main_iteration();
        }
    }

    initializeGreenSquarePointsArraysForCameraPoseComputation();
    computeCameraPose(input_camera);
    reprojection_error = checkReprojectionErrorOnCameraPose(input_camera);
    computePlaneEquation(input_camera);
    releasePointsUsedForCalibration();
    input_camera->camera_status = FULLY_CALIBRATED;
    Logger_startMessageSectionAndAppend("Camera pose computation completed.");
    Logger_append("\nReprojection error on camera pose: ");
    Logger_appendDouble(reprojection_error);
    Logger_append("\nPlane equation: ");
    Logger_appendPlaneEquation(input_camera);
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

void WorldVisionCalibration_convertWorldCoordinatesSetToImageCoordinatesSet(
    struct Point3DSet *world_coordinates_set, struct Point2DSet *output_images_coordinates_set, struct Camera *input_camera)
{
    cvProjectPoints2(world_coordinates_set->vector_of_points,
                     input_camera->camera_extrinsics->rotation_vector,
                     input_camera->camera_extrinsics->translation_vector, input_camera->camera_intrinsics->camera_matrix,
                     input_camera->camera_intrinsics->distortion_coefficients, output_images_coordinates_set->vector_of_points, NULL,
                     NULL, NULL, NULL, NULL, 0);
    int number_of_points = PointTypes_getNumberOfPointStoredInPoint3DSet(world_coordinates_set);
    output_images_coordinates_set->number_of_points = number_of_points;
    output_images_coordinates_set->filled_up_to_index = number_of_points * 2;
}

struct Point2D WorldVisionCalibration_convertWorldCoordinatesToImageCoordinates(
    struct Point3D world_coordinates, struct Camera *input_camera)
{
    struct Point3DSet *world_coordinates_set = PointTypes_initializePoint3DSet(2);
    PointTypes_addPointToPoint3DSet(world_coordinates_set, world_coordinates);
    PointTypes_addPointToPoint3DSet(world_coordinates_set, PointTypes_createPoint3D(0, 0, 0));

    struct Point2DSet *image_coordinates_set = PointTypes_initializePoint2DSet(2);
    WorldVisionCalibration_convertWorldCoordinatesSetToImageCoordinatesSet(world_coordinates_set, image_coordinates_set,
            input_camera);
    struct Point2D image_coordinates = PointTypes_getPointFromPoint2DSet(image_coordinates_set, 0);
    PointTypes_releasePoint3DSet(world_coordinates_set);
    PointTypes_releasePoint2DSet(image_coordinates_set);

    return image_coordinates;
}
