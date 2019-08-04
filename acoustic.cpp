#include "acoustic.hpp"


//int main()
int acoustic()
{
  printf("In main\n"); //delete
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

  printf("After declarations\n"); //delete
  // Fill history with a baseline so it doesn't take so long to level out
  for(uint32_t i = 0; i < HISTORY_LEN; i++)
  {
    history_circbuf[i] = 80.0;
  }
  
  printf("After history thing\n"); //delete
  
  time(&rawTime);
  info = localtime(&rawTime);

  sprintf(filename, "mnt/data/pcm/recording.%d%d%d%d%d%d.pcm",info->tm_year+1900, info->tm_mon+1, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
  printf("%s\n", filename); //delete
  _audiofile = fopen(filename, "wb");

  printf("after fopen\n"); //delete

  snd_pcm_t* handle;
  snd_pcm_sframes_t frames;
  static int16_t buffer[bufsize_bytes];

  printf("After more declarations\n"); //delete

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
        return -1;
      }
      sleep(1);
    }
    printf("Found PCM device, continuing\n");
    //WRITELOG("Microphone connected, waiting for gps lock\n");
    fflush(0);
  }
  
  printf("after snd_pcm_open\n"); //delete

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
  
  printf("after pcm initializations\n"); //delete

  while(!_shutdown)
  {
    printf("in while\n"); //delete
    frames = snd_pcm_readi(handle, buffer, bufsize_frames);
    if((int)frames < 0)
    {
      if(errno == ENODEV)
      {
        //WRITELOG("Microphone disconnected, shutting down\n");
        _poweroff = true;
        _shutdown = true;
        return -2;
      }
      //WRITELOG("Acoustic readi failed: %s\n", snd_strerror(frames));
      snd_pcm_recover(handle, frames, 0);
      continue;
    }
    else if((frames < (int)bufsize_frames) && !_shutdown)
    {
      //WRITELOG("Acoustic underrun occurred\n");
    }
    
    printf("After check frames\n"); //delete
    
    WRITEAUDIO(buffer, sizeof(buffer[0]), frames);
  
    printf("After WRITEAUDIO\n"); //delete

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

  return 0;
}
