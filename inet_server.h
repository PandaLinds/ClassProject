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

#define DEFAULT_PORT 54321
#define SECONDS_TO_WAIT (5)
#define MAX_CHAR (80)

char* welcomeStr = "Welcome to the Wireless Sensor Network\n";
char* testStr = "Message from Server\n";
char* lastStr = "exit\n";

extern int errno;
extern void sigHandler();
extern void brokenPipeHandler();
extern void serveClients();
void viewSysLogs();

static fd_set master, masterCopy;
static int server_sock;
static int fromlen, i, j, numSets;
static char c;
static FILE *fp;
static struct sockaddr_in server_sockaddr, client_sockaddr;
static FILE *fp2;
