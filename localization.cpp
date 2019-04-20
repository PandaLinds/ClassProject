
//Google C++ style guide is used

// This code watches input from a GPS, using a wrapper of libgps
// targeted for C++. When the latitude first is recorded, or 
// moves more than 3 meters the lat/long data will be saved 
// with a time stamp. 
// g++ -Wall $(pkg-config --cflags --libs libgps) localization.cpp -o localize
//   We are allowing C++14 because that is the R-Pi's standard


//look at http://www.infernodevelopment.com/c-log-file-class-forget-debuggers
// for how to log


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
#include "localization.hpp" //for location funcitons

#include <libgpsmm.h> // for GPS

using namespace std;


void signalHandler(int);






int main(void)
{
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  assert ((logfile_ptr = fopen("/tmp/gpslog.bin", "w")) >= 0);
  
  GPS();
  cout<<"I made it this far"<<endl;
  
  assert((fclose(logfile_ptr)) == true);
}






// destroy class and exit after ^c

void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;

  //close things if need be
  gps.~LOCATION();
  exit(signum);
}
