//Google C++ style guide is used

// This code watches input from a GPS, using a wrapper of libgps
// targeted for C++. When the latitude first is recorded, or 
// moves more than 9 meters the lat/long data will be saved 
// with a time stamp. 

// g++ -Wall -pedantic $(pkg-config --cflags --libs libgps) localization.cpp -o localize
//   We are allowing C++14 because that is the R-Pi's standard

// functions other than main will be converted to .h files 

// this is a test


#include <iostream> 
#include <string> //for time
#include <iomanip> // for GPS I/O
#include <ctime> //for converting time
#include <sstream> //provides string stream classes
#include <unistd.h> //for syslog
#include <syslog.h> //for syslog

#include <libgpsmm.h> // for GPS

using namespace std;


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

void GPS()
{
  //TODO:
  //Convert to polar and see if 9 meters off,
  // then save new data
  
  LOCATION instance1;
  
  gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);
  
  if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) 
  {
    std::cerr << "No GPSD running.\n";
  }

  for (int idx = 0; idx < 10; idx++) 
  {
    if (!gps_rec.waiting(1000000)) continue;

    struct gps_data_t *gpsd_data;  //style guide says put outside of forloop, cannot test currently

    if ((gpsd_data = gps_rec.read()) == NULL) 
    {
      std::cerr << "GPSD read error.\n";
    } 
    else 
    {
      while (((gpsd_data = gps_rec.read()) == NULL) ||
             (gpsd_data->fix.mode < MODE_2D)) 
      {
        // Do nothing until fix
      }
      timestamp_t ts { gpsd_data->fix.time };
      auto latitude2  { gpsd_data->fix.latitude };
      auto longitude2 { gpsd_data->fix.longitude };
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
  GPS();
  cout<<"I made it this far"<<endl;
}
