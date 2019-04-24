// this module will take the existing locations
//  with the an ID of the owner of the location
//  and create a map of the connections and their
//  location


// add logging to map, printing, compID, and TEST!!

#include <iostream>
#include <string>
#include <csignal>  // for ^c handler
#include "map.hpp"  //for the use of map

using namespace std;  //needed for use of string


int main()
{
	MAP testMap;
  double testLat, testLong;
  string testID;
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  testMap.printMap();
  cout<<"enter a latitude: "; cin>>testLat;
  cout<<"enter a longitude: "; cin>>testLong;
  cout<<"enter an ID: "; cin>>testID;
  testMap.addNode(testLat, testLong, testID);
  testMap.printMap();
}

