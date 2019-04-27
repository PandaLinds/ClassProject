/*
 * This module contains the code for the communication manager.
 */

#include <ctype.h> /* For tolower() */
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "inet_server.h"

int main(void)
{
  char hostname[64];
  struct hostent *hp;
  struct linger opt;
  int sockarg;
  int isPrimary = 1;

  openlog("R-PI-Server", LOG_CONS | LOG_PID, LOG_USER);
  /* Log to myLog.txt instead of /var/log/syslog */
  system("cat /var/log/syslog | grep R-PI-Server > myLog.txt");
  syslog(LOG_NOTICE, "%s", "Starting log...\n");

  gethostname(hostname, sizeof(hostname));

  if ((hp = (struct hostent*) gethostbyname(hostname)) == NULL)
  {
    fprintf(stderr, "Error: %s host unknown.\n", hostname);
    exit(-1);
  }

  if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Server: socket");
    exit(-1);
  }

  bzero((char*) &server_sockaddr, sizeof(server_sockaddr));
  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_port = htons(DEFAULT_PORT);
  bcopy (hp->h_addr, &server_sockaddr.sin_addr, hp->h_length);

  /* Bind address to the socket */
  if (bind(server_sock, (struct sockaddr *) &server_sockaddr,
     sizeof(server_sockaddr)) < 0) 
    {
      perror("Server: bind");
      exit(-1);
    }

  /* turn on zero linger time so that undelivered data is discarded when
     socket is closed
   */
  opt.l_onoff = 1;
  opt.l_linger = 0;

  sockarg = 1;
 
  setsockopt(server_sock, SOL_SOCKET, SO_LINGER, (char*) &opt, sizeof(opt));
  setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&sockarg, sizeof(int));
  signal(SIGINT, intHandler);
  signal(SIGPIPE, brokenPipeHandler);

  syslog(LOG_NOTICE, "%s", "Initialization successful, beginning to serve clients...\n");
  serveClients();

  syslog(LOG_NOTICE, "%s", "Server shutting down...\n");
  closelog();

  return 0;
}

/* Listen and accept loop function */
void serveClients()
{
  FILE *fp2 = fopen("data.txt", "a");
  fprintf(fp2, "Initialization successful, beginning to serve clients...\n");

  for (;;)
  {

    /* Listen on the socket */
    if (listen(server_sock, 5) < 0)
    {
      perror("Server: listen");
      exit(-1);
    }

    /* Accept connections */
    if ((client_sock = accept(server_sock, 
        (struct sockaddr *)&client_sockaddr,
                              &fromlen)) < 0) 
    {
      perror("Server: accept");
      exit(-1);
    }
    fp = fdopen(client_sock, "r");

    recv(client_sock, (char *)&num_sets, sizeof(int), 0);
    printf("number of sets = %d\n", num_sets);

    for (j = 0; j < num_sets; j++)
    {

      /* Send strings to the client */
      for (i = 0; i < NSTRS; i++)
      {
        send(client_sock, test_strs[i], strlen(test_strs[i]), 0);
        /* if backup, send map and data files */
      }
      /* Read client strings and print them out */
      for (i = 0; i < NSTRS; i++)
      {
        while((c = fgetc(fp)) != EOF)
        {
	  if (num_sets < 4)
            putchar(c);

          if (c == '\n')
            break;
        } /* end while */


      } /* end for NSTRS */

    } /* end for num_sets */

    close(client_sock);
    syslog(LOG_NOTICE, "%s", "Closed client sock...\n");
    fclose(fp2);

  } /* end for ever */

}


/* Close sockets after a Ctrl-C interrupt */
void intHandler()
{
  char ch;
  syslog(LOG_NOTICE, "%s", "Ctrl-C interrupt\n");

  printf("\nEnter y to close sockets or n to keep open: ");
  scanf(" %c", &ch);
  ch = tolower(ch);
  if (ch == 'y')
  {
    printf("\nSockets are being closed\n");
    close(client_sock);
    close(server_sock);
    syslog(LOG_NOTICE, "%s", "Sockets closed...\n");
    printf("Would you like to shut down the server?\n");
    scanf(" %c", &ch);
    ch = tolower(ch);
    if (ch == 'y')
    {
      syslog(LOG_NOTICE, "%s", "Server shutting down...\n");
      printf("Shutting down ...\n");
      exit(0);
    }

    else if (ch == 'n')
    {
      printf("Would you like to resume serving clients?\n");
      scanf(" %c", &ch);
      ch = tolower(ch);
      if (ch == 'y')
      {
        syslog(LOG_NOTICE, "%s", "Resuming serving clients...\n");
        printf("Resuming...\n");
        serveClients();
      }

      else
      {
        printf("Not continuing...\n");
      }
    }

    else
    {
      printf("\nInvalid input, not continuing...\n");
    }
  }

  else if (ch == 'n')
  {
    syslog(LOG_NOTICE, "%s", "Resuming...\n");
  }

  else
  {
    syslog(LOG_NOTICE, "%s", "Resuming...\n");
    printf("\nInvalid input, resuming by default...\n");
  }
}


void brokenPipeHandler()
{
  char ch;
  syslog(LOG_NOTICE, "%s", "Broken pipe\n");

  printf("Enter y to continue serving clients or n to halt:");
  scanf(" %c", &ch);
  ch = tolower(ch);

  if (ch == 'y')
  {
    syslog(LOG_NOTICE, "%s", "Resuming...\n");
    printf("\nWill continue serving clients\n");
    serveClients();
  }

  else if (ch == 'n')
  {
    syslog(LOG_NOTICE, "%s", "Server shutting down...\n");
    printf("Shutting down ...\n");
    exit(0);
  }

  else
  {
    syslog(LOG_NOTICE, "%s", "Resuming...\n");
    printf("\nInvalid input, resuming by default...\n");
  }
}

void dumpSysLogs()
{
  // Display log file data


}
