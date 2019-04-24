#include "localization.hpp"
#include <stdio.h>
#include <iostream> 
#include <csignal>  // for ^c handler

#include <libgpsmm.h> // for GPS

#define SPOOF (1)
LOCATION testGPS;

void spoofGPS() // this function will be added to .h file when it is created. 
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
    #ifdef SPOOF
      int spoofOption;
      float time, ept, lat, epy, lon, epx, alt, epv, track, epd;
      float speed, eps, climb, epc;
      cout<<"Spoof enabled"<<endl;
      cout<<" Press 1 to enter time,Lat,Long"<<endl;
      cout<<"Press 2 to enter all gpsd_data: ";
      cin>>spoofOption;
      if(spoofOption == 1)
      {
        cout<<"Enter the time: "; cin>>time;
        cout<<"Enter the latitude: "; cin>>lat;
        cout<<"Enter the longitute: "; cin>>lon;
        dataPtr = &gpsd_data;
        cout << "Spoofing data" << endl;
        dataPtr->fix.mode = MODE_3D;
        dataPtr->fix.time = (timestamp_t) time;
        dataPtr->fix.ept = 0.0000000001;
        dataPtr->fix.latitude = lat;
        dataPtr->fix.epy = 0.0000000001;
        dataPtr->fix.longitude = lon;
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
      else if (spoofOption == 2)
      {
        cout<<"Enter the time: "; cin>>time;
        cout<<"Enter the ept: "; cin>>ept;
        cout<<"Enter the latitude: "; cin>>lat;
        cout<<"Enter the epy: "; cin>>epy;
        cout<<"Enter the longitute: "; cin>>lon;
        cout<<"Enter the epx: "; cin>>epx;
        cout<<"Enter the altitude: "; cin>>alt;
        cout<<"Enter the epv: "; cin>>epv;
        cout<<"Enter the track: "; cin>>track;
        cout<<"Enter the epd: "; cin>>epd;
        cout<<"Enter the speed: "; cin>>speed;
        cout<<"Enter the eps: "; cin>>eps;
        cout<<"Enter the climb: "; cin>>climb;
        cout<<"Enter the epc: "; cin>>epc;
        dataPtr = &gpsd_data;
        cout << "Spoofing data" << endl;
        dataPtr->fix.mode = MODE_3D;
        dataPtr->fix.time = (timestamp_t) time;
        dataPtr->fix.ept = ept;
        dataPtr->fix.latitude = lat;
        dataPtr->fix.epy = epy;
        dataPtr->fix.longitude = lon;
        dataPtr->fix.epx = epx;
        dataPtr->fix.altitude = alt;
        dataPtr->fix.epv = epv;
        dataPtr->fix.track = track;
        dataPtr->fix.epd = epd;
        dataPtr->fix.speed = speed;
        dataPtr->fix.eps = eps;
        dataPtr->fix.climb = climb;
        dataPtr->fix.epc = epc;
        cout << "Data spoofed" << endl;
      }
      else
      {
        cout<<"Please try again"<<endl;
      }
    #else
    while (((dataPtr= gps_rec.read()) == NULL) ||
             (dataPtr->fix.mode < MODE_2D)) 
    {

        // Do nothing until fix
        //cout<<"Stuck trying to fix"<<endl;
    }

    // log the gps binary data
  //  assert((fwrite(dataPtr, sizeof(struct gps_data_t), 1, logfile_ptr)) == true);
      
    #endif
       
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
    comp = testGPS.gpsComp(newLatitude, newLongitude);
    cout<<"Distance: "<<comp<<endl;
    if (comp >= 3.0) 
    {     
      cout<<"saving"<<endl;
      testGPS.saveGPSData((double)(newLatitude), (double)(newLongitude), time_str); //fix after comp works
    }

// save to file to compare first?      
    
  }
}

//split initialization function?

void signalHandler(int signum);

int main(void)
{
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  spoofGPS();
  cout<<"I made it this far"<<endl;
  
}



// destroy class and exit after ^c

void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;
  exit(signum);
}
