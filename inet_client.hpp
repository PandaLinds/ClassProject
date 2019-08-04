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
#define SECONDS_TO_WAIT (5)
#define MAX_CHAR (80)
#define INCOMING_MESSAGE_MAX 75

static char message[120];
static char *strs = "From client 172.19.35.120\n";
static char *lastStr = "exit\n";
static int client_sock;

	
extern int errno;
extern void broken_pipe_handler();
extern void sigHandler();
