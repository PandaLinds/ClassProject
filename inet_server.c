/*
 * This module contains the code for the server.
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include "inet_server.h"

int main(void)
{
  char hostname[64];
  struct hostent *hp;
  struct linger opt;
  int sockarg;
  int isPrimary = 1;

  gethostname(hostname, sizeof(hostname));

  if((hp = (struct hostent*) gethostbyname(hostname)) == NULL)
  {
    fprintf(stderr, "Error: %s host unknown.\n", hostname);
    exit(-1);
  }

  if((server_sock=socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Server: socket");
    exit(-1);
  }

  bzero((char*) &server_sockaddr, sizeof(server_sockaddr));
  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_port = htons(DEFAULT_PORT);
  bcopy (hp->h_addr, &server_sockaddr.sin_addr, hp->h_length);

  /* Bind address to the socket */
  if(bind(server_sock, (struct sockaddr *) &server_sockaddr,
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

  serveClients();

  return 0;
}

/* Listen and accept loop function */
void serveClients()
{

  for(;;)
  {

    /* Listen on the socket */
    if(listen(server_sock, 5) < 0)
    {
      perror("Server: listen");
      exit(-1);
    }

    /* Accept connections */
    if((client_sock=accept(server_sock, 
                           (struct sockaddr *)&client_sockaddr,
                           &fromlen)) < 0) 
    {
      perror("Server: accept");
      exit(-1);
    }

    fp = fdopen(client_sock, "r");

    recv(client_sock, (char *)&num_sets, sizeof(int), 0);
    printf("number of sets = %d\n", num_sets);

    for(j=0;j<num_sets;j++)
    {

      /* Send strings to the client */
      for (i=0; i<NSTRS; i++)
        send(client_sock, test_strs[i], strlen(test_strs[i]), 0);

      /* Read client strings and print them out */
      for (i=0; i<NSTRS; i++)
      {
        while((c = fgetc(fp)) != EOF)
        {
	  if(num_sets < 4)
            putchar(c);

          if(c=='\n')
            break;
        } /* end while */

      } /* end for NSTRS */

    } /* end for num_sets */

    close(client_sock);

  } /* end for ever */

}


/* Close sockets after a Ctrl-C interrupt */
void intHandler()
{
  char ch;

  printf("Enter y to close sockets or n to keep open:");
  scanf("%c", &ch);

  if(ch == 'y')
  {
    printf("\nsockets are being closed\n");
    close(client_sock);
    close(server_sock);
  }

  printf("Server: Shutting down ...\n");

  exit(0);

}


void brokenPipeHandler()
{
  char ch;

  printf("Enter y to continue serving clients or n to halt:");
  scanf("%c", &ch);

  if(ch == 'y')
  {
    printf("\nwill continue serving clients\n");
    serveClients();
  }

  else
  {
    printf("Server: Shutting down ...\n");
    exit(0);
  }

}

void dumpSysLogs()
{ 
  // Display log files


}