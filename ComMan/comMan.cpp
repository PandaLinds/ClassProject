#include <iostream>
#include <string>
#include <sstream> // for ostream
#include "../map.hpp"
#include "../inet_server.hpp"


#define INCOMING_BUFF_MAX 75

using namespace std;

MAP map;

//select base from https://www.geeksforgeeks.org/tcp-and-udp-server-using-select/

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
  char incomingBuff[INCOMING_BUFF_MAX];
  ssize_t nBytes;
  int index, index2;
  
  FD_ZERO(&master);
  FD_SET(server_sock, &master);

  /* Listen on the socket */
  if (listen(server_sock, 5) < 0)
  {
    perror("Server: listen");
    exit(-1);
  }
  //enter select here
  
  for(;;)
  {
    fp2 = fopen("serverMessages.txt", "a");
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
          fprintf(fp2, "New Client: %d\n", client_sock);
        }
        else
        {
          memset(incomingBuff,0,sizeof(incomingBuff));
          nBytes = read(index, &incomingBuff, sizeof(incomingBuff));
          if(nBytes > 0)
          {
            fprintf(fp2, "Client %d says: %s", index, incomingBuff);
            printf("New Message\n");
            if(strncmp("Location", incomingBuff, 8) == 0)
            {
              string message = incomingBuff;
              size_t stringPos = message.find("Lat");
              string location = message.substr(stringPos);
              std::ostringstream monName;
              monName<<index;
              map.addNode(location, monName.str());
              map.printMap();
              
            }
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
    fclose(fp2);
  }
  struct timeval tv = {5,0};
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
void sigHandler(int sigNum)
{
  int index;
  struct timeval tv = {2,0};
  masterCopy = master;
  if( select(FD_SETSIZE, &masterCopy, NULL, NULL, &tv) < 0)
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
  close(server_sock);
  fclose(fp2);
  map.printMap();
  exit(0);
}

/* Handle broken connections with a client */
void brokenPipeHandler(int sigNum)
{
  int index;
  struct timeval tv = {2,0};
  masterCopy = master;
  if( select(FD_SETSIZE, &masterCopy, NULL, NULL, &tv) < 0)
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
  close(server_sock);
  fclose(fp2);
  exit(0);
}
