// this module will take the existing locations
//  with the an ID of the owner of the location
//  and create a map of the connections and their
//  location


// add logging to map, printing, compID, and TEST!!

#include <iostream>
#include <string>
#include <csignal>  // for ^c handler

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

MAP serverMap;


int main(int argc, char** argv)
{
  std::cout<<"I compile"<<std::endl;
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

//destroying class and exiting after ^c
void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;

  //close things if need be
  serverMap.~MAP();
  exit(signum);
}
