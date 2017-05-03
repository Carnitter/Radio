/*
* NAME: daily_alarm
* DESCRIPTION: .c file for the daily alarm
* AUTHORS: Group A1
*/

#include "daily_alarm.h"

#include <stdio.h> 
#include <stdlib.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <keyboard.h>
#include "state_machine.h"
#include <string.h>
#include "list.h"
#include "customcharhandler.h"
#include "displayhandler.h"
#include "keyboardhandler.h"
#include "display.h"
#include "schedular.h"
#include "structs.h"
#include "memoryhandler.h"
#include "rtchandler.h"


Calendar alarm_time;

/*
* NAME: show_char
* DESCRIPTION: main function for the daily alarm 
* PARAMS: char character, int position
* RETURNS: void
*/
int changeValue = 0;

void dailyC_alarm_task()
{
	enable_cursor();
	
	display_clear();	
	default_custom_chars();
	
	//@todo get last alarm_time
	
	alarm_time.hours = 0;
	alarm_time.minutes = 0;
	alarm_time.am_pm = AM;
	
	
	
	init_list("Track 1");
	add_to_list("Track 2");
	add_to_list("Track 3");
	
	show_information();

	busy = 1;
	
	
	
	while(busy)
	{
		if(go_idle)
		{
			home_state(&currentNode);
		}
	
		if(mayCheck == 1)
		{
		
			u_short pressed = get_pressed_once();
			switch(pressed)
			{
				case RAW_KEY_ESC:
				up_state(&currentNode);
				busy = 0;
				disable_cursor();
				break;
				
				case RAW_KEY_LEFT:
				changeValue--;
				if(changeValue < 0)
				{
					changeValue = 3;
					
				}
				show_information();
				break;
				
				case RAW_KEY_RIGHT:
				changeValue++;
				if(changeValue > 3)
				{
					changeValue = 0;
					
				}
				show_information();
				break;
				
				case RAW_KEY_UP:
					switch(changeValue)
					{
						case 0:
						up_list();
						break;
						
						case 1:
						alarm_time.hours++;
						if(alarm_time.hours > 12)
						{
							alarm_time.hours = 0;
						}
						show_information();
						break;
						
						case 2:
						alarm_time.minutes++;
						if(alarm_time.minutes > 59)
						{
							alarm_time.minutes = 0;
						}
						show_information();
						break;
						
						case 3:
						if(AM == alarm_time.am_pm)
							alarm_time.am_pm = PM;
						else
							alarm_time.am_pm = AM;
						
						show_information();
						break;
					}
				
				break;
				
				case RAW_KEY_DOWN:
					switch(changeValue)
					{
						case 0:
						down_list();
						break;
						
						case 1:
						alarm_time.hours--;
						if(alarm_time.hours < 0)
						{
							alarm_time.hours = 12;
						}
						show_information();
						break;
						
						case 2:
						alarm_time.minutes--;
						if(alarm_time.minutes < 0)
						{
							alarm_time.minutes = 59;
						}
						show_information();
						break;
						
						case 3:
						if(AM == alarm_time.am_pm)
							alarm_time.am_pm = PM;
						else
							alarm_time.am_pm = AM;
						
						show_information();
						break;
					}
				break;
				
				case RAW_KEY_OK:
					alarm_set();
					saveSettingsToFlash(current_node_ptr);
					up_state(&currentNode);
					busy = 0;
					disable_cursor();
				break; 
			}
		}
		
		NutSleep(10);
	}
}
	
	//funtion to show time + current item list
	void show_information()
	{
		display_string( "Daily" , 0);
		
			//display_string( alarm_time.hours + ":0" + alarm_time.minutes ,6);
			display_time_onpos(alarm_time, 6);
	
		
		display_string(get_selected(), 16);
		
		
		if(changeValue == 0)
		{
			show_char_custom('2', 31);
			set_cursor(0x14);
		}
		else if(changeValue == 1)
		{
			show_char(' ',31);
			set_cursor(0x07);
		}
		else if(changeValue == 2)
		{
			show_char(' ',31);
			set_cursor(0x0A);
		}
		else if(changeValue == 3)
		{
			show_char(' ',31);
			set_cursor(0x0B);
		}
				
	}
	
	//set alarm
	void alarm_set()
	{
		Calendar *d = malloc(sizeof(Calendar));
		d->day = 0;
		strcpy(d->month, "");
		d->year = 0;
		d->hours = alarm_time.hours;
		d->minutes = alarm_time.minutes;
		d->am_pm = alarm_time.am_pm;
		d->day_of_week = 0;
		
		int stream = 0;
		if(strcmp("Track 1",get_selected()) == 0)
		{
			stream = 0;
		}
		else if(strcmp("Track 2",get_selected()) == 0)
		{
			stream = 1;
		}
		else if(strcmp("Track 3",get_selected()) == 0)
		{
			stream = 2;
		}
		
		add(d, stream, normal, 0);
	}
	
	
	void up_list()
	{
		previous_item();
		show_information();
	}
	
	void down_list()
	{
		next_item();
		show_information();
	}

	
		
	
