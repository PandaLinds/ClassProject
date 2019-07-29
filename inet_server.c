/*
 * This module contains the code for the communication manager.
 */


#include <assert.h>     // for assert delete
#include <arpa/inet.h>  // for inet_aton() Delete?

#include "inet_server.h"

#define INCOMING_BUFF_MAX 75



int main(int argc, char **argv)
{
  char hostname[64];
  const char *ipstr = "192.168.0.4";
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
  char incomingBuff[INCOMING_BUFF_MAX];
  ssize_t nBytes;
  int index, index2;
  char inputChar = 'c';
  
  FD_ZERO(&master);
  FD_SET(server_sock, &master);

  /* Listen on the socket */
  if (listen(server_sock, 5) < 0)
  {
    perror("Server: listen");
    exit(-1);
  }
  //enter select here
  
  for(index2 = 0; index2 < 100; index2++)
  {
    masterCopy = master;
    if( select(FD_SETSIZE, &masterCopy, NULL, NULL, NULL) < 0)
    {
      perror("Server: Select");
      exit(-1);
    }
    
    for(index = 0; index < FD_SETSIZE; index++) //connects 2 then other takes over?
    {
      if(FD_ISSET(index, &masterCopy))
      {
        if(index == server_sock)
        {
          int client_sock;
          /* Accept connections */
          if ((client_sock = accept(server_sock, (struct sockaddr *)&client_sockaddr, &fromlen)) < 0) 
          {
            perror("Server: accept");
            exit(-1);
          }
          printf("\n\nNew connection!! %d\n\n", client_sock);
          FD_SET(client_sock, &master);
          send(client_sock, welcomeStr, strlen(welcomeStr),0);
        }
        else
        {
          memset(incomingBuff,0,sizeof(incomingBuff));
          nBytes = read(index, &incomingBuff, sizeof(incomingBuff));
          if(nBytes > 0)
          {
            fprintf(fp2, incomingBuff);
            printf("Server saved message\n");
            //printf("Client says: %s", incomingBuff);  //delete
            if(strncmp("exit", incomingBuff, 4) == 0)
            {
              send(index, lastStr, strlen(lastStr),0);
              printf("Closing socket %d\n", index);
              FD_CLR(index, &master);
              break;
            }
            else
            {
              send(index, testStr, strlen(testStr),0);
            }
          }
          else
          {
            printf("Closing socket %d\n", index);
            FD_CLR(index, &master);
          }
            
        }
      }
    }
  }
  masterCopy = master;
  if( select(FD_SETSIZE, &masterCopy, NULL, NULL, NULL) < 0)
  {
    perror("Server: Select");
    exit(-1);
  }
  for(index = 0; index < FD_SETSIZE; index++)
  {
    if(FD_ISSET(index, &masterCopy))
    {
      close(index);
      FD_CLR(index, &master);
      printf("Closing socket %d\n", index);
    }
  }
  FD_ZERO(&master);
  fclose(fp2);
}


/* Close sockets after a Ctrl-C signal */
void sigHandler()
{
  //printf("in handler\n"); //Delete
  //int index, selectRC;
  //printf("int\n"); //Delete
  //masterCopy = master;
  //printf("masterCopy\n"); //Delete
  //fclose(fp2); //move to bottom
  //printf("fclose\n");  //delete   stopping here for some reason
  //selectRC = select(FD_SETSIZE, &masterCopy, NULL, NULL, NULL);
  //printf("selectRC = %d\n", selectRC);
  //if (selectRC == 1)
  //{
    //close(server_sock);
    //FD_CLR(server_sock, &master);
  //}
  //else if(selectRC < 0)
  //{
    //perror("Server: Select");
    //printf("Select Bad\n");  //delete
    //exit(-1);
  //}
  //else if(selectRC > 0)
  //{
    //for(index = 0; index < FD_SETSIZE; index++)
    //{
      //printf("in for loop\n");  //delete
      //if(FD_ISSET(index, &masterCopy))
      //{
        //close(index);
        //FD_CLR(index, &master);
        //printf("Closing socket %d\n", index);
      //}
    //}
  //}
  //else
  //{
    //printf("nothing in FD_SET");
  //}
  //printf("Select\n");  //delete
  //FD_ZERO(&master);
  //printf("exiting\n");
  
  exit(0);
  
  
  
  
  
  //char ch;
  //syslog(LOG_NOTICE, "%s", "Ctrl-C interrupt\n");

  //printf("\nEnter y to close sockets or n to keep open: ");
  //scanf(" %c", &ch);
  //ch = tolower(ch);
  //if (ch == 'y')
  //{
    //printf("\nSockets are being closed\n");
    //send(client_sock, lastStr, strlen(lastStr), 0);
    //close(client_sock);
    //syslog(LOG_NOTICE, "%s", "Sockets closed...\n");
    //printf("Would you like to shut down the server?\n");
    //scanf(" %c", &ch);
    //ch = tolower(ch);
    //if (ch == 'y')
    //{
      //close(server_sock);
      //syslog(LOG_NOTICE, "%s", "Server shutting down...\n");
      //printf("Shutting down ...\n");
      //exit(0);
    //}

    //else if (ch == 'n')
    //{
      //printf("Would you like to resume serving clients?\n");
      //scanf(" %c", &ch);
      //ch = tolower(ch);
      //if (ch == 'y')
      //{
        //syslog(LOG_NOTICE, "%s", "Resuming serving clients...\n");
        //printf("Resuming...\n");
        //serveClients();
      //}

      //else
      //{
        //printf("Not continuing...\n");
      //}
    //}

    //else
    //{
      //printf("\nInvalid input, not continuing...\n");
    //}
  //}

  //else if (ch == 'n')
  //{
    //syslog(LOG_NOTICE, "%s", "Resuming...\n");
  //}

  //else
  //{
    //syslog(LOG_NOTICE, "%s", "Resuming...\n");
    //printf("\nInvalid input, resuming by default...\n");
  //}
}

/* Handle broken connections with a client */
void brokenPipeHandler()
{
  int index;
  masterCopy = master;
  if( select(FD_SETSIZE, &masterCopy, NULL, NULL, NULL) < 0)
  {
    perror("Server: Select");
    exit(-1);
  }
  for(index = 0; index < FD_SETSIZE; index++)
  {
    if(FD_ISSET(index, &masterCopy))
    {
      close(index);
      FD_CLR(index, &master);
      printf("Closing socket %d\n", index);
    }
  }
  FD_ZERO(&master);
  
  
  
  
  //char ch;
  //syslog(LOG_NOTICE, "%s", "Broken pipe\n");

  //printf("Enter y to continue serving clients or n to halt:");
  //scanf(" %c", &ch);
  //ch = tolower(ch);

  //if (ch == 'y')
  //{
    //syslog(LOG_NOTICE, "%s", "Resuming...\n");
    //printf("\nWill continue serving clients\n");
    //serveClients();
  //}

  //else if (ch == 'n')
  //{
    //syslog(LOG_NOTICE, "%s", "Server shutting down...\n");
    //printf("Shutting down ...\n");
    //exit(0);
  //}

  //else
  //{
    //syslog(LOG_NOTICE, "%s", "Resuming...\n");
    //printf("\nInvalid input, resuming by default...\n");
  //}
}

// Display log file data
void viewSysLogs()
{
  FILE *fp3 = fopen("myLog.txt", "r");

  fclose(fp3);


}
