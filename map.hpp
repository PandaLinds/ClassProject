// Class and function definitions for map.cpp


// add logging to map, printing, compID, and TEST!!

#include <iostream>
#include <string>

using namespace std;  //needed for use of string

struct NODE
{
  struct MONITOR_DATA
  {
    double latitude;
    double longitude;
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
  
    MAP()
    {
      head = NULL;
      tail = NULL;
    }
    
    ~MAP()
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
    
    void addNode (double newLat, double newLong, string newID);
    int checkID(string ID);
    void printMap();
};

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
