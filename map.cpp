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

MAP::MAP()
{
  head = NULL;
  tail = NULL;
}

MAP::~MAP()
{
  NODE *n = this->head, *current = NULL;
  
  while(n)
  {
    current = n;
    n = n->next;
    free(current);
  }
  
  head = tail = NULL;
}

void MAP::addNode (double newLat, double newLong, string newID)
{
  NODE *tmp = new NODE;
  tmp->monitor.latitude = newLat;
  tmp->monitor.longitude = newLong;
  tmp->monitor.monitorID = newID;
  tmp->next = NULL;
  
  if(head == NULL)
  {
    head = tmp;
    tail = tmp;
  }
  else
  {
    tail->next = tmp;
    tail = tail->next;
  }
}

void MAP::printMap()
{
  NODE *copyMap = this->head;
  if (copyMap == NULL)
  {
    cout<<"Nothing to print"<<endl;
  }
  while(copyMap != NULL)
  {
    cout<<"Latitude: "<<copyMap->monitor.latitude<<", Longitude: "<<copyMap->monitor.latitude<<", Monitor ID: "<<copyMap->monitor.monitorID<<endl;
    copyMap = copyMap->next;
  }
}

//destroying class and exiting after ^c
void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;
  exit(signum);
}
