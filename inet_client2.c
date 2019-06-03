/*
 * This is the module for the client.
 */

#include "inet_client.h"



int client_sock;

int main(argc, argv)
int argc;
char **argv;
{
  char c;
  FILE *fp;
  int i, j, len, num_sets; //client_sock,
  struct hostent *hp;
  struct sockaddr_in client_sockaddr;
  struct linger opt;
  int sockarg;
  signal(SIGINT, sigHandler); //Need?
  //signal(SIGPIPE, broken_pipe_handler); //Need? defined later line 72

  if (argc < 2)
  {
    printf("Usage: inet_client <server hostname>\n");
    exit(-1);
  }

  if ((hp = gethostbyname(argv[1])) == NULL)    //change to addr?
  {
    fprintf(stderr, "Error: %s unknown host.\n", argv[1]);
    exit(-1);
  }

  if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("client: socket");
    exit(-1);
  }

  client_sockaddr.sin_family = AF_INET;
  client_sockaddr.sin_port = htons(LOCAL_PORT);
  bcopy(hp->h_addr, &client_sockaddr.sin_addr, hp->h_length);

  /* discard undelivered data on closed socket */ 
  opt.l_onoff = 1;
  opt.l_linger = 0;

  sockarg = 1;

  setsockopt(client_sock, SOL_SOCKET, SO_LINGER, (char*) &opt, sizeof(opt));

  setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&sockarg, sizeof(int));

  while (connect(client_sock, (struct sockaddr*)&client_sockaddr,
     sizeof(client_sockaddr)) < 0) 
  {
    printf("Unable to connect to server... retrying in 5 seconds...\n");
    sleep(5);
  }

  signal(SIGPIPE, broken_pipe_handler);

  fp = fdopen(client_sock, "r");
  
  num_sets = 1;
  for(;;)
  {
    send(client_sock, (char *)&num_sets, sizeof(int), 0);
    for (j = 0; j < num_sets; j++)
    {
      //printf("before the decoding of server message\n"); //delete
      /* Read the server string and print it out */
      while ((c = fgetc(fp)) != EOF)  //it blows up here!! it is not calling broken pipe handler after server closes
      {                         //printf("during the decoding of server message\n"); //delete
          putchar(c);
          
        if (c == '\n')
        {
          break;
        }
      }
      send(client_sock, strs, strlen(strs),0);
  /* Listen for drones and alert the server when one is heard 
  while (1) { }*/

    }
    sleep(SECONDS_TO_WAIT);
  }
  close(client_sock);

  exit(0);

}

void sigHandler()
{
  printf("Closing client and socket\n");
  close(client_sock);
  exit(0);
}

void broken_pipe_handler()
{
  printf("\nbroken pipe signal received try reconnecting\n");
  //add reconnecting code
  exit(0);
}
