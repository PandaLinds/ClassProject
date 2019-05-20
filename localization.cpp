
//Google C++ style guide is used

// This code watches input from a GPS, using a wrapper of libgps
// targeted for C++. When the latitude first is recorded, or 
// moves more than 3 meters the lat/long data will be saved 
// with a time stamp. 
// g++ -Wall $(pkg-config --cflags --libs libgps) localization.cpp -o localize
//   We are allowing C++14 because that is the R-Pi's standard


//look at http://www.infernodevelopment.com/c-log-file-class-forget-debuggers
// for how to log

//converting c++ string into string. Need to remember how to.

#include "localization.hpp" 


LOCATION::LOCATION()
{
  gpsData.latitude = 0.0;
  gpsData.longitude = 0.0;
  gpsData.currentTime = " ";
  assert ((logfile_ptr = fopen("/tmp/gpslog.bin", "w")) >= 0);
}
LOCATION::~LOCATION()
{
  fprintf(locationFilePtr, "GPS no longer tracking location\n");
  assert((fclose(logfile_ptr)) == true);
}

int LOCATION::saveGPSData(double GPSlat, double GPSlong, string time)
{
  //add aserts to prove lat/long not 0 
  gpsData.latitude = GPSlat;
  gpsData.longitude = GPSlong;
  gpsData.currentTime = time;
  // set decimal precision
  std::cout.precision(6);
  std::cout.setf(std::ios::fixed, std::ios::floatfield);
  fprintf(locationFilePtr, "New Location:\n");
  fprintf(locationFilePtr, "  %s, %f, %f\n", gpsData.currentTime, gpsData.latitude, gpsData.longitude);
  //std::cout<<timeStamp<<","<<latitude<<","<<longitude<<endl;  
  //save class to a file
  return 0;
  
}


double LOCATION::gpsComp(double lat, double lon)
{
  //compare lat/longs in meters using the Hervsine method
  double hervsine, useHervsine, distanceMeters, earthRadMeters = 6371;
  
  hervsine =  (pow((sin((gpsData.latitude-lat)/2)),2)+(cos(lat)*cos(gpsData.latitude)*pow((sin((gpsData.longitude-lon)/2)),2)));
  
  useHervsine = 2*atan2(sqrt(hervsine), sqrt(1.0-hervsine));
  
  distanceMeters = earthRadMeters * useHervsine;
  return distanceMeters;
  
}


//global so signal_handler will work and logs are correct
LOCATION gps;



void trackGPS() // this function will be added to .h file when it is created. 
{
  gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);   //finds GPS, see what # the default port is
  
  if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) // ENABLE turns off repording modes, JSON turns on JSON reporting data
  {
    std::cerr << "No GPSD running.\n";
  }
  
  struct gps_data_t gpsd_data;  
  struct gps_data_t *dataPtr = &gpsd_data;
  double comp;
  int fixCount;
  
  for (;;)  // change back while forever
  {
    
    while (((dataPtr= gps_rec.read()) == NULL) ||
             (dataPtr->fix.mode < MODE_2D)) 
    {
        // Do nothing until fix
        if(fixCount == 190,000) //number to space out the printing
        {
          fprintf(locationFilePtr, "Not getting signal, stuck in loop trying to fix...\n");
        }
        fixCount++;
    }


      
    timestamp_t ts { dataPtr->fix.time };
    auto newLatitude  { dataPtr->fix.latitude };
    auto newLongitude { dataPtr->fix.longitude };
      
    // convert GPSD's timestamp_t into time_t
    time_t seconds { (time_t)ts };
    auto   tm = *std::localtime(&seconds);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    auto time_str { oss.str() };

    comp = gps.gpsComp(newLatitude, newLongitude);
    
    if (comp >= 3.0) 
    {     
      gps.saveGPSData((double)(newLatitude), (double)(newLongitude), time_str); //fix after comp works
    }
     
    
  }
}

void spoofGPS()
{
  gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);   //finds GPS, see what # the default port is
  
  if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) // ENABLE turns off repording modes, JSON turns on JSON reporting data
  {
    std::cerr << "No GPSD running.\n";
  }
  
  struct gps_data_t gpsd_data;  
  struct gps_data_t *dataPtr = &gpsd_data;
  double comp;
  int fixCount;
  
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
        if(fixCount == 190,000) //number to space out the printing
        {
          fprintf(locationFilePtr, "Not getting signal, stuck in loop trying to fix...\n");
        }
        fixCount++;
    }

    // log the gps binary data
    assert((fwrite(dataPtr, sizeof(struct gps_data_t), 1, logfile_ptr)) == true);
      
    #endif
      
    timestamp_t ts { dataPtr->fix.time };
    auto newLatitude  { dataPtr->fix.latitude };
    auto newLongitude { dataPtr->fix.longitude };
      
    // convert GPSD's timestamp_t into time_t
    time_t seconds { (time_t)ts };
    auto   tm = *std::localtime(&seconds);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    auto time_str { oss.str() };

    comp = gps.gpsComp(newLatitude, newLongitude);
    
    if (comp >= 3.0) 
    {     
      gps.saveGPSData((double)(newLatitude), (double)(newLongitude), time_str); //fix after comp works
    }
     
    
  }
}

int log(void) // have as a seperate thing in the Main?
{
  FILE *logfile_ptr;
  assert ((logfile_ptr = fopen("/tmp/gpslog.bin", "w")) >= 0);
  
  cout<<"I made it this far"<<endl;
  // log the gps binary data
  //  assert((fwrite(dataPtr, sizeof(struct gps_data_t), 1, logfile_ptr)) == true);
  assert((fclose(logfile_ptr)) == true);
}





