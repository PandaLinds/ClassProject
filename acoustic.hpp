#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <alsa/asoundlib.h>
#include <math.h>
#include <time.h>



#define WRITEAUDIO(buffer, framesize, framecount) { fwrite(buffer, framesize, framecount, _audiofile); fflush(_audiofile); }


// Globals
static bool _shutdown = false;
static bool _poweroff = false;
static FILE* _audiofile;
