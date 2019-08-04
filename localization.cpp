
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
  cout<<"Enabling"<<endl; //delete
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
  cout<<"Finding"<<endl; //delete
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
      }
      sleep(SECONDS_TO_WAIT);
    }
    return(GOOD);
}

int LOCATION::checkGPSData()
{
  cout<<"Checking"<<endl; //delete
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

string LOCATION::currentLoc()  //finish this!
{
  ostringstream CurrentLoc;
  CurrentLoc<<"Lat: "<<gpsData.latitude<<", Long: "<<gpsData.longitude<<", time: "<<gpsData.currentTime;
  return CurrentLoc.str();
}


////this is a duplicate of trackGPS, but adds spoofing
//void spoofGPS()
//{
  //fprintf(locationFilePtr, "I am now spoofing\n");
  //gpsmm gps_rec("localhost", DEFAULT_GPSD_PORT);   //finds GPS, see what # the default port is
  
  //if (gps_rec.stream(WATCH_ENABLE | WATCH_JSON) == NULL) // ENABLE turns off repording modes, JSON turns on JSON reporting data
  //{
    //std::cerr << "No GPSD running.\n";
    //fprintf(locationFilePtr, "Can't get GPS to stream\n");
  //}
  
  //struct gps_data_t gpsd_data;  
  //struct gps_data_t *dataPtr = &gpsd_data;
  //double comp;
  
  //for (;;)  // loop forever
  //{
    //#ifdef SPOOF
      ////start of spoofing
      //int spoofOption;
      //float time, ept, lat, epy, lon, epx, alt, epv, track, epd;
      //float speed, eps, climb, epc;
      //cout<<"Spoof enabled"<<endl;
      //cout<<" Press 1 to enter time,Lat,Long"<<endl;
      //cout<<"Press 2 to enter all gpsd_data: ";
      //cin>>spoofOption;
      //if(spoofOption == 1)
      //{  //hard coded float numbers are not needed for option 1, only there so gpsd works
        //cout<<"Enter the time: "; cin>>time;
        //cout<<"Enter the latitude: "; cin>>lat;
        //cout<<"Enter the longitute: "; cin>>lon;
        //dataPtr = &gpsd_data;
        //cout << "Spoofing data" << endl;
        //dataPtr->fix.mode = MODE_3D;
        //dataPtr->fix.time = (timestamp_t) time;
        //dataPtr->fix.ept = 0.0000000001;
        //dataPtr->fix.latitude = lat;
        //dataPtr->fix.epy = 0.0000000001;
        //dataPtr->fix.longitude = lon;
        //dataPtr->fix.epx = 0.0000000001;
        //dataPtr->fix.altitude = 7777777.11;
        //dataPtr->fix.epv = 0.0000000001;
        //dataPtr->fix.track = 0.0;
        //dataPtr->fix.epd = 0.0000000001;
        //dataPtr->fix.speed = 1.0;
        //dataPtr->fix.eps = 0.0000000001;
        //dataPtr->fix.climb = 1.0;
        //dataPtr->fix.epc = 0.0000000001;
        //cout << "Data spoofed" << endl;
      //}
      //else if (spoofOption == 2)
      //{
        //cout<<"Enter the time: "; cin>>time;
        //cout<<"Enter the ept: "; cin>>ept;
        //cout<<"Enter the latitude: "; cin>>lat;
        //cout<<"Enter the epy: "; cin>>epy;
        //cout<<"Enter the longitute: "; cin>>lon;
        //cout<<"Enter the epx: "; cin>>epx;
        //cout<<"Enter the altitude: "; cin>>alt;
        //cout<<"Enter the epv: "; cin>>epv;
        //cout<<"Enter the track: "; cin>>track;
        //cout<<"Enter the epd: "; cin>>epd;
        //cout<<"Enter the speed: "; cin>>speed;
        //cout<<"Enter the eps: "; cin>>eps;
        //cout<<"Enter the climb: "; cin>>climb;
        //cout<<"Enter the epc: "; cin>>epc;
        //dataPtr = &gpsd_data;
        //cout << "Spoofing data" << endl;
        //dataPtr->fix.mode = MODE_3D;
        //dataPtr->fix.time = (timestamp_t) time;
        //dataPtr->fix.ept = ept;
        //dataPtr->fix.latitude = lat;
        //dataPtr->fix.epy = epy;
        //dataPtr->fix.longitude = lon;
        //dataPtr->fix.epx = epx;
        //dataPtr->fix.altitude = alt;
        //dataPtr->fix.epv = epv;
        //dataPtr->fix.track = track;
        //dataPtr->fix.epd = epd;
        //dataPtr->fix.speed = speed;
        //dataPtr->fix.eps = eps;
        //dataPtr->fix.climb = climb;
        //dataPtr->fix.epc = epc;
        //cout << "Data spoofed" << endl;
      //}
      //else
      //{
        //cout<<"Please try again"<<endl;
      //}
      ////end of spoofing
    //#else
      ////start of regular tracking
      //while (((dataPtr= gps_rec.read()) == NULL) ||
               //(dataPtr->fix.mode < MODE_2D)) 
      //{
        ////log and wait 15 seconds until fix
        //time(&rawtime);
        //timeinfo = localtime(&rawtime);   
        //fprintf(locationFilePtr, "Not getting signal at %s, trying again in %d seconds...\n", asctime(timeinfo), SECONDS_TO_WAIT);
        //sleep(SECONDS_TO_WAIT);
      //}
  
      //// log the gps binary data
      //assert((fwrite(dataPtr, sizeof(struct gps_data_t), 1, logfile_ptr)) == true);
      ////end of regular tracking
    //#endif
    
    ////get time and location from GPS
    //timestamp_t ts { dataPtr->fix.time };
    //auto newLatitude  { dataPtr->fix.latitude };
    //auto newLongitude { dataPtr->fix.longitude };
      
    //// convert GPSD's timestamp_t into time_t
    //time_t seconds { (time_t)ts };
    //auto   tm = *std::localtime(&seconds);

    //std::ostringstream oss;
    //oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    //auto time_str { oss.str() };

    //comp = gps.gpsComp(newLatitude, newLongitude);
    
    ////compare new location to last to see if out of tolerence 
    //if (comp >= TOLERANCE) 
    //{     
      //gps.saveGPSData((double)(newLatitude), (double)(newLongitude), time_str); //fix after comp works
    //}
     
    
  //} // end loop forever
//}


////defining the log function (needs work and not in use yet)
//int log(void) // have as a seperate thing in the Main?
//{
  //FILE *logfile_ptr;
  //assert ((logfile_ptr = fopen("/tmp/gpslog.bin", "w")) >= 0);
  
  //cout<<"I made it this far"<<endl;
  //// log the gps binary data
  ////  assert((fwrite(dataPtr, sizeof(struct gps_data_t), 1, logfile_ptr)) == true);
  //assert((fclose(logfile_ptr)) == true);
  //return 1;
//}





