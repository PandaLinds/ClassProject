#include "localization.hpp"
#include <stdio.h>
#include <iostream> 
#include <csignal>  // for ^c handler

#include <libgpsmm.h> // for GPS

#define SPOOF (1)

//split initialization function?

void signalHandler(int signum);

int main(void)
{
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  spoofGPS();
  cout<<"I made it this far"<<endl;
  
}



// destroy class and exit after ^c

void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;
  exit(signum);
}
