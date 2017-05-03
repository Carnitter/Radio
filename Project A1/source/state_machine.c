#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mutex.h>
#include "main_menu.h"
#include "state_machine.h"
#include "alarmmenu.h"
#include "daily_alarm.h"
#include "stream.h"
#include "week_alarm.h"
#include "rtchandler.h"
#include "log.h"
#include "memorygame.h"

#define LOG_MODULE  LOG_MAIN_MODULE

/*	NAME: state_machine.c
	DESCRIPTION: This c-file manages the different states the menu can have and executes their actions
	AUTHORS: Group A1
 */

//prototypes
void build_menu(State* , void(*)(void), State* ,State* , State* , State*, State*);
void execute_state(State**);

//local variables
State language_selection, no_internet, idle, menu_rss, menu_alarm, menu_stream, alarm, stream, rss, alarm_d, alarm_w, alarm_w_2, alarm_s, stream, stream_play, daily_alarm, alarm_state, memory_game;



 /*
	NAME: build_menu_structure
	DESCRIPTION: Function to create the menu structure
	PARAMS: NONE
	RETURNS: void
 */
void build_menu_structure(void)
{
	currentNode = malloc(sizeof(State));
	
	NutMutexInit(&mutex);
	
    build_menu(&language_selection, &select_language_task, &no_internet, &no_internet, NULL, &idle, NULL);
    strcpy(language_selection.name, "language selection");

    build_menu(&no_internet, &no_internet_task, &language_selection, &language_selection, NULL, NULL, NULL);
    strcpy(no_internet.name, "no internet");

    build_menu(&idle, &idle_task, &menu_rss, &menu_alarm, NULL, &memory_game, NULL);
    strcpy(idle.name, "idle");

    build_menu(&menu_rss, &menu_rss_task, &menu_stream, &idle, NULL, &rss, &idle);
    strcpy(menu_rss.name, "menu rss");

    build_menu(&menu_alarm, &menu_alarm_task, &idle, &menu_stream, NULL, &alarm, &idle);
    strcpy(menu_alarm.name, "menu alarm");

    build_menu(&menu_stream, &menu_stream_task, &menu_alarm, &menu_rss, NULL, &stream, &idle);
    strcpy(menu_stream.name, "menu stream");

    build_menu(&alarm, &alarm_task, &daily_alarm, &alarm_s, &menu_alarm, &alarm_w, &idle);
    strcpy(alarm.name, "alarm");

    build_menu(&stream, &stream_task, NULL, NULL, &menu_stream, &stream_play, &idle);
    strcpy(stream.name, "stream");

    build_menu(&stream_play, &stream_play_task, NULL, NULL, &stream, NULL, &idle);
    strcpy(stream_play.name, "stream play");

    build_menu(&rss, &rss_task, NULL, NULL, &menu_rss, NULL, &idle);
    strcpy(rss.name, "rss");

    build_menu(&alarm_s, &spec_alarm_task, NULL, NULL, &alarm, NULL, &idle);
    strcpy(alarm_s.name, "alarm s");

    build_menu(&alarm_w, &week_alarm_task, NULL, NULL, &alarm, &alarm_w_2, &idle);
    strcpy(alarm_w.name, "alarm w");

    build_menu(&alarm_w_2, &week2_alarm_task, NULL, NULL, &alarm_w, NULL, &idle);
    strcpy(alarm_w_2.name, "alarm w 2");
	
	build_menu(&daily_alarm, &daily_alarm_task, NULL, NULL, &alarm, NULL, &idle);
	strcpy(daily_alarm.name, "daily");
	
	build_menu(&alarm_state, &alarm_state_task, &idle, NULL, NULL, NULL, NULL);
	strcpy(alarm_state.name, "alarm");
	
	build_menu(&memory_game, &memory_game_task, &idle, &idle, &idle, &idle, &idle);
	strcpy(alarm_state.name, "memorygame");
	
	
	currentNode = &language_selection;
	execute_state(&currentNode);


}

 /*
	NAME: build_menu
	DESCRIPTION: Function to create a menu with a function pointer to execute
	PARAMS: current -> pointer to current state, function -> pointer to function, prev-next-up-down -> pointers to structs that can be navigated to, name -> string for name of state
	RETURNS: void
 */
void build_menu(State* current, void(*function)(void), State* prevNode,State* nextNode, State* upNode, State* downNode, State* homeNode)
{
	current->next = nextNode;
	current->prev = prevNode;
	current->up = upNode;
	current->down = downNode;
	current->task = function;
	current->home = homeNode;
}

 /*
	NAME: next_state
	DESCRIPTION: Function to go the menu next of the current menu.
	PARAMS: pointer to the pointer of the current node
	RETURNS: void
 */
void next_state(State **current_node)
{
    if((*current_node)->next != NULL)
	{
        (*current_node) = (*current_node)->next;
		execute_state(&currentNode);
	}
}

 /*
	NAME: prev_state
	DESCRIPTION: Function to go to the menu before the current menu
	PARAMS: pointer to the pointer of the current node
	RETURNS: void
 */
void prev_state(State **current_node)
{
    if((*current_node)->prev != NULL)
	{
	(*current_node) = (*current_node)->prev;
	execute_state(&currentNode);
	}
}

 /*
	NAME: down_state
	DESCRIPTION: Function to go to the menu under the current menu
	PARAMS: pointer to the pointer of the current node
	RETURNS: void
 */
void down_state(State **current_node)
{
    if((*current_node)->down != NULL)
	{
        (*current_node) = (*current_node)->down;
		execute_state(&currentNode);
	}
}


 /*
	NAME: up_state
	DESCRIPTION: Function to go to the menu above the current menu
	PARAMS: pointer to the pointer of the current node
	RETURNS: void
 */
void up_state(State **current_node)
{
    if((*current_node)->up != NULL)
	{
	(*current_node) = (*current_node)->up;
	execute_state(&currentNode);
	}
}

 /*
	NAME: execute_state
	DESCRIPTION: Function to execute the action in the current state
	PARAMS: pointer to the pointer of the current node
	RETURNS: void
 */
void execute_state(State **current_node)
{

    if((*current_node)->task != NULL)
        (*current_node)->task();
	
	
}

 /*
	NAME: home_state
	DESCRIPTION: Function to go to the home menu (idle menu)
	PARAMS: pointer to the pointer of the current node
	RETURNS: void
 */

void home_state(State **current_node)
{
	if((*current_node)->home != NULL)
	{
	(*current_node) = (*current_node)->home;
	execute_state(&currentNode);
	}
}

/*
	NAME: navigate_state
	DESCRIPTION: Function to go the menu next of the current menu.
	PARAMS: pointer to the pointer of the current node, int steps to stream
	RETURNS: void
 */
void navigate_state(State **current_node, int steps)
{
	int step = 0;
	
	while(step != steps)
	{
		if((*current_node)->next != NULL)
		{
			(*current_node) = (*current_node)->next;
		}
		
		step++;
	}
	
	
	if((*current_node)->down != NULL)
	{
		(*current_node) = (*current_node)->down;
		execute_state(&currentNode);
	}	
}


/*
	NAME: navigate_alarm_state
	DESCRIPTION: current state -> alarm_state
	PARAMS: void
	RETURNS: void
 */
void navigate_alarm_state()
{
	LogMsg_P(LOG_INFO, PSTR("ALARM GOING OFF MOFO"));
	State* alarm = &alarm_state;

	execute_state(&alarm);
	LogMsg_P(LOG_INFO, PSTR("DON"));
}
