#include "acousticEm.hpp"
#include <iostream>

void emulateDetection() 
{
  //copied from SERVER code
  //openlog("R-PI-MONITOR", LOG_CONS | LOG_PID, LOG_USER); /* Log to monitor.txt */ 
  //system("cat /var/log/syslog | grep R-PI-MONITOR > monitorLog.txt"); 
  //syslog(LOG_NOTICE, "%s", "Starting log...\n");  
  //syslog(LOG_NOTICE, "%s", "Initialization successful, beginning to serve clients...\n");
  fprintf(acousticFilePtr, "\n");
  //fclose(acousticFilePtr); //used in test code for now
}
