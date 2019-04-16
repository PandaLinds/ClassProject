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

MAP serverMap;




int testDriver()
{
  double testLat, testLong;
  string testID;
  cout<<"enter a latitude: "; cin>>testLat;
  cout<<"enter a longitude: "; cin>>testLong;
  cout<<"enter an ID: "; cin>>testID;
  
  //call to test if map works
}


//destroying class and exiting after ^c
void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;

  //close things if need be
  serverMap.~MAP();
  exit(signum);
}
