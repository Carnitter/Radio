#ifndef stream_play_define
#define stream_play_define

#include <sys/nutconfig.h>
#include <sys/types.h>

//#include <stdlib.h>
//#include <string.h>
#include <stdio.h>
#include <io.h>

extern int threadStop;

int initPlayerstream(void);
int playstream(FILE *);
int stop_thread(void);
int pausestream(void);
int volumemax(void);
int ProcessMetaData(FILE *);
#endif
