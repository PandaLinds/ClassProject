//Google C++ style guide is used

// This code watches input from a GPS, using a wrapper of libgps
// targeted for C++. When the latitude first is recorded, or 
// moves more than 3 meters the lat/long data will be saved 
// with a time stamp. 

// what modules are being interfaced

// assumes gpsd is running

// process ok? how to start?

// g++ -Wall -pedantic $(pkg-config --cflags --libs libgps) localization.cpp -o localize
//   We are allowing C++14 because that is the R-Pi's standard

// functions other than main will be converted to .h files 

#include <stdio.h>
FILE *logfile_ptr;

#include <iostream> 
#include <string> //for time
#include <iomanip> // for GPS I/O
#include <ctime> //for converting time
#include <sstream> //provides string stream classes
#include <unistd.h> //for syslog
#include <syslog.h> //for syslog
//may need math.h for polar conversion

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

void GPS() // this function will be added to .h file when it is created. 
{
  //TODO:
  //Convert to polar and see if 9 meters off,
  // then save new data
  
  LOCATION instance1;
  
  gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);   //finds GPS, see what # the default port is
  
  if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) // ENABLE turns off repording modes, JSON turns on JSON reporting data
  {
    std::cerr << "No GPSD running.\n";
    //add error handler, possibly an assert
  }

  for (int idx = 0; idx < 10; idx++) 
  {
    if (!gps_rec.waiting(1000000)) continue;

    struct gps_data_t *gpsd_data;  //style guide says put outside of forloop, will do at later date

    if ((gpsd_data = gps_rec.read()) == NULL) 
    {
      std::cerr << "GPSD read error.\n";
      //add error handler, possibly an assert
    } 
    struct gps_data_t gpsd_data;  //style guide says put outside of forloop, cannot test currently
  struct gps_data_t *dataPtr = &gpsd_data;
  
  for (;;)  // change back while forever
  {
    if (!gps_rec.waiting(1000000)) continue;  //find what 1000000 is and make not magic number
    

    

    if ((dataPtr = gps_rec.read()) == NULL) 
    {
      std::cerr << "GPSD read error.\n";
    } 
    else 
    {
      while (((dataPtr= gps_rec.read()) == NULL) ||
             (dataPtr->fix.mode < MODE_2D)) 
      {
		// uncomment break to continue with fake GSP data
		// break;
        // Do nothing until fix
      }
      
      // log the gps binary data
      fwrite(dataPtr, sizeof(struct gps_data_t), 1, logfile_ptr);
       
      timestamp_t ts { dataPtr->fix.time };
      auto latitude2  { dataPtr->fix.latitude };
      auto longitude2 { dataPtr->fix.longitude };
      cout<<longitude2<<endl;
      
      // convert GPSD's timestamp_t into time_t
      time_t seconds { (time_t)ts };
      auto   tm = *std::localtime(&seconds);

      std::ostringstream oss;
      oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
      auto time_str { oss.str() };
      instance1.saveGPSData((double)latitude2, (double)longitude2, time_str);

      
    }
  }
}




int main(void)
{
  // error check this - TBD
  logfile_ptr = fopen("/tmp/gpslog.bin", "w");
  
  GPS();
  cout<<"I made it this far"<<endl;
  
  fclose(logfile_ptr);
}


//add int handler for exiting loop
// add asserts for return codes
