#include <iostream>
#include <string>
#include <csignal>  // for ^c handler
#include <pthread.h>
#include "map.hpp"  //for the use of map
#include "localization.hpp"

using namespace std;  //needed for use of string

void signalHandler(int signum);
void updateMap();




void* GPSthread(void*)
{
  trackGPS();
}

int main()
{
	
  double testLat, testLong;
  string testID;
  pthread_t GPS;
  MAP testMap;
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  pthread_create(&GPS, NULL, &GPSthread, NULL);
  
  for (int i = 0; i<5; i++)
  {
    cout<<"enter a latitude: "; cin>>testLat;
    cout<<"enter a longitude: "; cin>>testLong;
    cout<<"enter an ID: "; cin>>testID;
    testMap.addNode(testLat, testLong, testID);
    testMap.printMap();
  }
  pthread_join(GPS, NULL);
}

void updateMap ()
{
  // when GPS saved to file, pull out and update;
}

//destroying class and exiting after ^c
void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;
  exit(signum);
}
