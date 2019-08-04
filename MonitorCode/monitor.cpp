#include <iostream>
#include <string>
#include <csignal>  // for ^c handler
#include <random>
#include "../localization.hpp"
#include "../detectMotion.hpp"
#include "../acoustic.hpp"
#include "monitor.hpp"
//#include "../inet_client.hpp"
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
  
  if (MonitorLocation.enableGPS() < 0)
  {
    fprintf(fp2, "GPS not fount\n");
    return((void)BAD);
  }
  for(int i = 0; i < 5; i++)
  {
    MonitorLocation.findSignal();
    if (MonitorLocation.checkGPSData() == CHANGE_SAVED)
    {
      //send data
      //log data
      messageCont<<"Location Changed: "<<MonitorLocation.currentLoc()<<"\n";
      lstrs = messageCont.str();
      if ((SendRC=send(client_sock, lstrs.c_str(), strlen(lstrs.c_str()),0)) <= 0)
      {
        fprintf(fp2, "GPS Message not sent\n");
      }
    }
  }
}

void motionthread(void)
{
  // Drawing variables for writing to the window
  stringstream drawnStringStream;
  Point textOrg(10, 10);
  
  // Bounding rectangle and contours to visually track target
  Rect boundingR;
  vector<vector<Point> > contours;
  
  // d1 and d2 for calculating the differences
  // result, the result of and operation, calculated on d1 and d2
  // number_of_changes, the amount of changes in the result matrix.
  // color, the color for drawing the rectangle when something has changed.
  Mat prevPrevFrame, prevFrame, currentFrame, capture_frame, resultTracked, display;  
  Mat d1, d2, motion;
  MotionDetectData_t motionDetectData;
  int numberOfSequence = 0;
  unsigned int frameCnt = 0;
  VideoCapture capture_interface;
  int no_capture_cnt=0;
  char userInput;
  string mstrs = "";
  ostringstream toSend;
  int SendRC;
  time_t rawTime;
  struct tm *info, *oldInfo;
  
  time(&rawTime);
  info = localtime(&rawTime);
  oldInfo = info;
  

  // Erode kernel
  Mat kernel_ero = getStructuringElement(MORPH_RECT, Size(2, 2));

#ifdef CAMERA  

  //if(argc > 1)
  //{
      //// Set up camera for specified device
      //capture_interface.open(argv[1]);

      //if (capture_interface.isOpened()) { // check if we succeeded
        //cout << "Successfully opened input stream " << argv[1] << "!" << endl;
      //} else {
        //cout << "Failed to open input stream " << argv[1] << "!" << endl;
        //exit(EXIT_SUCCESS);
      //}
  //}
  //else
  //{
      // Set up camera for default device
      capture_interface.open(DEVICE_ID);

      if (capture_interface.isOpened()) { // check if we succeeded
        cout << "Successfully opened input stream " << DEVICE_ID << "!" << endl;
      } else {
        cout << "Failed to open input stream " << DEVICE_ID << "!" << endl;
        exit(EXIT_SUCCESS);
      }
  //}

#else

  if(argc > 1)
  {
      capture_interface.open(argv[1]);

      if (capture_interface.isOpened()) { // check if we succeeded
        cout << "Successfully opened Device ID " << argv[1] << "!" << endl;
      } else {
        cout << "Failed to open Device ID " << argv[1] << "!" << endl;
        exit(EXIT_SUCCESS);
      }
  }
  else
  {
      printf("Usage: motion_detect <video source>\n"); exit(-1);
  }

#endif

  // Take image, initialize mats, and convert them to gray
  capture_interface >> capture_frame;

  if(capture_frame.empty()) {cout << "First frame error" << endl; exit(-1);};

  prevPrevFrame = Mat::zeros(capture_frame.size(), capture_frame.type());
  prevFrame = Mat::zeros(capture_frame.size(), capture_frame.type());
  currentFrame = Mat::zeros(capture_frame.size(), capture_frame.type());

#ifdef SHOW_DIFF
  display = Mat::zeros(Size(capture_frame.cols * 2, capture_frame.rows * 2), capture_frame.type());
#else
  display = Mat::zeros(Size(capture_frame.cols * 2, capture_frame.rows * 1), capture_frame.type());
#endif
  
  capture_frame.copyTo(prevFrame);
  capture_frame.copyTo(currentFrame);
  
  cvtColor(prevFrame, prevFrame, CV_RGB2GRAY);
  cvtColor(currentFrame, currentFrame, CV_RGB2GRAY);
  
  cout << "Image Capture Resolution: " << currentFrame.cols << "x" << currentFrame.rows << endl;

  // Setup display window	
  namedWindow(WINDOW_NAME, WINDOW_AUTOSIZE | CV_GUI_NORMAL); 
  createTrackbar("Threshold:", WINDOW_NAME, &currentThreshold, MAX_THRESHOLD, NULL);
  createTrackbar("Max Deviation:", WINDOW_NAME, &currentDeviation, MAX_DEVIATION, NULL);
  createTrackbar("Pixels Changed:", WINDOW_NAME, &currentMotionTrigger, currentFrame.cols * currentFrame.rows, NULL);	
  
  cvWaitKey (DELAY_IN_MSEC);

  // All settings have been set, now go in endless frame acquisition loop
  while (running)
  {
    // Take a new image
    capture_interface >> capture_frame;
    if(capture_frame.empty()) 
    {
      no_capture_cnt++;

      if(no_capture_cnt < 10)
        continue;
      else
        break; 
    }

    prevFrame.copyTo(prevPrevFrame);
    currentFrame.copyTo(prevFrame);
    capture_frame.copyTo(currentFrame);
    
    cvtColor(currentFrame, currentFrame, CV_RGB2GRAY);
    
    // Calc differences between the images and do AND-operation threshold image
    absdiff(prevPrevFrame, currentFrame, d1);
    absdiff(prevFrame, currentFrame, d2);
    bitwise_and(d1, d2, motion);
    threshold(motion, motion, currentThreshold, 255, CV_THRESH_BINARY);
    erode(motion, motion, kernel_ero);
    
    motionDetectData = detectMotion(motion(Rect(CAM_WIDTH_OFFSET, 0, capture_frame.cols-(CAM_WIDTH_OFFSET * 2), capture_frame.rows)), currentDeviation, currentMotionTrigger);

    /* 
    * I think it's self-descriptive. We pick 4 different ROIs and copy
    * the frames we need into them. Piece of cake!
    */
    display = Scalar(0, 0, 0);
    
    capture_frame.copyTo(resultTracked);
    if (motionDetectData.isMotion) 
    {
      findContours(motion, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
      for( int i = 0; i < contours.size(); i++ ) 
      {
         boundingR = boundingRect(contours[i]);
         rectangle(resultTracked, boundingR.tl(), boundingR.br(), Scalar(0, 255, 0), 2, CV_AA , 0);
       }
    }
    
#ifdef SHOW_DIFF
    cvtColor(d1, d1, CV_GRAY2RGB);
    d1.copyTo(display(Rect(capture_frame.cols * 0, capture_frame.rows * 0, capture_frame.cols, capture_frame.rows)));
    cvtColor(d2, d2, CV_GRAY2RGB);
    d2.copyTo(display(Rect(capture_frame.cols * 1, capture_frame.rows * 0, capture_frame.cols, capture_frame.rows)));
    cvtColor(motion, motion, CV_GRAY2RGB);
    motion.copyTo(display(Rect(capture_frame.cols * 0, capture_frame.rows * 1, capture_frame.cols, capture_frame.rows)));
    resultTracked.copyTo(display(Rect(capture_frame.cols * 1, capture_frame.rows * 1, capture_frame.cols, capture_frame.rows)));
#else
    cvtColor(motion, motion, CV_GRAY2RGB);
    motion.copyTo(display(Rect(capture_frame.cols * 0, capture_frame.rows * 0, capture_frame.cols, capture_frame.rows)));
    resultTracked.copyTo(display(Rect(capture_frame.cols * 1, capture_frame.rows * 0, capture_frame.cols, capture_frame.rows)));
#endif

    drawnStringStream.str("");
    drawnStringStream << "Mean: " << motionDetectData.mean[0];
    textOrg.x = 10;
    textOrg.y = 20;
    putText(display, drawnStringStream.str(), textOrg, FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar::all(255), 2, 8);
    
    drawnStringStream.str("");
    drawnStringStream << "Deviation: " << motionDetectData.stddev[0];
    textOrg.x = 10;
    textOrg.y = 50;
    putText(display, drawnStringStream.str(), textOrg, FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar::all(255), 2, 8);
    
    drawnStringStream.str("");
    drawnStringStream << "Changes: " << motionDetectData.numberOfChanges;		
    textOrg.x = 10;
    textOrg.y = 80;
    putText(display, drawnStringStream.str(), textOrg, FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar::all(255), 2, 8);

    // save detected frames
    if (motionDetectData.isMotion) 
    {
      //if (numberOfSequence > 0) //saveImg(capture_frame, DETECT_DIRECTORY, EXTENSION, FILE_FORMAT, numberOfSequence, 1);
      saveImg(capture_frame, DETECT_DIRECTORY, EXTENSION, FILE_FORMAT, frameCnt, 1);
      toSend.str("");
      time(&rawTime);
      info = localtime(&rawTime);
      
      oldInfo = info;
      toSend << "Motion Detected at: "<< asctime(info) << "\n";
      mstrs = toSend.str();
      
      if ((SendRC=send(client_sock, mstrs.c_str(), strlen(mstrs.c_str()),0)) <= 0)
      {
        fprintf(fp2, "Motion Detect Message not sent\n");
      }
      numberOfSequence++;
    } 
    else
    {
      numberOfSequence = 0;
    }

    // save every frame to compare detected frames to
    if((frameCnt % COLLECT_EVERY_NTH_FRAME) ==  0)
    {
      saveImg(capture_frame, COLLECT_DIRECTORY, EXTENSION, FILE_FORMAT, frameCnt, 0);
    }
    
    imshow(WINDOW_NAME, display);
    frameCnt++;
    userInput = (char)cvWaitKey(DELAY_IN_MSEC);
    if(userInput == ESC_KEY)
    {
      break;
    }
  }
  
  capture_interface.release();

  destroyAllWindows();
  
  return;
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
  //string hostName = "172.19.172.14"; //King
  string hostName ="192.168.0.4"; //house
  

  fp = fdopen(client_sock, "r");
  for(;;)
  {
    send(client_sock, strs, strlen(strs),0);
    memset(incomingBuff,0,sizeof(incomingBuff));

    //write message to a file. Make a second file for binary file?
    n_read = read(client_sock, &incomingBuff, sizeof(incomingBuff));
    if(n_read >0)
    {
      fprintf(fp2, incomingBuff);
      printf("Client saved message\n");
    }
  /* Listen for drones and alert the server when one is heard 
  while (1) { }*/

    if(strncmp("exit", incomingBuff, 4) == 0)
    {
      break;
    }
    sleep(SECONDS_TO_WAIT);
  }
  close(client_sock);
  
  return((void)GOOD);
}



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
  
  if ((SendRC=send(client_sock, astrs.c_str(), strlen(astrs.c_str()),0)) <= 0)
  {
    fprintf(fp2, "GPS Message not sent\n");
  }

  // Fill history with a baseline so it doesn't take so long to level out
  for(uint32_t i = 0; i < HISTORY_LEN; i++)
  {
    history_circbuf[i] = 80.0;
  }
  
  time(&rawTime);
  info = localtime(&rawTime);

  sprintf(filename, "mnt/data/pcm/recording.%d%d%d%d%d%d.pcm",info->tm_year+1900, info->tm_mon+1, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
  _audiofile = fopen(filename, "wb");

  snd_pcm_t* handle;
  snd_pcm_sframes_t frames;
  static int16_t buffer[bufsize_bytes];

  if(snd_pcm_open(&handle, device, SND_PCM_STREAM_CAPTURE, 0) != 0)
  {
    printf("Failed to open pcm device (mic not connected)... waiting for mic\n");
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
    //WRITELOG("Microphone connected, waiting for gps lock\n");
    fflush(0);
  }

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
  
  clientInit();
  
  thread GPS(GPSthread);
  thread motion(motionthread);
  thread client(clientthread);
  thread acoustic(acousticthread);
  
  client.join();
  motion.join();
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
  //string hostName = "172.19.172.14";  //king
  string hostName ="192.168.0.4"; //house
  
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
  //fclose(motionFilePtr);
  fclose(fp2);
  exit(signum);
}
