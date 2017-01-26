 
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"
#include <stdio.h>

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle( CvPoint* pt1, CvPoint* pt2, CvPoint* pt0 )
{
    double dx1 = pt1->x - pt0->x;
    double dy1 = pt1->y - pt0->y;
    double dx2 = pt2->x - pt0->x;
    double dy2 = pt2->y - pt0->y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

_Bool detect_square(CvSeq* contours)
{
    //printf("%u %u %u\n", contours->total, abs(cvContourArea(contours,CV_WHOLE_SEQ, 0)), cvCheckContourConvexity(contours));
                 if( contours->total == 4 &&
                    fabs(cvContourArea(contours,CV_WHOLE_SEQ, 0)) > 1000 &&
                    cvCheckContourConvexity(contours) )
                {
                    double s = 0;

                    unsigned int i;
                    for( i = 0; i < 5; i++ )
                    {
                        // find minimum angle between joint
                        // edges (maximum of cosine)
                        if( i >= 2 )
                        {
                            double t = fabs(angle(
                            (CvPoint*)cvGetSeqElem( contours, i ),
                            (CvPoint*)cvGetSeqElem( contours, i-2 ),
                            (CvPoint*)cvGetSeqElem( contours, i-1 )));
                            s = s > t ? s : t;
                        }
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    //printf("%lf\n", s);
                    if( s < 0.5 )
                        for( i = 0; i < 4; i++ ) {
                            //printf("square detected\n");
                            return 1;
                            //cvSeqPush( squares,
                            //    (CvPoint*)cvGetSeqElem( result, i ));
                        }
                }
                return 0;
}

_Bool detect_dualSquare(CvSeq* contours) {
    if (contours->v_next && detect_square(contours) && detect_square(contours->v_next))
        return 1;

    return 0;
}

int main(int argc,char *argv[])
{
    int c;
    IplImage* img;
    CvCapture* cv_cap = cvCaptureFromCAM(1);
    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_WIDTH, 1600);
    cvSetCaptureProperty(cv_cap, CV_CAP_PROP_FRAME_HEIGHT, 1200);
    
    cvNamedWindow("Video",0); // create window
    cvNamedWindow("Video-orig",0); // create window
    cvNamedWindow("Video-lines",0); // create window
    cvNamedWindow("Video-innersquare",0); // create window
    CvMemStorage* storage = cvCreateMemStorage(0);
    
    for(;;) {
        img = cvQueryFrame(cv_cap); // get frame
   
        if(img != 0) {
            cvShowImage("Video-orig", img);
            cvSmooth(img, img, CV_GAUSSIAN, 5, 0, 0, 0);
            //cvShowImage("Video", color_img); // show frame
            IplImage* img_yuv = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
            cvCvtColor(img, img_yuv, CV_BGR2YCrCb);
            CvScalar upper_bound = cvScalar(246, 120, 120, 255);
            CvScalar lower_bound = cvScalar(10, 0, 0, 255);
            //IplImage* img_green = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
            

            IplImage* im_bw = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
            cvInRangeS(img_yuv, lower_bound, upper_bound, im_bw);
            //printf("test\n");
            //cvThreshold(img, im_bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
            cvDilate(im_bw, im_bw, NULL, 2);
            cvErode(im_bw, im_bw, NULL, 4);
            cvDilate(im_bw, im_bw, NULL, 2);
            //cvAdaptiveThreshold(img, im_bw, 255,  CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 101, 0);
            /*
            IplImage* img32f = cvCreateImage( cvSize(w,w), IPL_DEPTH_32F, 1 );
            IplImage* img32s = cvCreateImage( cvSize(w,w), IPL_DEPTH_32S, 1 );*/
            //IplImage* img3 = cvCloneImage(img);//cvCreateImage( cvGetSize(img), 8, 3 );
            cvShowImage("Video", im_bw);

            CvSeq* contours = 0;
            if (cvFindContours( im_bw, storage, &contours, sizeof(CvContour),
                    CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) )) {

                contours = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 200, 1);

                contours;
                //contours
                //cvDrawContours( img, contours, CV_RGB(255,0,0), CV_RGB(0,255,0), 8, 3, CV_AA, cvPoint(0,0) );
                
                CvSeq* h_seq = contours;
                while (h_seq) {
                    CvSeq* v_seq = h_seq;
                while (v_seq) {
                    if (detect_dualSquare(v_seq)) {
                    //printf("detected dual square\n");
                    //cvDrawContours( img, v_seq->v_next, CV_RGB(255,0,0), CV_RGB(0,255,0), 0, CV_FILLED, 8, cvPoint(0,0) );
                        {
                            CvPoint2D32f src[4];
                            src[0] = cvPointTo32f(*(CvPoint*)cvGetSeqElem( v_seq->v_next, 0));
                            src[1] = cvPointTo32f(*(CvPoint*)cvGetSeqElem( v_seq->v_next, 1));
                            src[2] = cvPointTo32f(*(CvPoint*)cvGetSeqElem( v_seq->v_next, 2));
                            src[3] = cvPointTo32f(*(CvPoint*)cvGetSeqElem( v_seq->v_next, 3));
                            
                            //printf("%f, %f  %f, %f\n", src[0].x, src[0].y, src[3].x, src[3].y);
                            /* fix rotation*/
                            if (src[0].x * src[0].y > src[3].x * src[3].y) {
                                CvPoint2D32f temp = src[0];
                                src[0] = src[3];
                                src[3] = src[2];
                                src[2] = src[1];
                                src[1] = temp;
                            }
                            
                            CvPoint2D32f dst[4];
                            dst[0] = cvPoint2D32f(0, 0);
                            dst[1] = cvPoint2D32f(1000, 0);
                            dst[2] = cvPoint2D32f(1000, 1000);
                            dst[3] = cvPoint2D32f(0, 1000);

                            CvMat *perspective_matrix = cvCreateMat(3, 3, CV_32F);
                            perspective_matrix = cvGetPerspectiveTransform(src, dst, perspective_matrix);

                            IplImage* im_square_image = cvCreateImage(cvSize(1000,1000),IPL_DEPTH_8U,3);
                            cvWarpPerspective(img_yuv, im_square_image, perspective_matrix, CV_INTER_LINEAR, cvScalarAll(0));
                            cvReleaseMat(&perspective_matrix);
                            
                            {
                                IplImage* im_y = cvCreateImage(cvGetSize(im_square_image),IPL_DEPTH_8U,1);
                                IplImage* im_u = cvCreateImage(cvGetSize(im_square_image),IPL_DEPTH_8U,1);
                                IplImage* im_v = cvCreateImage(cvGetSize(im_square_image),IPL_DEPTH_8U,1);
                                
                                cvSplit(im_square_image, im_y, im_u, im_v, 0);
                                
                                IplImage* im_bw_inner = cvCreateImage(cvGetSize(im_square_image),IPL_DEPTH_8U,1);
                                
                                CvScalar upper_bound = cvScalar(256, 139, 139, 255);
                                CvScalar lower_bound = cvScalar(0, 115, 115, 255);
                                cvInRangeS(im_square_image, lower_bound, upper_bound, im_bw_inner);

                                //cvThreshold(im_u, im_bw_inner, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

                                
                                cvErode(im_bw_inner, im_bw_inner, NULL, 2);
                                cvDilate(im_bw_inner, im_bw_inner, NULL, 2);
                                cvDilate(im_bw_inner, im_bw_inner, NULL, 2);
                                cvErode(im_bw_inner, im_bw_inner, NULL, 2);

                                
                                CvSeq* contours_inner = 0;
                                
                                if (cvFindContours( im_bw_inner, storage, &contours_inner, sizeof(CvContour),
                                        CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) )) {
                                    contours_inner = cvApproxPoly(contours_inner, sizeof(CvContour), storage, CV_POLY_APPROX_DP, 20, 1);
                                
                                    cvDrawContours( im_square_image, contours_inner, CV_RGB(255,0,0), CV_RGB(0,255,0), 8, 3, CV_AA, cvPoint(0,0) );
                                    cvShowImage("Video-innersquare", im_square_image);
                                }
                                //cvShowImage("Video-innersquare", im_bw_inner);
                                cvReleaseImage( &im_bw_inner );
                                cvReleaseImage( &im_y );
                                cvReleaseImage( &im_u );
                                cvReleaseImage( &im_v );
                                
                            }
                            
                            //cvShowImage("Video-innersquare", im_square_image);
                            cvReleaseImage( &im_square_image );
                        }
                    }
                    v_seq = v_seq->v_next;
                }
                    h_seq = h_seq->h_next;
                
                }

            }

            cvShowImage("Video-lines", img);
            
            cvReleaseImage( &im_bw );
            cvReleaseImage( &img_yuv );

        }
        c = cvWaitKey(10); // wait 10 ms or for key stroke
        if(c == 27)
            break; // if ESC, break and quit
    }
    /* clean up */
    cvReleaseCapture( &cv_cap );
    cvReleaseMemStorage( &storage );
    cvDestroyWindow("Video");
}
