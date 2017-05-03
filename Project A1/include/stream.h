#ifndef stream_define
#define stream_define

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <sys/thread.h>
#include <sys/heap.h>

#include "system.h"
#include "portio.h"
#include "display.h"
#include "displayhandler.h"
#include "keyboard.h"
#include "keyboardhandler.h"
#include "log.h"
#include "alarmmenu.h"
#include "main_menu.h"
#include "state_machine.h"
#include "customcharhandler.h"

//volatile int threadStop;

extern int playing;
extern void stream_task(void);
extern void stream_play_task(void);
void set_station(int);
void stop_stream(void);
void play(void);
#endif
