
//Google C++ style guide is used

// This code watches input from a GPS, using a wrapper of libgps
// targeted for C++. When the latitude first is recorded, or 
// moves more than 3 meters the lat/long data will be saved 
// with a time stamp. 
// g++ -Wall $(pkg-config --cflags --libs libgps) localization.cpp -o localize
//   We are allowing C++14 because that is the R-Pi's standard


#include "localization.hpp" 
#define SPOOF (1)
#define SECONDS_TO_WAIT (7)
#define TOLERANCE (3.0)


gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);  //GPS class initializer
struct gps_data_t gpsd_data;  
struct gps_data_t *dataPtr = &gpsd_data;
time_t rawtime;  //for timestamp
struct tm * timeinfo; //for timestamp

LOCATION::LOCATION()   //defining constructor
{
  gpsData.latitude = 0.0;
  gpsData.longitude = 0.0;
  gpsData.currentTime = " ";
  
}
LOCATION::~LOCATION()   //defining destructor
{
  fprintf(locationFilePtr, "GPS no longer tracking location\n");
  fclose(locationDataPtr);
}



//defining the save data function
int LOCATION::saveGPSData(double GPSlat, double GPSlong, string time)  
{
  gpsData.latitude = GPSlat;
  gpsData.longitude = GPSlong;
  gpsData.currentTime = time;
  // set decimal precision
  std::cout.precision(6);  //making sure that the floating points are no more than 6 decimal places
  std::cout.setf(std::ios::fixed, std::ios::floatfield);
  fprintf(locationFilePtr, "New Location:  %s, %f, %f\n", gpsData.currentTime.c_str(), gpsData.latitude, gpsData.longitude);
  fprintf(locationDataPtr, "New Location:  %s, %f, %f\n", gpsData.currentTime.c_str(), gpsData.latitude, gpsData.longitude);
  //save structure to a file
  //assert((fwrite(dataToSave, sizeof(struct GPS_DATA), 1, locationDataPtr)) == true); //saving GPS_DATA structure to binary file use in future?
  return 0;
  
}

//add a function that compares the time as well!


//defining data comparison function
double LOCATION::gpsComp(double lat, double lon)
{
  //compare lat/longs in meters using the Hervsine method
  double hervsine, useHervsine, distanceMeters, earthRadMeters = 6371;
  
  hervsine =  (pow((sin((gpsData.latitude-lat)/2)),2)+(cos(lat)*cos(gpsData.latitude)*pow((sin((gpsData.longitude-lon)/2)),2)));
  
  useHervsine = 2*atan2(sqrt(hervsine), sqrt(1.0-hervsine));
  
  distanceMeters = earthRadMeters * useHervsine;
  return distanceMeters;
  
}


int LOCATION::enableGPS()
{
  if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) // ENABLE turns off repording modes, JSON turns on JSON reporting data
  {
    //std::cerr << "No GPSD running.\n";   // !!! change to loging and exit 
    fprintf(locationFilePtr, "Can't get GPS to stream.\n");
    return(BAD);
  }
  return(GOOD);
}

int LOCATION::findSignal()
{
  int count = 0;
  while (((dataPtr= gps_rec.read()) == NULL) ||
             (dataPtr->fix.mode < MODE_2D)) 
    {
      // log and wait 15 seconds until fix
      time(&rawtime);
      timeinfo = localtime(&rawtime);   
      fprintf(locationFilePtr, "Not getting signal at %s  trying again in %d seconds...\n", asctime(timeinfo), SECONDS_TO_WAIT);
      count++;
      if(count > WAIT_TOLERANCE)
      {
        fprintf(locationFilePtr, "Exceded wait tolerance of %d retries. Exiting... /n", WAIT_TOLERANCE);
        return(BAD);
      }
      sleep(SECONDS_TO_WAIT);
    }
    return(GOOD);
}

int LOCATION::checkGPSData()
{
  int comp;
  //get time and location 
  timestamp_t ts { dataPtr->fix.time };
  auto newLatitude  { dataPtr->fix.latitude };
  auto newLongitude { dataPtr->fix.longitude };
    
  // convert GPSD's timestamp_t into time_t
  time_t seconds { (time_t)ts };
  auto   tm = *std::localtime(&seconds);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
  auto time_str { oss.str() };
  
  //add time comparison

  comp = gpsComp(newLatitude, newLongitude);
  
  //compare new location to last to see if out of tolerence 
  if (comp >= TOLERANCE) 
  {     
    saveGPSData((double)(newLatitude), (double)(newLongitude), time_str); //fix after comp works
    return(CHANGE_SAVED);
  }
  else  // this was used to test, delete?
  {
    time(&rawtime);
    timeinfo = localtime(&rawtime);  
    fprintf(locationFilePtr, "Got the same location at %s\n", asctime(timeinfo));
  }
  
  return(GOOD);
}

string LOCATION::currentLoc()
{
  ostringstream CurrentLoc;
  CurrentLoc.precision(6);  //making sure that the floating points are no more than 6 decimal places
  CurrentLoc.setf(std::ios::fixed, std::ios::floatfield);
  CurrentLoc<<"Lat: "<<gpsData.latitude<<", Long: "<<gpsData.longitude<<", time: "<<gpsData.currentTime;
  return CurrentLoc.str();
}






