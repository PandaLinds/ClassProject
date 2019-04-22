#include "localization.hpp"
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

#define SPOOF (1)

//split initialization function?
void testGPS() // this function will be added to .h file when it is created. 
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
      
    cout << "Calling GPS comp" << endl;
    comp = gps.gpsComp(newLatitude, newLongitude);
    cout<<"Distance: "<<comp<<endl;
    if (comp >= 3.0) 
    {     
      cout<<"saving"<<endl;
      gps.saveGPSData((double)(newLatitude), (double)(newLongitude), time_str); //fix after comp works
    }

// save to file to compare first?      
    
  }
}




int test(void)
{
  testGPS();
}


