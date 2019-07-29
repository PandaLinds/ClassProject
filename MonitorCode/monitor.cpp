#include <iostream>
#include <string>
#include <csignal>  // for ^c handler
#include <random>
#include "../capture.hpp"
#include "../localization.hpp"
#include "monitor.hpp"
//#include "../inet_client.hpp"
//#include <pthread.h>
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
  char *lstrs = "Location Changed\n";
  
  if (MonitorLocation.enableGPS() < 0)
  {
    cout<<"GPS not found"<<endl;
    return((void)BAD);
  }
  for(int i = 0; i < 5; i++)
  {
    MonitorLocation.findSignal();
    returnCode = MonitorLocation.checkGPSData();
    if (returnCode == CHANGE_SAVED)
    {
      //send data
      //log data
      if ((SendRC=send(client_sock, lstrs, strlen(lstrs),0)) <= 0)
      {
        cout<<"Message not sent"<<endl;
        if (SendRC == 0)
        {
          cout<<"Server not there"<<endl;
        }
      }
    }
  }
}

void motionthread(void)
{
  //time(&rawtime);
  char *mstrs = "Motion Detected\n";
  ssize_t SendRC;
  
  char snapshotfile[]  = "save1/snapshot00000.jpg";
  char framesavefile[] = "save1/frame00000000.jpg";
  
  char snapshotfile2[]  = "save2/snapshot00000.jpg";
  char framesavefile2[] = "save2/frame00000000.jpg";
      
  IplImage* difframe2;
  IplImage* difframe2_gray;
  IplImage* difframe2_bw;
  IplImage* frame;

  cvNamedWindow("Motion Detect", CV_WINDOW_AUTOSIZE);
  CvCapture* capture2 = cvCreateCameraCapture(0);
  IplImage* frame2;
  IplImage* prevframe2;

  double diffSum=0.0;

  int cnt=0; int savingdata=0;
  int motioncnt=1;
  Mat mat_difframe2_gray;
  
  cvSetCaptureProperty(capture2, CV_CAP_PROP_FRAME_WIDTH, DHRES);
  cvSetCaptureProperty(capture2, CV_CAP_PROP_FRAME_HEIGHT, DVRES);
  
  frame2=cvQueryFrame(capture2);
  prevframe2=cvCreateImage(cvGetSize(frame2), frame2->depth, frame2->nChannels);
  difframe2=cvCreateImage(cvGetSize(frame2), frame2->depth, frame2->nChannels);
  difframe2_bw=cvCreateImage(cvGetSize(frame2),IPL_DEPTH_8U,1);
  
  Mat mat_difframe2 = cvarrToMat(difframe2); //stackoverflow.com/questions/15925084/conversion-from-iplimage-to-cvmat
  Mat mat_difframe2_bw = cvarrToMat(difframe2_bw); //2nd answer(addresses constructor removal)

  cvCopy(frame2, prevframe2);

  cvAbsDiff(frame2, prevframe2, difframe2);
  cvtColor(mat_difframe2, mat_difframe2_gray, CV_RGB2GRAY);
  threshold(mat_difframe2_gray, mat_difframe2_bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

  while(1) //only works if press M first
  {
      
      frame=cvQueryFrame(capture2);
   
      if(!frame || !frame2) break;
      
      cvAbsDiff(frame2, prevframe2, difframe2);
      cvtColor(mat_difframe2, mat_difframe2_gray, CV_RGB2GRAY);
      
      threshold(mat_difframe2_gray, mat_difframe2_bw, 10, 1, CV_THRESH_BINARY);
      diffSum = (sum(mat_difframe2_bw)[0]);

      cvShowImage("Motion Detect", frame);
      
      //printf("diffSum=%lf, motioncnt=%d\n", diffSum, motioncnt);

      char c = cvWaitKey(33);
      if( c == ESC_KEY ) 
      {
        fprintf(motionFilePtr, "Escape\n");
        break;
      }
      if( c == 'p') 
      {
        // on "p" key press
        cnt++;
        
        fprintf(motionFilePtr, "Snapshot, frame %08d saved as %s\n", motioncnt, framesavefile2);
        printf("snapshot %05d taken as %s\n", cnt, snapshotfile2);
        sprintf(&snapshotfile2[8], "%05d.jpg", cnt);
        imwrite(framesavefile2, cvarrToMat(frame2));
      }
      
      if( c == 's') 
      {
        // on "s" key press
        savingdata++;
        
        fprintf(motionFilePtr, "Hard save, frame %08d saved as %s\n", motioncnt, framesavefile2);
        //printf("frame %08d saved as %s\n", savingdata, framesavefile2);
        sprintf(&framesavefile2[8], "%08d.png", savingdata);
        imwrite(framesavefile2, cvarrToMat(frame2));
      }
      
      if((motioncnt && (diffSum > 800)) || (c == 'm') ) 
      {
        // on "m" key press
        motioncnt++;

        fprintf(motionFilePtr, "Motion Detected, frame %08d saved as %s\n", motioncnt, framesavefile2);
        //printf("frame %08d saved as %s\n", motioncnt, framesavefile2);
        sprintf(&framesavefile2[8], "%08d.png", motioncnt);
        imwrite(framesavefile2, cvarrToMat(frame2));
        //send data
        //log data
        if ((SendRC=send(client_sock, mstrs, strlen(mstrs),0)) <= 0)
        {
          cout<<"Message not sent"<<endl;
          if (SendRC == 0)
          {
            cout<<"Server not there"<<endl;
          }
        }
      }
             
      if( c == 'q' && (savingdata || motioncnt)) 
      {
        fprintf(motionFilePtr, "Reset\n");
        savingdata=0;
        motioncnt=1;
      }
      
      cvCopy(frame2, prevframe2);
      sleep(MOTION_WAIT);
  }

  cvReleaseCapture(&capture2);
  cvDestroyWindow("Motion Detect");
  
  fprintf(motionFilePtr, "Camera no longer detecing motion\n");
  fclose(motionDataPtr);
  return((void)0);
}



void clientthread(void)
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
  string hostName = "192.168.0.4";

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
  
  return((void)GOOD);
}


int main()
{
	double testLat, testLong;
  string testID;
  //pthread_t GPS, motion;
  //registering signal SIGINT and signal handler 
  signal(SIGINT, signalHandler);
  
  clientInit();
  
  thread GPS(GPSthread);
  thread motion(motionthread);
  thread client(clientthread);
  
  client.join();
  motion.join();
  GPS.join();
  fclose(locationFilePtr);
  fclose(motionFilePtr);
  fclose(fp2);
  
}

void updateMap ()
{
  // when GPS saved to file, pull out and update;
}




int clientInit()
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
  string hostName = "192.168.0.4";
  
  //signal(SIGINT, sigHandler); //Need?
  //signal(SIGPIPE, broken_pipe_handler); //Need? 

  //if (argc < 2)
  //{
    //printf("Usage: inet_client <server hostname>\n");
    //exit(-1);
  //}
  
  
  //cout<<"Enter the host IP: "<<endl;
  //cin>>hostName;
  if ((hp = gethostbyname(hostName.c_str())) == NULL)    //change to addr?
  {
    fprintf(stderr, "Error: %s unknown host.\n", hostName.c_str());
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

  //signal(SIGPIPE, broken_pipe_handler);
  return(GOOD);
}






//destroying class and exiting after ^c
void signalHandler(int signum)
{
  cout<<"Interupt signal \""<<signum<<"\" recieved"<<endl;
  fclose(locationFilePtr);
  fclose(motionFilePtr);
  fclose(fp2);
  exit(signum);
}
