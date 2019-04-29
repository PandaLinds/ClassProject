#inlcude "acousticEm.hpp"

void emulateDetection() 
{
  //copied from SERVER code
  penlog("R-PI-MONITOR", LOG_CONS | LOG_PID, LOG_USER); /* Log to monitor.txt */ 
  system("cat /var/log/syslog | grep R-PI-MONITOR > monitorLog.txt"); 
  syslog(ACOUSTIC_MONITOR, "%s", "Starting log...\n");  
  syslog(ACOUSTIC_MONITOR, "%s", "I think I heard a drone.\n");
}
