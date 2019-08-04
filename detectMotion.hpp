#include <iostream>
#include <fstream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <ctime>
#include <time.h>
#include <dirent.h>
#include <sstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

//from motion detect
#define CAMERA
#define WINDOW_NAME "Motion Detector"

#define DEVICE_ID 0

//#define SHOW_DIFF

#define DELAY_IN_MSEC (15)

#define DETECT_DIRECTORY ("mnt/data/detect/")
#define COLLECT_DIRECTORY ("mnt/data/collect/")

#define COLLECT_EVERY_NTH_FRAME 30

#define FILE_FORMAT ("%d%h%Y_%H%M%S") // 1Jan1970/1Jan1970_12153
#define EXTENSION (".png") // extension of the images

#define MAX_THRESHOLD 255
#define MAX_DEVIATION 255 // Maximum allowable deviation of a pixel to count as "changed"

#define ESC_KEY 27


#ifdef LOW_RES_DETECTION
static int currentThreshold = 5;
static int currentDeviation = 3;
static int currentMotionTrigger = 10;
#else
//int currentThreshold = 15;
//int currentDeviation = 9;
//int currentMotionTrigger = 30;
static int currentThreshold = 20;
static int currentDeviation = 15;
static int currentMotionTrigger = 75;
#endif

#define CAM_WIDTH_OFFSET 0

using namespace std;
using namespace cv;

static bool running = true;

typedef struct {
  bool isMotion;
  Scalar mean;
  Scalar stddev;
  int numberOfChanges;
} MotionDetectData_t;







inline bool saveImg(Mat image, const string directory,
                    const string extension, const string fileFormat,
                    const int sequence, int traceOn);
inline void directoryExistsOrCreate(const char* pzPath);
inline MotionDetectData_t detectMotion(const Mat &motion, 
                                       int max_deviation, 
                                       int triggerCount);
