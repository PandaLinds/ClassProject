/*
 * This is the module for the client.
 */

#include "inet_client.hpp"





//int main(int argc,char** argv)
int client()
{
  char c;
  FILE *fp;
  int i, j, len, num_sets; //client_sock,
  struct hostent *hp;
  struct sockaddr_in client_sockaddr;
  struct linger opt;
  int sockarg;
  ssize_t n_read;
  char incomingBuff[INCOMING_MESSAGE_MAX];
  FILE *fp2 = fopen("data.txt", "a"); //write results to here?
  //signal(SIGINT, sigHandler); //Need?
  //signal(SIGPIPE, broken_pipe_handler); //Need? 

  //if (argc < 2)
  //{
    //printf("Usage: inet_client <server hostname>\n");
    //exit(-1);
  //}

  //if ((hp = gethostbyname(argv[1])) == NULL)    
  //{
    //fprintf(stderr, "Error: %s unknown host.\n", argv[1]);
    //exit(-1);
  //}

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

  //signal(SIGPIPE, broken_pipe_handler);

  fp = fdopen(client_sock, "r");
  
  num_sets = 1;
  send(client_sock, (char *)&num_sets, sizeof(int), 0);
  for(;;)
  {
    send(client_sock, strs, strlen(strs),0);
    memset(incomingBuff,0,sizeof(incomingBuff));
    for (j = 0; j < num_sets; j++)
    {
      //write message to a file. Make a second file for binary file?
      n_read = read(client_sock, &incomingBuff, sizeof(incomingBuff));
      if(n_read >0)
      {
        fprintf(fp2, incomingBuff);
        printf("Client saved message\n");
      }
  /* Listen for drones and alert the server when one is heard 
  while (1) { }*/

    }
    if(strncmp("exit", incomingBuff, 4) == 0)
    {
      break;
    }
    sleep(SECONDS_TO_WAIT);
  }
  close(client_sock);

  exit(0);

}

void sigHandler()
{
  printf("Closing client and socket\n");
  send(client_sock, lastStr, strlen(lastStr), 0);
  close(client_sock);
  exit(0);
}

void broken_pipe_handler()
{
  printf("\nbroken pipe signal received try reconnecting\n");
  //add reconnecting code
  exit(0);
}
