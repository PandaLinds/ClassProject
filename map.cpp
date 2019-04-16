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




int main()
{
  std::cout<<"I compile"<<std::endl;
}


//destroying class and exiting after ^c
void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;

  //close things if need be
  serverMap.~MAP();
  exit(signum);
}
