#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#define NSTRS 3
#define DEFAULT_PORT 54321
#define PRIMARY 1

char *test_strs[NSTRS] = {
        "This is the first server string.\n",
        "This is the second server string.\n",
        "This is the third server string.\n"
};

extern int errno;
extern void intHandler();
extern void brokenPipeHandler();
extern void serveClients();


static int server_sock, client_sock;
static int fromlen, i, j, num_sets;
static char c;
static FILE *fp;
static struct sockaddr_in server_sockaddr, client_sockaddr;

/* Function prototypes */
void serveClients();
void intHandler();
void brokenPipeHandler();
void dumpSysLogs();
