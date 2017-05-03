/*
* NAME: stream
* DESCRIPTION: .c file which has functions to perform a alarm menu item tasks
* AUTHORS: Group A1
*/

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <sys/thread.h>
#include <sys/heap.h>
#include "system.h"
#include <sys/thread.h>
#include <sys/timer.h>

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
#include "list.h"

/*-------------------------------------------------------------------------*/
/* local defines                                                           */
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/* local variable definitions                                              */
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/* local routines (prototyping)                                            */
/*-------------------------------------------------------------------------*/

/*@{*/

/*-------------------------------------------------------------------------*/
/*                         start of code                                   */
/*-------------------------------------------------------------------------*/

/*
* NAME: alarm_task
* DESCRIPTION: Function for selection menu of the alarm types
* PARAMS:None
* RETURNS: void
*/
void alarm_task()
{
	default_custom_chars();
	
	init_list("DAILY");
	add_to_list("SPECIAL");
	add_to_list("WEEKLY");
	
	int alarmCursorPos = 0;
	busy = 1;
	
	display_clear();
	display_string("Alarm",0);
	show_char_custom('3', 16);
	display_string(get_selected(), 21);
	show_char_custom('4', 31);
	
	while(busy == 1)
	{
		if(go_idle)
		{
			home_state(&currentNode);
		}

		if(mayCheck == 1)
		{
			display_string("Alarm",0);
			show_char_custom('3', 16);
			display_string(get_selected(), 21);
			show_char_custom('4', 31);
		
			switch(get_pressed_once()) 
			{

				case RAW_KEY_LEFT  :
					alarmCursorPos = alarmCursorPos-1;
					if(alarmCursorPos < 0)
					{
						alarmCursorPos = 0;
					}
					previous_item();
					display_clear();
					display_string("Alarm",0);
					show_char_custom('3', 16);
					display_string(get_selected(), 21);
					show_char_custom('4', 31);
					break;
				
				case RAW_KEY_RIGHT  :
					alarmCursorPos = alarmCursorPos+1;
					if(alarmCursorPos > 2)
					{
						alarmCursorPos = 2;
					}
					next_item();
					display_clear();
					display_string("Alarm",0);
					show_char_custom('3', 16);
					display_string(get_selected(), 21);
					show_char_custom('4', 31);
					break;
					
				case RAW_KEY_ESC  :
					up_state(&currentNode);
					busy = 0;
					break;
					
				case RAW_KEY_OK  :
					switch(alarmCursorPos) 
					{
						case 0  :
						prev_state(&currentNode);
						busy = 0;
						break;
						
						case 1  :
						next_state(&currentNode);
						busy = 0;
						break;
						
						case 2  :
						down_state(&currentNode);
						busy = 0;
						break;
					}
					break;
			}
		}
		
		NutSleep(10);
	}
}
