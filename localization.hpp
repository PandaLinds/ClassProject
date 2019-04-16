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

using namespace std;

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


//split initialization function?
void GPS() // this function will be added to .h file when it is created. 
{
  
  gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);   //finds GPS, see what # the default port is
  
  if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) // ENABLE turns off repording modes, JSON turns on JSON reporting data
  {
    std::cerr << "No GPSD running.\n";
    
  }

  struct gps_data_t gpsd_data;  
  struct gps_data_t *dataPtr = &gpsd_data;
  double comp;
  
  for (;;)  // change back while forever
  {
    
    while (((dataPtr= gps_rec.read()) == NULL) ||
             (dataPtr->fix.mode < MODE_2D)) 
    {

        // Do nothing until fix
        //cout<<"Stuck trying to fix"<<endl;
    }

    // log the gps binary data
  //  assert((fwrite(dataPtr, sizeof(struct gps_data_t), 1, logfile_ptr)) == true);
      
      
       
    cout << "Time, lat, lon parsing" << endl;
    timestamp_t ts { dataPtr->fix.time };
    auto newLatitude  { dataPtr->fix.latitude };
    auto newLongitude { dataPtr->fix.longitude };
    cout << "Time, lat, lon parsed" << endl;
      
    // convert GPSD's timestamp_t into time_t
    time_t seconds { (time_t)ts };
    auto   tm = *std::localtime(&seconds);

    cout << "Time convert" << endl;

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    auto time_str { oss.str() };
      
//    cout << "Calling GPS comp" << endl;
//    comp = instance1.gpsComp(newLatitude, newLongitude);
//    cout<<"Distance: "<<comp<<endl;
//    if (comp >= 3.0) 
//    {     
//      instance1.saveGPSData((double)(newLatitude), (double)(newLongitude), time_str); //fix after comp works
//    }

// save to file to compare first?      
    
  }
}


