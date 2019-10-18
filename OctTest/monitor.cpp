#include <iostream>
#include <string>
#include <csignal>  // for ^c handler
#include <random>
#include "../localization.hpp"
#include "../acoustic.hpp"
#include "monitor.hpp"
#include <unistd.h>
#include <thread>
#include <time.h>

#define MOTION_WAIT 3




//add time stamp to motion


using namespace std;  //needed for use of string


void signalHandler(int signum);
void updateMap();
int clientInit();

FILE *fp2 = fopen("data.txt", "a"); //write results to here?

void GPSthread(void)
{
  ssize_t SendRC;
  int returnCode;
  LOCATION MonitorLocation;
  string lstrs = "";
  ostringstream messageCont;
  
  // Enable GPS
  if (MonitorLocation.enableGPS() < 0)
  {
    fprintf(fp2, "GPS not found\n");
    return((void)BAD);
  }
	
  // For the life of the monitor, monitor and update GPS position
  for(;;)
  {
    MonitorLocation.findSignal();
	  
    // check if GPS pos has changed, send update to Com Man
    if (MonitorLocation.checkGPSData() == CHANGE_SAVED)
    {
      //send data
      //log data
      messageCont<<"Location Changed: "<<MonitorLocation.currentLoc()<<"\n";
      lstrs = messageCont.str();
      //if ((SendRC=send(client_sock, lstrs.c_str(), strlen(lstrs.c_str()),0)) <= 0)
      //{
        //fprintf(fp2, "GPS Message not sent\n");
      //}
      
    }
  }
}


////possibly change message sending in other functs to a queue and send here
//void clientthread(void) 
//{
  //char c;
  //FILE *fp;
  //int i, j, len, num_sets; //client_sock,
  //struct hostent *hp;
  //struct sockaddr_in client_sockaddr;
  //struct linger opt;
  //int sockarg;
  //ssize_t n_read;
  //char incomingBuff[INCOMING_MESSAGE_MAX];
  ////string hostName = "172.19.172.14"; //King
  //string hostName ="192.168.0.4"; //house
  
  //// open the socket
  //fp = fdopen(client_sock, "r");
	
  ////send/recieve messages to Com Man
  //for(;;)
  //{
    //send(client_sock, strs, strlen(strs),0);
    //memset(incomingBuff,0,sizeof(incomingBuff));

    ////write message to a file. Make a second file for binary file?
    //n_read = read(client_sock, &incomingBuff, sizeof(incomingBuff));
    //if(n_read >0)
    //{
      //fprintf(fp2, incomingBuff);
      //printf("Client saved message\n");
    //}
  ///* Listen for drones and alert the server when one is heard 
  //while (1) { }*/

    //if(strncmp("exit", incomingBuff, 4) == 0)
    //{
      //break;
    //}
    //sleep(SECONDS_TO_WAIT);
  //}
  //close(client_sock);
  
  //return((void)GOOD);
//}



