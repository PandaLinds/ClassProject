//random example code from:
// https://stackoverflow.com/questions/18726102/what-difference-between-rand-and-random-functions

#include "acousticEm.hpp" //to test acoustic
#include <iostream>
#include <random> // for random number generator
#include <csignal>  // for ^c handler

#define FOREVER (1)

void signalHandler(int signum);

int main()
{
  signal(SIGINT, signalHandler);
  std::random_device r;
  std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
  std::mt19937 eng{seed};
  
  std::uniform_int_distribution<> dist(1,100);
  
  while(FOREVER)
  {
    if((dist(eng))<50)
    {
      emulateDetection();
    }
  }
}

void signalHandler(int signum)
{
  std::cout<<"Interupt signal \""<<signum<<"\" recieved"<<std::endl;
  exit(signum);
  fclose(acousticFilePtr);
}
