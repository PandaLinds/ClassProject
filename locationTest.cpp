#include "localization.hpp"
#include <stdio.h>
#include <iostream> 
#include <csignal>  // for ^c handler

#include <libgpsmm.h> // for GPS


void signalHandler(int signum);

int main(void)
{
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  //comment out the function not needed for the test
  trackGPS();
  //spoofGPS();
  
}



// destroy class and exit after ^c

void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;
  exit(signum);
}
