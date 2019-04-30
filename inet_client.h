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

#define NSTRS 3
#define MAX_IT 1
#define LOCAL_PORT 54321

char *strs[NSTRS] = {
	"This is the first client string.\n",
	"This is the second client string.\n",
	"This is the third client string.\n"
};

extern int errno;
extern void broken_pipe_handler();

