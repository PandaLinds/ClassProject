/*
 *
 *  Example by Sam Siewert 
 *
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

//#define HRES 640
//#define VRES 480

#define DHRES 640
#define DVRES 480

#define ESC_KEY 27

char snapshotfile[]  = "save1/snapshot00000.jpg";
char framesavefile[] = "save1/frame00000000.jpg";

char snapshotfile2[]  = "save2/snapshot00000.jpg";
char framesavefile2[] = "save2/frame00000000.jpg";
    
IplImage* difframe2;
IplImage* difframe2_gray;
IplImage* difframe2_bw;

int main( int argc, char** argv )
{
    IplImage* frame;

    cvNamedWindow("Capture Example 2", CV_WINDOW_AUTOSIZE);
    CvCapture* capture2 = cvCreateCameraCapture(0);
    IplImage* frame2;
    IplImage* prevframe2;

    double diffSum=0.0;

    int cnt=0; int savingdata=0;
    int motioncnt=0;
    Mat mat_difframe2_gray;
    
    cvSetCaptureProperty(capture2, CV_CAP_PROP_FRAME_WIDTH, DHRES);
    cvSetCaptureProperty(capture2, CV_CAP_PROP_FRAME_HEIGHT, DVRES);
    
    frame2=cvQueryFrame(capture2);
    prevframe2=cvCreateImage(cvGetSize(frame2), frame2->depth, frame2->nChannels);
    difframe2=cvCreateImage(cvGetSize(frame2), frame2->depth, frame2->nChannels);
    difframe2_bw=cvCreateImage(cvGetSize(frame2),IPL_DEPTH_8U,1);
    
    Mat mat_difframe2 = cvarrToMat(difframe2); //stackoverflow.com/questions/15925084/conversion-from-iplimage-to-cvmat
    Mat mat_difframe2_bw = cvarrToMat(difframe2_bw); //2nd answer(addresses constructor removal)
	
    cvCopy(frame2, prevframe2);

    cvAbsDiff(frame2, prevframe2, difframe2);
    cvtColor(mat_difframe2, mat_difframe2_gray, CV_RGB2GRAY);
    threshold(mat_difframe2_gray, mat_difframe2_bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	
    while(1) //only works if press M first
    {
        
        frame=cvQueryFrame(capture2);
     
        if(!frame || !frame2) break;
        
        cvAbsDiff(frame2, prevframe2, difframe2);
        cvtColor(mat_difframe2, mat_difframe2_gray, CV_RGB2GRAY);
        
        threshold(mat_difframe2_gray, mat_difframe2_bw, 10, 1, CV_THRESH_BINARY);
        diffSum = (sum(mat_difframe2_bw)[0]);

        cvShowImage("Capture Example 2", frame);
        
        //printf("diffSum=%lf, motioncnt=%d\n", diffSum, motioncnt);

        char c = cvWaitKey(33);
        if( c == ESC_KEY ) break;
        
        if( c == 'p') 
        {
			// on "p" key press
            cnt++;
            
            printf("snapshot %05d taken as %s\n", cnt, snapshotfile2);
            sprintf(&snapshotfile2[8], "%05d.jpg", cnt);
            imwrite(framesavefile2, cvarrToMat(frame2));
        }
        
        if( c == 's') 
        {
            // on "s" key press
            savingdata++;
            
            printf("frame %08d saved as %s\n", savingdata, framesavefile2);
            sprintf(&framesavefile2[8], "%08d.png", savingdata);
            imwrite(framesavefile2, cvarrToMat(frame2));
        }
        
        if((motioncnt && (diffSum > 800)) || (c == 'm') ) 
        {
            // on "m" key press
            motioncnt++;


            printf("frame %08d saved as %s\n", motioncnt, framesavefile2);
            sprintf(&framesavefile2[8], "%08d.png", motioncnt);
            imwrite(framesavefile2, cvarrToMat(frame2));
        }
               
        if( c == 'q' && (savingdata || motioncnt)) 
        {
            savingdata=0;
            motioncnt=0;
        }
        
        cvCopy(frame2, prevframe2);
    }

    cvReleaseCapture(&capture2);
    cvDestroyWindow("Capture Example 2");
    
};







