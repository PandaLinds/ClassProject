#ifndef ACOUSTICEM_HPP
#define ACOUSTICEM_HPP

#include <iostream>
#include <syslog.h>
static FILE *acousticFilePtr= fopen("monitorLog.txt", "a");
void emulateDetection();

#endif
