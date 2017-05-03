/*
* NAME: idle
* DESCRIPTION: .h file for the idle menu
* AUTHORS: Group A1
*/
#ifndef idle_set
#define idle_set
#include "uxml.h"

int internet_result;
int idleinit;

//xml weahter node
UXML_NODE *node;

extern void idleC_task(void);
extern void show_time(void);
extern void show_icons(void);
extern void show_date(void);
extern void show_weather(void);
extern void get_weather(void);
extern void show_animation(void);
extern void show_rss(int);
#endif
