#include <sys/mutex.h>

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_
typedef struct State
{
	char name[16];
    struct State* next;
	struct State* prev;
	struct State* down;
	struct State* up;
	struct State* home;
	void (*task)(void);
} State;

State* currentNode;

int busy;

int go_idle;

int mayCheck;

MUTEX mutex;

void build_menu_structure(void);
void next_state(State**);
void prev_state(State**);
void up_state(State**);
void down_state(State**);
void home_state(State**);
void navigate_state(State **, int);
void navigate_alarm_state(void);

#endif 
