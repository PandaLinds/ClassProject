// this module will take the existing locations
//  with the an ID of the owner of the location
//  and create a map of the connections and their
//  location



#include <iostream>
#include <string>
#include <csignal>  // for ^c handler
#include "map.hpp"  //for the use of map

using namespace std;  //needed for use of string

void signalHandler(int signum);

int main()
{
	MAP testMap;
  double testLat, testLong;
  string testID;
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  for (int i = 0; i<5; i++)
  {
    cout<<"enter a latitude: "; cin>>testLat;
    cout<<"enter a longitude: "; cin>>testLong;
    cout<<"enter an ID: "; cin>>testID;
    testMap.addNode(testLat, testLong, testID);
    testMap.printMap();
  }
  
}



//destroying class and exiting after ^c
void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;
  exit(signum);
}
