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
#include <csignal>  // for ^c handler

#include <libgpsmm.h> // for GPS

using namespace std; //won't compile stings without this

//add class to .h file when created to match style guide.
class LOCATION
{
  private:
  //the lat/long data is in DBUS_TYPE_DOUBLE but will not compile.
  // will convert when necessary.
  double latitude, longitude;
  string timeStamp;
  
  public:
  //funcitons LOCATION
  int saveGPSData(double, double, string);
  double gpsComp(double,double);
  LOCATION()
  {
    latitude = 0.0;
    longitude = 0.0;
    timeStamp = " ";
    cout<<"made the class"<<endl;
  }
  ~LOCATION()
  {
    cout<<"Program is finished"<<endl;
  }
  
};


int LOCATION::saveGPSData(double GPSlat, double GPSlong, string time)
{
  //add aserts to prove lat/long not 0 
  latitude = GPSlat;
  longitude = GPSlong;
  timeStamp = time;
  // set decimal precision
  std::cout.precision(6);
  std::cout.setf(std::ios::fixed, std::ios::floatfield);
  std::cout<<timeStamp<<","<<latitude<<","<<longitude<<endl;
  
  //save class to a file
  return 0;
  
}


double LOCATION::gpsComp(double lat, double lon)
{
  //compare lat/longs in meters using the Hervsine method
  double hervsine, useHervsine, distanceMeters, earthRadMeters = 6371;
  
  hervsine =  (pow((sin((latitude-lat)/2)),2)+(cos(lat)*cos(latitude)*pow((sin((longitude-lon)/2)),2)));
  
  useHervsine = 2*atan2(sqrt(hervsine), sqrt(1.0-hervsine));
  
  distanceMeters = earthRadMeters * useHervsine;
  return distanceMeters;
  
}



void GPS(); //to be defined by user
