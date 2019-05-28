#ifndef LOCALIZATION_HPP
#define LOCALIZATION_HPP

#include <stdio.h>
#include <iostream> 
#include <string> //for time
#include <iomanip> // for GPS I/O
#include <ctime> //for converting time
#include <sstream> //provides string stream classes
#include <unistd.h> //for syslog
#include <syslog.h> //for syslog
#include <assert.h> //for asserts
#include <cmath>  //for finding distances between coordinates
#include <time.h> //for timestamp for waiting.
#include <libgpsmm.h> // for GPS



using namespace std; //won't compile stings without this

//add class to .h file when created to match style guide.
class LOCATION
{
  private:
  struct GPS_DATA
  {
    //the lat/long data is in DBUS_TYPE_DOUBLE but will not compile.
    // will convert when necessary.
    double latitude, longitude;
    string currentTime;
  };
  GPS_DATA gpsData; 
  GPS_DATA *dataToSave = &gpsData;
  
  public:
  //funcitons LOCATION
  int saveGPSData(double, double, string);
  double gpsComp(double,double);
  LOCATION();
  ~LOCATION();
  
};

static FILE *locationFilePtr= fopen("monitorLog.txt", "a");
static FILE *locationDataPtr= fopen("locationData.txt", "a");
//add a file to save GPS data done? ^^
void trackGPS();
void spoofGPS();



#endif
