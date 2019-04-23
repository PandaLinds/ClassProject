#include "localization.hpp"
#include <stdio.h>
#include <iostream> 

#include <libgpsmm.h> // for GPS

#define SPOOF (1)

//split initialization function?


int main(void)
{
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
//  testGPS();
  cout<<"I made it this far"<<endl;
  
}

/*  struct gps_data_t gpsd_data;  
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
*/
