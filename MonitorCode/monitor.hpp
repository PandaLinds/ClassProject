//log class for saving data
#include <iostream>
#include <string>

//stuff for client:
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>


#define LOCAL_PORT 54321
#define SECONDS_TO_WAIT (5)
#define MAX_CHAR (80)
#define INCOMING_MESSAGE_MAX 75

char *strs = "From client 172.19.35.120\n";
char *lastStr = "exit\n";
int client_sock; //for clinetthread


//Class for logging data
class LOG
{
  private:  //Struct is private so the data can't be manipulated. 
  string dataToSend;
    
  public:  //functions that can be used to save and compare data
  //funcitons LOCATION
  int setData();
  int sendData();
  LOG();
  ~LOG();
  
};
