#include "opencv2/videoio/videoio_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/calib3d/calib3d_c.h"
#include "world_vision.h"
#include "ui_event.h"
#include <errno.h>

enum thread_status {TERMINATED, RUNNING};

enum thread_status main_loop_status;

const int WORLD_CAMERA_WIDTH = 1280;
const int WORLD_CAMERA_HEIGHT = 720;

enum camera_status {UNCALIBRATED, CALIBRATED};

struct camera_intrinsics {
    CvMat *camera_matrix;
    CvMat *distortion_coefficients;
};

enum camera_status world_camera_status = UNCALIBRATED;
struct camera_intrinsics *world_camera_intrinsics;
GdkPixbuf *world_camera_pixbuf = NULL;

static gboolean initialize_camera_matrix_and_distortion_coefficients_from_file(GtkWidget *widget);

void set_main_loop_status_running(void)
{
    g_mutex_lock(&mutex);
    main_loop_status = RUNNING;
    g_mutex_unlock(&mutex);
}

void set_main_loop_status_terminated(void)
{
    g_mutex_lock(&mutex);
    main_loop_status = TERMINATED;
    g_mutex_unlock(&mutex);
}

gpointer world_camera_feeder(gpointer data)
{
    CvCapture *cv_cap;
    IplImage* frame = NULL;
    IplImage* frame_BGR = NULL;
    IplImage* frame_BGR_corrected = NULL;

    world_camera_intrinsics = (struct camera_intrinsics *) malloc(sizeof(struct camera_intrinsics));
    world_camera_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, IPL_DEPTH_8U, WORLD_CAMERA_WIDTH,
                                         WORLD_CAMERA_HEIGHT);

    /* Find camera. 0 is for the embedded webcam. */
    for(int i = 1; i < 100; i++) {
        cv_cap = cvCreateCameraCapture(i);

        if(cv_cap) {
            break;
        }
    }

    if(!cv_cap) {
        cv_cap = cvCreateCameraCapture(0);
    }

    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_WIDTH, WORLD_CAMERA_WIDTH);
    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_HEIGHT, WORLD_CAMERA_HEIGHT);

    while(TRUE) {
        g_mutex_lock(&mutex);

        if(main_loop_status == TERMINATED) {
            g_object_unref(world_camera_pixbuf);
            g_mutex_unlock(&mutex);
            cvReleaseCapture(&cv_cap);

            if(world_camera_status != UNCALIBRATED) {
                cvReleaseMat(&(world_camera_intrinsics->camera_matrix));
                cvReleaseMat(&(world_camera_intrinsics->distortion_coefficients));
            }

            free(world_camera_intrinsics);
            g_thread_exit(0);
        } else {
            g_mutex_unlock(&mutex);
        }

        frame_BGR = cvQueryFrame(cv_cap);
        frame = cvCloneImage(frame_BGR);

        if(world_camera_status == CALIBRATED) {
            CvMat *optimal_camera_matrix = cvCreateMat(3, 3, CV_64F);
            cvGetOptimalNewCameraMatrix(world_camera_intrinsics->camera_matrix, world_camera_intrinsics->distortion_coefficients,
                                        cvSize(WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT),
                                        1.0, optimal_camera_matrix,
                                        cvSize(WORLD_CAMERA_WIDTH, WORLD_CAMERA_HEIGHT), 0, 0);
            frame_BGR_corrected = cvCloneImage(frame_BGR);
            cvUndistort2(frame_BGR, frame_BGR_corrected, world_camera_intrinsics->camera_matrix,
                         world_camera_intrinsics->distortion_coefficients, optimal_camera_matrix);
            cvReleaseMat(&optimal_camera_matrix);
        } else if(world_camera_status == UNCALIBRATED) {
            frame_BGR_corrected = cvCloneImage(frame_BGR);
        }

        cvCvtColor((CvArr*) frame_BGR_corrected, frame, CV_BGR2RGB);

        g_mutex_lock(&mutex);
        g_object_unref(world_camera_pixbuf);

        if(frame != NULL) {
            world_camera_pixbuf = gdk_pixbuf_new_from_data((guchar*) frame->imageData,
                                  GDK_COLORSPACE_RGB, FALSE, frame->depth, frame->width,
                                  frame->height, (frame->widthStep), NULL, NULL);
        }

        g_mutex_unlock(&mutex);

        cvReleaseImage(&frame_BGR_corrected);
        cvReleaseImage(&frame);
    }

    return NULL;
}

gboolean world_camera_draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    return draw_event_callback(widget, event, world_camera_pixbuf);
}

gboolean world_camera_calibration_clicked_event_callback(GtkWidget *widget, gpointer data)
{
    if(initialize_camera_matrix_and_distortion_coefficients_from_file(widget)) {
    }

    return TRUE;
}

static gboolean initialize_camera_matrix_and_distortion_coefficients_from_file(GtkWidget *widget)
{
    gboolean isCalibrationFileValid;

    do {
        isCalibrationFileValid = TRUE;
        GtkWidget *dialog = gtk_file_chooser_dialog_new("open calibration xml file",
                            GTK_WINDOW(widget),
                            GTK_FILE_CHOOSER_ACTION_OPEN,
                            "cancel", GTK_RESPONSE_CANCEL,
                            "select", GTK_RESPONSE_ACCEPT,
                            NULL);
        GtkFileFilter *filter = gtk_file_filter_new();
        gint response;

        gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
        gtk_file_filter_set_name(filter, "xml data file");
        gtk_file_filter_add_mime_type(filter, "application/xml");
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
        response = gtk_dialog_run(GTK_DIALOG(dialog));

        if(response == GTK_RESPONSE_ACCEPT) {
            char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            CvFileStorage *file_storage = cvOpenFileStorage(filename, NULL, CV_STORAGE_READ, NULL);
            world_camera_intrinsics->camera_matrix = (CvMat*) cvReadByName(file_storage, NULL, "Camera_Matrix", NULL);
            world_camera_intrinsics->distortion_coefficients = (CvMat*) cvReadByName(file_storage, NULL, "Distortion_Coefficients",
                    NULL);

            if(world_camera_intrinsics->camera_matrix == NULL || world_camera_intrinsics->distortion_coefficients == NULL) {
                GtkWidget *errorDialog = gtk_message_dialog_new(GTK_WINDOW(widget),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_CLOSE,
                                         "Error reading “%s”: %s.\n Please, select a valid XML calibration file.",
                                         filename,
                                         g_strerror(EINVAL));
                gtk_dialog_run(GTK_DIALOG(errorDialog));
                gtk_widget_destroy(errorDialog);
                isCalibrationFileValid = FALSE;
            }

            cvReleaseFileStorage(&file_storage);
            g_free(filename);
            gtk_widget_destroy(dialog);
        } else if(response == GTK_RESPONSE_CANCEL) {
            gtk_widget_destroy(dialog);

            return FALSE;
        }
    } while(!isCalibrationFileValid);

    world_camera_status = CALIBRATED;

    return TRUE;
}
