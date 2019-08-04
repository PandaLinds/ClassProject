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

#define GOOD 0
#define BAD (-1)
#define WAIT_TOLERANCE 100
#define CHANGE_SAVED 2


using namespace std; //won't compile stings without this



//Class for localization
class LOCATION
{
  private:  //Struct is private so the data can't be manipulated. 
  struct GPS_DATA
  {
    //the lat/long data is in DBUS_TYPE_DOUBLE but will not compile.
    // will convert when necessary.
    double latitude, longitude;
    string currentTime;
  };
  GPS_DATA gpsData; 
  GPS_DATA *dataToSave = &gpsData;
  
  public:  //functions that can be used to save and compare data
  //funcitons LOCATION
  int enableGPS();
  int findSignal();
  int checkGPSData();
  int saveGPSData(double, double, string);
  string currentLoc();
  double gpsComp(double,double);
  LOCATION();
  ~LOCATION();
  
};

static FILE *locationFilePtr= fopen("monitorLog.txt", "a");    //log file
static FILE *locationDataPtr= fopen("locationData.txt", "a");  //save data
//add a file to save GPS data done? ^^
void trackGPS();
void spoofGPS();



#endif
