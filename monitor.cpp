#include <iostream>
#include <string>
#include <csignal>  // for ^c handler
#include <pthread.h>
#include <random>
#include "acousticEm.hpp"
#include "map.hpp"  //for the use of map
#include "localization.hpp"


using namespace std;  //needed for use of string

void signalHandler(int signum);
void updateMap();




void* GPSthread(void*)
{
  trackGPS();
}

void* acousticthread(void*)
{
  signal(SIGINT, signalHandler);
  std::random_device r;
  std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
  std::mt19937 eng{seed};
  
  std::uniform_int_distribution<> dist(1,100);
  
  while(1)
  {
    if(((dist(eng))==1)&&((dist(eng))==1)&&((dist(eng))==1))
    {
      emulateDetection();
    }
  }
}

int main()
{
	
  double testLat, testLong;
  string testID;
  pthread_t GPS, acoustic;
  MAP testMap;
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  
  pthread_create(&GPS, NULL, &GPSthread, NULL);
  pthread_create(&GPS, NULL, &acousticthread, NULL);
  
  for (int i = 0; i<5; i++)
  {
    cout<<"enter a latitude: "; cin>>testLat;
    cout<<"enter a longitude: "; cin>>testLong;
    cout<<"enter an ID: "; cin>>testID;
    testMap.addNode(testLat, testLong, testID);
    testMap.printMap();
  }
  pthread_join(GPS, NULL);
  pthread_join(acoustic,NULL);
  fclose(locationFilePtr);
  fclose(acousticFilePtr);
  fclose(mapFilePtr);
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
  fclose(acousticFilePtr);
  fclose(mapFilePtr);
  exit(signum);
}
