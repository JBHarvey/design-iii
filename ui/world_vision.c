#include "opencv2/videoio/videoio_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "world_vision.h"
#include "ui_event.h"
#include <errno.h>

const int WORLD_CAMERA_WIDTH = 1600;
const int WORLD_CAMERA_HEIGHT = 1200;

GdkPixbuf* world_camera_pixbuf;

static gboolean initialize_camera_matrix_and_distortion_coefficients_from_file(GtkWidget *widget, CvMat *camera_matrix,
        CvMat *distortion_coefficients);

gpointer world_camera_feeder(gpointer data)
{
    CvCapture *cv_cap;
    IplImage* frame;
    IplImage* frame_BGR;

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
        frame_BGR = cvQueryFrame(cv_cap);

        if(!frame) {
            frame = cvCreateImage(cvGetSize((CvArr*) frame_BGR), IPL_DEPTH_8U, 3);
        }

        cvCvtColor(frame_BGR, frame, CV_BGR2RGB);
        world_camera_pixbuf = gdk_pixbuf_new_from_data((guchar*) frame->imageData,
                              GDK_COLORSPACE_RGB, FALSE, frame->depth, frame->width,
                              frame->height, (frame->widthStep), NULL, NULL);
    }

    cvReleaseCapture(&cv_cap);
    cvReleaseImage(&frame_BGR);
    cvReleaseImage(&frame);

    return NULL;
}

gboolean world_camera_draw_event_callback(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    return draw_event_callback(widget, event, world_camera_pixbuf);
}

gboolean world_camera_calibration_clicked_event_callback(GtkWidget *widget, gpointer data)
{
    CvMat* camera_matrix = cvCreateMat(3, 3, CV_64F);
    CvMat* distortion_coefficients = cvCreateMat(5, 1, CV_64F);

    initialize_camera_matrix_and_distortion_coefficients_from_file(widget, camera_matrix, distortion_coefficients);

    cvReleaseMat(&camera_matrix);
    cvReleaseMat(&distortion_coefficients);

    return TRUE;
}

static gboolean initialize_camera_matrix_and_distortion_coefficients_from_file(GtkWidget *widget, CvMat *camera_matrix,
        CvMat *distortion_coefficients)
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
            CvFileStorage *fs = cvOpenFileStorage(filename, NULL, CV_STORAGE_READ, NULL);

            camera_matrix = (CvMat*) cvReadByName(fs, NULL, "Camera_Matrix", NULL);
            distortion_coefficients = (CvMat*) cvReadByName(fs, NULL, "Distortion_Coefficients", NULL);

            if(camera_matrix == NULL || distortion_coefficients == NULL) {
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

            cvReleaseFileStorage(&fs);
            g_free(filename);
            gtk_widget_destroy(dialog);
        } else if(response == GTK_RESPONSE_CANCEL) {
            gtk_widget_destroy(dialog);

            return FALSE;
        }
    } while(!isCalibrationFileValid);

    return TRUE;
}
