// Class and function definitions for map.cpp

#ifndef MAP_HPP
#define MAP_HPP

// add logging to map, printing, compID, and TEST!!

#include <iostream>
#include <string>

using namespace std;  //needed for use of string

struct NODE
{
  struct MONITOR_DATA
  {
    string location;
    string monitorID;
  };
  MONITOR_DATA monitor;
  NODE *next;
};

class MAP
{
  private:
    NODE *head, *tail;
  
  public:
  //constuctors, destructors, and functions
  
  MAP();    
  ~MAP();
  void addNode (string newLoc, string newID);
  int checkID(string ID);
  void printMap();
};

static FILE *mapFilePtr= fopen("comManLog.txt", "a");


#endif
