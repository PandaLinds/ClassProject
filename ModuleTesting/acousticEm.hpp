#ifndef ACOUSTICEM_HPP
#define ACOUSTICEM_HPP

#include <iostream>
#include <syslog.h>

static FILE *acousticFilePtr= fopen("monitorLog.txt", "a");

//function not able to be reached by monitor
void emulateDetection();

#endif
