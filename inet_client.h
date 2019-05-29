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
#include <signal.h>     /* For sigHandler()              */

#define LOCAL_PORT 54321

//char *strs[2] = {
	//"Connection to client succesful.\n",
	//"34.54000, 112.46850.\n"
//};

char *strs = "Wazzup!\n";

	
extern int errno;
extern void broken_pipe_handler();
extern void sigHandler();
