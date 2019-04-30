
//Google C++ style guide is used

// This code watches input from a GPS, using a wrapper of libgps
// targeted for C++. When the latitude first is recorded, or 
// moves more than 3 meters the lat/long data will be saved 
// with a time stamp. 
// g++ -Wall $(pkg-config --cflags --libs libgps) localization.cpp -o localize
//   We are allowing C++14 because that is the R-Pi's standard


//look at http://www.infernodevelopment.com/c-log-file-class-forget-debuggers
// for how to log

#include "localization.hpp" 

LOCATION::LOCATION()
{
  gpsData.latitude = 0.0;
  gpsData.longitude = 0.0;
  gpsData.CurrentTime = " ";
}
LOCATION::~LOCATION()
{
  fprintf(locationFilePtr, "GPS no longer tracking location\n");
}

int LOCATION::saveGPSData(double GPSlat, double GPSlong, string time)
{
  //add aserts to prove lat/long not 0 
  gpsData.latitude = GPSlat;
  gpsData.longitude = GPSlong;
  gpsData.timeStamp = time;
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

int log(void) // have as a seperate thing in the Main?
{
  FILE *logfile_ptr;
  assert ((logfile_ptr = fopen("/tmp/gpslog.bin", "w")) >= 0);
  
  cout<<"I made it this far"<<endl;
  // log the gps binary data
  //  assert((fwrite(dataPtr, sizeof(struct gps_data_t), 1, logfile_ptr)) == true);
  assert((fclose(logfile_ptr)) == true);
}





