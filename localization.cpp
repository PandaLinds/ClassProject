
//Google C++ style guide is used

// This code watches input from a GPS, using a wrapper of libgps
// targeted for C++. When the latitude first is recorded, or 
// moves more than 3 meters the lat/long data will be saved 
// with a time stamp. 

// what modules are being interfaced

// assumes gpsd is running

// process ok? how to start?

// TODO: make seperate running file and this the test driver
//       Makefile work
//       Make Mapping 

// g++ -Wall $(pkg-config --cflags --libs libgps) localization.cpp -o localize
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
#include <assert.h> //for asserts
#include <cmath>  //for finding distances between coordinates

#include <libgpsmm.h> // for GPS

using namespace std;

//add class to .h file when created to match style guide.
class LOCATION
{
  //unblock after comparason working
  //private:
  public:
  //the lat/long data is in DBUS_TYPE_DOUBLE but will not compile.
  // will convert when necessary.
  double latitude, longitude;
  string timeStamp;
  
  //public:
  //funcitons LOCATION
  int saveGPSData(double, double, string);
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


double gpsComp(double lat1, double lat2, double long1, double long2)
{
  //compare lat/longs instead of polar
  double hervsine, useHervsine, distanceMeters, earthRadMeters = 6371;
  
  hervsine =  (pow((sin((lat2-lat1)/2)),2)+(cos(lat1)*cos(lat2)*pow((sin((long2-long1)/2)),2)));
  
  useHervsine = 2*atan2(sqrt(hervsine), sqrt(1.0-hervsine));
  
  distanceMeters = earthRadMeters * useHervsine;
  return distanceMeters;
  
}

#define SPOOF (1)

//split initialization function?
void GPS() // this function will be added to .h file when it is created. 
{
  //TODO:
  //Convert to polar and see if 3 meters off,
  // then save new data
  
  LOCATION instance1;
  
  gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);   //finds GPS, see what # the default port is
  
  assert(gps_rec.stream(WATCH_ENABLE | WATCH_JSON) != NULL);
  
  //if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) // ENABLE turns off repording modes, JSON turns on JSON reporting data
  //{
    //std::cerr << "No GPSD running.\n";
    
  //}

  struct gps_data_t gpsd_data;  
  struct gps_data_t *dataPtr = &gpsd_data;
  double comp;
  
  for (;;)  // change back while forever
  {
    //if (!gps_rec.waiting(1000000)) continue;  //find what 1000000 is and make not magic number
    
    assert(gps_rec.waiting(1000000) == true );
    
    assert((dataPtr = gps_rec.read()) != NULL);
    
    //if ((dataPtr = gps_rec.read()) == NULL) 
    //{
      //std::cerr << "GPSD read error.\n";
    //} 
    //else 
    //{
    
      //while loop is needed
      while (((dataPtr= gps_rec.read()) == NULL) ||
             (dataPtr->fix.mode < MODE_2D)) 
      {
		// uncomment break to continue with fake GSP data
		 break;
        // Do nothing until fix
        cout<<"Stuck trying to fix"<<endl;
      }

      // Set spoof data here
      if(SPOOF)
      {
         dataPtr = &gpsd_data;
         cout << "Spoofing data" << endl;
         dataPtr->fix.mode = MODE_3D;
         dataPtr->fix.time = (timestamp_t) 999999999.11;
         dataPtr->fix.ept = 0.0000000001;
         dataPtr->fix.latitude = 1.0;
         dataPtr->fix.epy = 0.0000000001;
         dataPtr->fix.longitude = 1.0;
         dataPtr->fix.epx = 0.0000000001;
         dataPtr->fix.altitude = 7777777.11;
         dataPtr->fix.epv = 0.0000000001;
         dataPtr->fix.track = 0.0;
         dataPtr->fix.epd = 0.0000000001;
         dataPtr->fix.speed = 1.0;
         dataPtr->fix.eps = 0.0000000001;
         dataPtr->fix.climb = 1.0;
         dataPtr->fix.epc = 0.0000000001;
         cout << "Data spoofed" << endl;
      }

      assert(dataPtr != NULL);
      assert(dataPtr->fix.mode >= MODE_2D);
      // log the gps binary data
      assert((fwrite(dataPtr, sizeof(struct gps_data_t), 1, logfile_ptr)) == true);
      
      
       
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
      
      cout << "Calling GPS comp" << endl;
      comp = gpsComp(instance1.latitude, newLatitude, instance1.longitude, newLongitude);
      cout<<"Distance: "<<comp<<endl;
      if (comp >= 3.0) 
      {     
        instance1.saveGPSData((double)(newLatitude), (double)(newLongitude), time_str); //fix after comp works
      }
      
      
    //}
  }
}




int main(void)
{
  // error check this - TBD
  assert ((logfile_ptr = fopen("/tmp/gpslog.bin", "w")) >= 0);
  
  GPS();
  cout<<"I made it this far"<<endl;
  
  assert((fclose(logfile_ptr)) == true);
}


/*add int handler for exiting loop */
// add asserts for return codes
