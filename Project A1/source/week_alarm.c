/*
* NAME: week_alarm
* DESCRIPTION: .c file which has functions to perform the week alarm  menu item tasks
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
#include "week_alarm.h"
#include "schedular.h"
#include "structs.h"
#include "memoryhandler.h"
#include "rtchandler.h"

int hour = 11;
int min = 57;
int week_alarm_cursorPos = 0;
enum am_pm_enum am__pm = PM;
char* dayWeek;

/*
	NAME: week_alarm_task
	DESCRIPTION: Function which handles the week alarm menu
	PARAMS: NONE
	RETURNS: void
 */
void week_alarm_task()
{ 
	disable_cursor();
	default_custom_chars();
	display_clear();
	display_string("Week Alarm", 0);
	init_list("Monday");
	add_to_list("Tuesday");
	add_to_list("Wednesday");
	add_to_list("Thursday");
	add_to_list("Friday");
	add_to_list("Saturday");
	add_to_list("Sunday");
	show_week_alarm();
	busy = 1;
	while(busy == 1)
	{
		if(go_idle)
		{
			home_state(&currentNode);
		}

		if(mayCheck == 1)
		{
			display_string("Week Alarm", 0);
			show_week_alarm();
			
			u_short pressed = get_pressed_once();
			switch(pressed)
			{
				case RAW_KEY_DOWN:
				next_item();
				show_week_alarm();
				break;
				
				case RAW_KEY_UP:
				previous_item();
				show_week_alarm();
				break;
				
				case RAW_KEY_ESC:
				up_state(&currentNode);
				busy = 0;
				disable_cursor();
				break;
				
				case RAW_KEY_OK:
				down_state(&currentNode);
				busy = 0;
				disable_cursor();
				break;
			}
		}
		NutSleep(10);
	}
}


/*
	NAME: week2_alarm_task
	DESCRIPTION: Function which handles the week2 alarm menu
	PARAMS: NONE
	RETURNS: void
 */
void week2_alarm_task()
{
	enable_cursor();
	default_custom_chars();
	display_clear();
	display_string(get_selected(), 0);
	dayWeek = get_selected();
	show_week_time();
	init_list("Sound1");
	add_to_list("Sound2");
	add_to_list("Sound3");
	display_string(get_selected(), 16);
	show_char_custom('2',31);
	week_alarm_cursorPos = 0;
	int busy = 1;
	while(busy == 1)
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
				
				case RAW_KEY_DOWN:
					switch(week_alarm_cursorPos)
					{
						case 0:
							next_item();
							display_string(get_selected(), 16);
							show_char_custom('2',31);
						break;
						case 1:
							show_char('H',31);
							hour--;
							if(hour<0)
								hour = 12;
							show_week_time();
						break;
						case 2:
							show_char('M',31);
							min--;
							if(min<0)
								min = 59;
							show_week_time();
						break;
						case 3:
						if(am__pm == PM)
						{
							am__pm = AM;
							show_week_time();
						}else if(am__pm == AM)
						{
							am__pm = PM;
							show_week_time();
						}
						break;
					}
				break;
				
				case RAW_KEY_UP:
					switch(week_alarm_cursorPos)
					{
						case 0:
							previous_item();
							display_string(get_selected(), 16);
							show_char_custom('2',31);
						break;
						case 1:
							show_char('H',31);
							hour++;
							if(hour>12)
								hour = 0;
							show_week_time();
						break;
						case 2:
							show_char('M',31);
							min++;
							if(min>59)
								min = 0;
							show_week_time();
						break;
						case 3:
						if(am__pm == PM)
						{
							am__pm = AM;
							show_week_time();
						}else if(am__pm == AM)
						{
							am__pm = PM;
							show_week_time();
						}
						break;
					}
				break;
				
				case RAW_KEY_LEFT:
					week_alarm_cursorPos = (week_alarm_cursorPos-1)%4;
					if(week_alarm_cursorPos < 0)
						week_alarm_cursorPos = 3;
						switch(week_alarm_cursorPos)
						{
							case 0:
							show_char_custom('2',31);
							set_cursor(0x13);
							break;
							case 1:
							show_char(' ',31);
							set_cursor(0x0B);
							break;
							case 2:
							show_char(' ',31);
							set_cursor(0x0E);
							break;
							case 3:
							show_char(' ',31);
							set_cursor(0x0F);
							break;
						}
				break;
			
				case RAW_KEY_RIGHT:
				week_alarm_cursorPos = (week_alarm_cursorPos+1)%4;
					switch(week_alarm_cursorPos)
					{
						case 0:
						show_char_custom('2',31);
						set_cursor(0x13);
						break;
						case 1:
						show_char(' ',31);
						set_cursor(0x0B);
						break;
						case 2:
						show_char(' ',31);
						set_cursor(0x0E);
						break;
						case 3:
						show_char(' ',31);
						set_cursor(0x0F);
						break;
					}
				break;
			
				case RAW_KEY_OK:
					weekalarm_set();
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

int day_of_week_number()
{
	if(strcmp(dayWeek, "Monday")==0)
	{
		return 1;
	}
	else if(strcmp(dayWeek, "Tuesday")==0)
	{
		return 2;
	}
	else if(strcmp(dayWeek, "Wednesday")==0)
	{
		return 3;
	}
	else if(strcmp(dayWeek, "Thursday")==0)
	{
		return 4;
	}
	else if(strcmp(dayWeek, "Friday")==0)
	{
		return 5;
	}
	else if(strcmp(dayWeek, "Saturday")==0)
	{
		return 6;
	}
	else if(strcmp(dayWeek, "Sunday")==0)
	{
		return 0;
	}
	else{return 8;}
}

//set alarm
void weekalarm_set()
{
	Calendar *d = malloc(sizeof(Calendar));
	d->day = 0;
	strcpy(d->month, "");
	d->year = 0;
	d->hours = hour;
	d->minutes = min;
	d->am_pm = am__pm;
	d->day_of_week = day_of_week_number();
	
	int stream = 0;
	if(strcmp("Sound1",get_selected()) == 0)
	{
		stream = 0;
	}
	else if(strcmp("Sound2",get_selected()) == 0)
	{
		stream = 1;
	}
	else if(strcmp("Sound3",get_selected()) == 0)
	{
		stream = 2;
	}
	
	add(d, stream, weekly, 0);
	
}

/*
	NAME: show_week_alarm
	DESCRIPTION: Hulp function to show the current mode on screen.
	PARAMS: NONE
	RETURNS: void
 */
void show_week_alarm()
{
	display_clear();
	display_string("Week Alarm:", 0);
	show_char_custom('2', 31);
	display_string(get_selected(), 16);
}

/*
	NAME: show_week_time
	DESCRIPTION: Hulp function to show the current time on screen.
	PARAMS: NONE
	RETURNS: void
 */
void show_week_time()
{
	show_char((char)((hour/10)+'0'),10);
	show_char((char)((hour%10)+'0'),11);
	show_char(':',12);
	show_char((char)((min/10)+'0'),13);
	show_char((char)((min%10)+'0'),14);
	if(am__pm == PM)
	{
		show_char_custom('6',15);
	}
	else
	{
		show_char_custom('5',15);
	}
}