void acousticthread(void)
{
  #define framesize 2 // 16 bits = 2 bytes
  #define bufsize_bytes 9600 // 48k frames/sec -> 4800 frames/100ms -> 9600 bytes/100ms
  static const uint32_t bufsize_frames = bufsize_bytes/framesize;
  static const char* device = "hw:1,0";
  #define HISTORY_LEN 600 // average across 1 minute
  uint8_t history_iter = 0;
  double history_circbuf[HISTORY_LEN];
  uint8_t overavg_count = 0;
  char filename[80];
  time_t rawTime;
  struct tm *info;
  string astrs = "Acoustic is recording\n";
  ssize_t SendRC;
  
  //// Send the mic is recording to Com Man
  //if ((SendRC=send(client_sock, astrs.c_str(), strlen(astrs.c_str()),0)) <= 0)
  //{
    //fprintf(fp2, "Acoustic Message not sent\n");
  //}
  

  // Fill history with a baseline so it doesn't take so long to level out
  for(uint32_t i = 0; i < HISTORY_LEN; i++)
  {
    history_circbuf[i] = 80.0;
  }
  
  time(&rawTime);
  info = localtime(&rawTime);

  sprintf(filename, "mnt/data/pcm/recording.%d,%d,%d,%d:%d:%d.pcm",info->tm_year+1900, info->tm_mon+1, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
  _audiofile = fopen(filename, "wb");
  
  // Record name
  fprintf(fp2, "File started recording at about %d %d %d %d:%d:%d\n", info->tm_year+1900, info->tm_mon+1, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);

  snd_pcm_t* handle;
  snd_pcm_sframes_t frames;
  static int16_t buffer[bufsize_bytes];
  
  // open PCM device (mic)
  if(snd_pcm_open(&handle, device, SND_PCM_STREAM_CAPTURE, 0) != 0)
  {
    printf("Failed to open pcm device (mic not connected)... waiting for mic\n");
    fprintf(fp2, "Failed to open pcm device (mic not connected)... waiting for mic\n");
    //WRITELOG("Failed to open pcm device (mic not connected)... waiting for mic\n");
    fflush(0);
	  
    while(snd_pcm_open(&handle, device, SND_PCM_STREAM_CAPTURE, 0) != 0)
    {
      if(_shutdown)
      {
        //WRITELOG("abort!\n");
        printf("abort!\n");
        fflush(0);
        return;
      }
      sleep(1);
    }
    printf("Found PCM device, continuing\n");
    fprintf(fp2, "Found PCM device, continuing\n");
    //WRITELOG("Microphone connected, waiting for gps lock\n");
    fflush(0);
  }

  //make sure PMC device gets resources needed
  snd_pcm_hw_params_t* params;
  assert(snd_pcm_hw_params_malloc(&params) == 0);
  assert(snd_pcm_hw_params_any(handle, params) == 0);
  assert(snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED) == 0);
  assert(snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE) == 0);
  assert(snd_pcm_hw_params_set_rate(handle, params, 48000, 0) == 0);
  assert(snd_pcm_hw_params_set_channels(handle, params, 1) == 0);
  assert(snd_pcm_hw_params(handle, params) == 0);
  snd_pcm_hw_params_free(params);

  assert(snd_pcm_prepare(handle) == 0);
  assert(snd_pcm_start(handle) == 0);

  //record until shutdown
  while(!_shutdown)
  {
    frames = snd_pcm_readi(handle, buffer, bufsize_frames);
    if((int)frames < 0)
    {
      if(errno == ENODEV)
      {
        //WRITELOG("Microphone disconnected, shutting down\n");
        _poweroff = true;
        _shutdown = true;
        return;
      }
      //WRITELOG("Acoustic readi failed: %s\n", snd_strerror(frames));
      snd_pcm_recover(handle, frames, 0);
      continue;
    }
    else if((frames < (int)bufsize_frames) && !_shutdown)
    {
      //WRITELOG("Acoustic underrun occurred\n");
    }
    
    WRITEAUDIO(buffer, sizeof(buffer[0]), frames);

    uint64_t total = 0;
    uint32_t i = 0;
    for(i = 0; (int)i < frames; i++)
    {
      total += (buffer[i]) * (buffer[i]);
    }

    double rms = sqrt(total*1.0/frames);
    double rollingavg = 0.0f;

    for(i = 0; i < HISTORY_LEN; i++)
    {
      rollingavg += history_circbuf[i];
    }
    rollingavg /= HISTORY_LEN;

    if(rms > rollingavg)
    {
      overavg_count++;
    }
    else
    {
      overavg_count = 0;
    }

    history_circbuf[history_iter++] = rms;
    history_iter %= HISTORY_LEN;

    //WRITELOG("Acoustic RMS:%7.2f Rolling Average:%7.2f overavg_count:%3d\n", rms, rollingavg, overavg_count);
  }

  assert(snd_pcm_close(handle) == 0);

  return;
}



int main()
{
	double testLat, testLong;
  string testID;
  //pthread_t GPS, motion;
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  //clientInit();
  
  thread GPS(GPSthread);
  //thread client(clientthread);
  thread acoustic(acousticthread);
  
  //client.join();
  GPS.join();
  acoustic.join();
  fclose(locationFilePtr);
  //fclose(motionFilePtr);
  fclose(fp2);
  
}

void updateMap ()
{
  // when GPS saved to file, pull out and update;
}




//int clientInit() //This funciton is called first so other functions can send data
//{
  //char c;
  //FILE *fp;
  //int i, j, len, num_sets; //client_sock,
  //struct hostent *hp;
  //struct sockaddr_in client_sockaddr;
  //struct linger opt;
  //int sockarg;
  //ssize_t n_read;
  //char incomingBuff[INCOMING_MESSAGE_MAX];
  ////string hostName = "172.19.172.14";  //king
  //string hostName ="192.168.0.4"; //house
  
  ////signal(SIGINT, sigHandler); //Need?
  ////signal(SIGPIPE, broken_pipe_handler); //Need? 

  ////if (argc < 2)
  ////{
    ////printf("Usage: inet_client <server hostname>\n");
    ////exit(-1);
  ////}
  
  
  ////cout<<"Enter the host IP: "<<endl;
  ////cin>>hostName;
  //if ((hp = gethostbyname(hostName.c_str())) == NULL)    //change to addr?
  //{
    //fprintf(stderr, "Error: %s unknown host.\n", hostName.c_str());
    //exit(-1);
  //}

  //if ((client_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  //{
    //perror("client: socket");
    //exit(-1);
  //}

  //client_sockaddr.sin_family = AF_INET;
  //client_sockaddr.sin_port = htons(LOCAL_PORT);
  //bcopy(hp->h_addr, &client_sockaddr.sin_addr, hp->h_length);

  ///* discard undelivered data on closed socket */ 
  //opt.l_onoff = 1;
  //opt.l_linger = 0;

  //sockarg = 1;

  //setsockopt(client_sock, SOL_SOCKET, SO_LINGER, (char*) &opt, sizeof(opt));

  //setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&sockarg, sizeof(int));

  //while (connect(client_sock, (struct sockaddr*)&client_sockaddr,
     //sizeof(client_sockaddr)) < 0) 
  //{
    //printf("Unable to connect to server... retrying in 5 seconds...\n");
    //sleep(5);
  //}

  ////signal(SIGPIPE, broken_pipe_handler);
  //return(GOOD);
//}






//destroying class and exiting after ^c
void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;
  fclose(locationFilePtr);
  //fclose(motionFilePtr);
  fclose(fp2);
  exit(signum);
}
