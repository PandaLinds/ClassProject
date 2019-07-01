/*
 * This module contains the code for the communication manager.
 */


#include <assert.h>     // for assert delete
#include <arpa/inet.h>  // for inet_aton() Delete?

#include "inet_server.h"

#define INCOMING_BUFF_MAX 200


int main(int argc, char **argv)
{
  char hostname[64];
  const char *ipstr = "172.19.35.120";
  struct in_addr ip;
  struct hostent *hp;
  struct linger opt;
  int sockarg;
  

  openlog("R-PI-Server", LOG_CONS | LOG_PID, LOG_USER);
  /* Log to myLog.txt instead of /var/log/syslog */
  system("cat /var/log/syslog | grep R-PI-Server > myLog.txt");
  syslog(LOG_NOTICE, "%s", "Starting log...\n");  

  if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Server: socket");
    exit(-1);
  }

  bzero((char*) &server_sockaddr, sizeof(server_sockaddr));
  server_sockaddr.sin_family = AF_INET;
  /* Connect to the default port if none is given, otherwise use the value passed in */
  if (argc < 2)
  {
    server_sockaddr.sin_port = htons(DEFAULT_PORT);
  }
  else
  {
    server_sockaddr.sin_port = htons((int)argv[1]);
  }

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
  signal(SIGINT, sigHandler);
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
  char buff[MAX_CHAR];
  int returnCode, arrayElem = 0;
  char incomingBuff[INCOMING_BUFF_MAX];
  ssize_t n_read;
  fprintf(fp2, "Initialization successful, beginning to serve clients...\n");
  
  
  //from here
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
    syslog(LOG_NOTICE, "%s", "Accepted new client connection.\n");
    //to here, put in different function, then make new process serve 
    
    recv(client_sock, (char *)&numSets, sizeof(int), 0);
    printf("number of sets = %d\n", numSets);
    for(;;)
    {
      arrayElem = 0;
      /* Send test string to the client */
      returnCode = send(client_sock, testStr, strlen(testStr), 0);
      syslog(LOG_NOTICE, "%s", "Sent test string to client.\n");
      for (j = 0; j < numSets; j++)
      { 
        ///* Read client strings and print them out */
        //while((c = fgetc(fp)) != EOF ) 
        //{
          
          //if (numSets < 4)
          //{
            //putchar(c);
            //buff[arrayElem++] = c;
          //}
          //if (c == '\n') 
          //{
            //break;
          //}
        //} /* end while */
        //write message to a file. Make a second file for binary file?
        n_read = read(client_sock, &incomingBuff, sizeof(incomingBuff));
        printf("----n_read is %zu \n", (size_t) n_read);
        if(n_read >0)
        {
          fprintf(fp2, incomingBuff);
          memset(incomingBuff,0,sizeof(incomingBuff));
        }
        syslog(LOG_NOTICE, "%s", "Received message from client."); 
      } /* end for numSets */
      sleep(SECONDS_TO_WAIT);
      if(strncmp("exit", buff, 4) == 0)
      {
        break;
      }
    }
  
    send(client_sock, lastStr, strlen(lastStr), 0);
    close(client_sock);
    syslog(LOG_NOTICE, "%s", "Closed client sock...\n");
      

  } //end forever

  fclose(fp2);
}


/* Close sockets after a Ctrl-C signal */
void sigHandler()
{
  char ch;
  syslog(LOG_NOTICE, "%s", "Ctrl-C interrupt\n");

  printf("\nEnter y to close sockets or n to keep open: ");
  scanf(" %c", &ch);
  ch = tolower(ch);
  if (ch == 'y')
  {
    printf("\nSockets are being closed\n");
    send(client_sock, lastStr, strlen(lastStr), 0);
    close(client_sock);
    syslog(LOG_NOTICE, "%s", "Sockets closed...\n");
    printf("Would you like to shut down the server?\n");
    scanf(" %c", &ch);
    ch = tolower(ch);
    if (ch == 'y')
    {
      close(server_sock);
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

/* Handle broken connections with a client */
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

// Display log file data
void viewSysLogs()
{
  FILE *fp3 = fopen("myLog.txt", "r");

  fclose(fp3);


}
