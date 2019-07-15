#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

//#define HRES 640
//#define VRES 480

#define DHRES 640
#define DVRES 480

#define ESC_KEY 27



int MotionDetect();


static FILE *motionFilePtr= fopen("monitorLog.txt", "a");    //log file
static FILE *motionDataPtr= fopen("motionData.txt", "a");  //save data
