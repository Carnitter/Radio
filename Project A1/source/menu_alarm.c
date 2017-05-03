/*
* NAME: menu_alarm
* DESCRIPTION: .c file for the alarm menu item
* AUTHORS: Group A1
*/

#include <string.h>
#include <stdio.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include "idle.h"
#include "menu_alarm.h"
#include "displayhandler.h"
#include "list.h"
#include "state_machine.h"
#include "keyboard.h"
#include "keyboardhandler.h"
#include "customcharhandler.h"
#include "schedular.h"
#include "stream.h"
#include "rss.h"


/*
* NAME: menu_alarmC_task
* DESCRIPTION: main function for the alarm menu
* PARAMS: 
* RETURNS: void
*/
void menu_alarmC_task()
{
	default_custom_chars();
	disable_cursor();
	display_clear();
	
	busy = 1;
	while(busy)
	{
		if(go_idle)
		{
			home_state(&currentNode);
		}

		if(mayCheck == 1)
		{
			show_time();
			show_icons();
			display_string("Alarm",22);
			
			u_short pressed = get_pressed_once();
			switch(pressed)
			{
				case RAW_KEY_LEFT:
				prev_state(&currentNode);
				busy = 0;
				break;
				
				case RAW_KEY_RIGHT:
				next_state(&currentNode);
				busy = 0;
				break;
				
				case RAW_KEY_01:
				//TODO kadir individueel
				break;
				
				case RAW_KEY_02:
				//TODO DAILY
				break;
				
				case RAW_KEY_03:
					navigate_state(&currentNode,1);
					busy = 0;
				break;
				
				case RAW_KEY_04:
					//Display weather
					display_clear();
					while(get_pressed() == RAW_KEY_04)
					{
						if(mayCheck == 1)
						{
						 show_weather();
						}
						NutSleep(10);
					}
					display_clear();
					default_custom_chars();
				break;
				
				case RAW_KEY_05:
					if(!playing && rss_set == 1){
						display_clear();
						int i = 0;
						int wait = 0;
						while(get_pressed() == RAW_KEY_05)
						{
							
							show_rss(i);
							if(wait > 50)
							{
								wait = 0;
								i--;
								NutSleep(10);
							}
							if(i <= -48)
							{
								display_clear();
								i = 0;
							}
							wait++;
						}
						display_clear();
					}
				break;
				
				case RAW_KEY_ESC:
					snooze();
				break;
				
				case RAW_KEY_ALT:
					stop_alarm();
				break;
				
				case RAW_KEY_OK:
					down_state(&currentNode);
					busy = 0;
				break;
			}
		}
		
		NutSleep(10);
	}
}

