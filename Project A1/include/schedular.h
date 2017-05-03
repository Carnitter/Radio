#ifndef SCHEDULAR
#define SCHEDULAR

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "normal_alarm.h"
#include "weekly_alarm.h"
#include "special_alarm.h"
#include "structs.h"


enum type { normal, weekly, special};

typedef struct Schedular_Item
{
	Calendar *date;
	int 		 stream;
	enum   type  alarm_type;
	int snoozecount;
	
} Schedular_Item;

typedef struct Node
{
	struct Node* prev;
	struct Node* next;
	struct Schedular_Item* schedular_item;
}Node;

enum type alarmThatIsGoingOff;
Node *current_node_ptr;
Node *alarm_trigger_ptr;

extern void schedular_init(void);
extern void add(Calendar *, int, enum type, int snoozecount);
extern void remove(Schedular_Item *);
extern void check(void);
extern void updateDate(Calendar *, Calendar *);
extern void snooze(void);
extern void stop_alarm(void);
extern void show_backlight_time(void);
extern void dont_show_backlight_time(void);
extern int list_size(void);
extern void remove_all(void);

#endif
