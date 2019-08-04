#include <ctype.h>      /* For tolower()                 */
#include <netdb.h>      /* For gethostbyname()           */
#include <netinet/in.h> /* For server_sockaddr           */
#include <signal.h>     /* For sigHandler()              */
#include <stdio.h>      /* For printf() and scanf()      */
#include <stdlib.h>     /* For NULL                      */
#include <string.h>     /* For strlen()                  */
#include <strings.h>    /* For bzero()                   */
#include <syslog.h>     /* For syslog()                  */
#include <sys/select.h> /* for select                    */
#include <sys/time.h>   /* for select                    */
#include <sys/socket.h> /* For socket()                  */
#include <sys/types.h>  /* For socket()                  */
#include <unistd.h>     /* For gethostname() and sleep() */
#include <csignal>      /* For using signal()            */

#define DEFAULT_PORT 54321
#define SECONDS_TO_WAIT (5)
#define MAX_CHAR (80)

static char* welcomeStr = "Welcome to the Wireless Sensor Network\n";
static char* testStr = "Message from Server\n";
static char* lastStr = "exit\n";

extern int errno;
extern void sigHandler(int);
extern void brokenPipeHandler(int);
extern void serveClients();
void viewSysLogs();
 
static fd_set master, masterCopy;
static int server_sock;
static int i, j, numSets;
static uint fromlen;
static char c;
static FILE *fp;
static FILE *fp2;
static struct sockaddr_in server_sockaddr, client_sockaddr;

