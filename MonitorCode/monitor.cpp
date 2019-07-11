#include <iostream>
#include <string>
#include <csignal>  // for ^c handler
#include <random>
#include "capture.hpp"
#include "localization.hpp"
#include <pthread.h>
#include <unistd.h>



using namespace std;  //needed for use of string

void signalHandler(int signum);
void updateMap();

void* GPSthread(void*)
{
  trackGPS();
}

void* motionthread(void*)
{
  detect();
}

int main()
{
	double testLat, testLong;
  string testID;
  pthread_t GPS, motion;
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  
  pthread_create(&GPS, NULL, &GPSthread, NULL);
  pthread_create(&GPS, NULL, &motionthread, NULL);
  
  pthread_join(GPS, NULL);
  pthread_join(motion,NULL);
  fclose(locationFilePtr);
  fclose(motionFilePtr);
  
  
  
  
}

void updateMap ()
{
  // when GPS saved to file, pull out and update;
}

//destroying class and exiting after ^c
void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;
  fclose(locationFilePtr);
  fclose(motionFilePtr);
  exit(signum);
}
