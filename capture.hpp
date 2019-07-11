#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int detect();

static FILE *motionFilePtr= fopen("monitorLog.txt", "a");    //log file
static FILE *motionDataPtr= fopen("motionData.txt", "a");  //save data
