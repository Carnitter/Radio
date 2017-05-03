/*
* NAME: main_menu
* DESCRIPTION: .c file for some submenu taskts
* AUTHORS: Group A1
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <sys/heap.h>
#include "displayhandler.h"
#include <keyboardhandler.h>
#include <keyboard.h>
#include "state_machine.h"
#include <string.h>
#include "rss.h"
#include "idle.h"
#include "menu_alarm.h"
#include "menu_stream.h"
#include "menu_rss.h"
#include "special.h"
#include "customcharhandler.h"
#include "daily_alarm.h"
#include "list.h"
#include "display.h"
#include "schedular.h"
#include "rtchandler.h"
#include "memoryhandler.h"
#include "flash.h"
#include "structs.h"
#include <fs/fs.h>
#include <dev/urom.h>
#include <fs/uromfs.h>
#include <io.h>

char text[16];
int busy = 0;
int alarm_busy = 0;
HANDLE timer, timer2;
HANDLE event = 0, event2 = 0;

/*
	NAME: select_language_task
	DESCRIPTION: Function which handles the language selection state of the menu.
	PARAMS: NONE
	RETURNS: void
 */
void select_language_task(void)
{
	mayCheck = 1;
	
	default_custom_chars();
	if(timezone==-999)
	{
		timezone=0;
		//init list with countrys
		init_list("United Kingdom");
		add_to_list("Netherlands");
		add_to_list("Greece");
		
		strncpy(text, "Choose Country:", sizeof(text));
		display_clear();
		display_string(text, 0);
		display_string(get_selected(), 16);
		show_char_custom('2', 31);
		
		int busy = 1;
		while(busy == 1)
		{
			u_short pressed =  get_pressed_once();
			switch(pressed)
			{
				case RAW_KEY_OK:
				saveSettingsToFlash(current_node_ptr);
				down_state(&currentNode);
				busy = 0;
				break;
				
				case RAW_KEY_UP:
					if(get_selected_item()->previous)
					{
						timezone++;
					}
					previous_item();
					display_clear();
					display_string(text, 0);
					display_string(get_selected(), 16);
					show_char_custom('2', 31);
				break;
				
				case RAW_KEY_DOWN:
					if(get_selected_item()->next)
					{
						timezone--;
					}
					next_item();
					display_clear();
					display_string(text, 0);
					display_string(get_selected(), 16);
					show_char_custom('2', 31);
				break;
			}
		}
	}
	else
	{
		down_state(&currentNode);
	}
}

/*
	NAME: no_internet_task
	DESCRIPTION: Function which handles the no internet state of the menu
	PARAMS: NONE
	RETURNS: void
 */
void no_internet_task(void)
{
	strncpy(text, "no internet", sizeof(text));
	display_clear();
    display_string(text, 0);
	
	int busy = 1;
	while(busy == 1)
	{
		u_short pressed =  get_pressed_once();
		switch(pressed)
		{
			case RAW_KEY_OK:
			down_state(&currentNode);
			busy = 0;
			break;
			
			case RAW_KEY_LEFT:
			next_state(&currentNode);
			busy = 0;
			break;
			
			case RAW_KEY_RIGHT:
			prev_state(&currentNode);
			busy = 0;
			break;
		}
	}
}

/*
	NAME: idle_task
	DESCRIPTION: Function which handles the idle state of the menu
	PARAMS: NONE
	RETURNS: void
 */
void idle_task(void)
{
	idleC_task();
}




/*
	NAME: menu_rss_task
	DESCRIPTION: Function which handles selection menu before the rss menu
	PARAMS: NONE
	RETURNS: void
 */
void menu_rss_task(void)
{
    menu_rssC_task();
}

/*
	NAME: menu_alarm_task
	DESCRIPTION: Function which handles the selection menu before the alarm menu
	PARAMS: NONE
	RETURNS: void
 */
void menu_alarm_task(void)
{
    menu_alarmC_task();
}

/*
	NAME: menu_stream_task
	DESCRIPTION: Function which handles the selection menu before the alarm overview menu
	PARAMS: NONE
	RETURNS: void
 */
void menu_stream_task(void)
{
    menu_streamC_task();
}

void daily_alarm_task(void)
{
	dailyC_alarm_task();
}


THREAD(TimerEvent, arg)
{
    NutThreadSetPriority(32);
	printf("in normal and weekly event");
	for (;;) 
	{
        if (NutEventWait(arg, 121000))
		{
            snooze();
			alarm_busy = 0;
		}
        else
		{
			snooze();
			alarm_busy = 0;
		}
		NutSleep(10);
		break;
    }
	NutSleep(10);
	NutTimerStop(timer);
	NutThreadExit();
}

THREAD(TimerEvent2, arg)
{
    NutThreadSetPriority(32);
	printf("in special event");
	    for (;;) {
        if (NutEventWait(arg, 1801000))
		{
            stop_alarm();
			alarm_busy = 0;
		}
        else
		{
			stop_alarm();
			alarm_busy = 0;
		}
		NutSleep(10);
		break;
    }
	NutSleep(10);
	NutTimerStop(timer);
	NutThreadExit();
}

void TimerCallback(HANDLE timer, void *arg)
{
	NutEventPostAsync(arg);
}

void alarm_state_task(void)
{
	mayCheck = 0;
	
	disable_cursor();
	default_custom_chars();
		
	display_clear();
	

	//Doesn't work so it's in comments
	// if(alarmThatIsGoingOff == special)
	// {
		// printf("\n alarm type: %i \n", alarmThatIsGoingOff);
		// NutThreadCreate("tmr2", TimerEvent2, &event2, 512);
		// timer2 = NutTimerStart(1800000, TimerCallback, &event2, TM_ONESHOT);
	// }
	// else
	// {
		// printf("\n alarm type: %i \n", alarmThatIsGoingOff);
		// //Doesn't work
		// NutThreadCreate("tmr1", TimerEvent, &event, 512);
		// timer = NutTimerStart(120000,TimerCallback,&event, TM_ONESHOT);
	// }
	
	
	alarm_busy = 1;
	while(alarm_busy == 1)
	{
		LcdBackLight(LCD_BACKLIGHT_ON);
		show_time();
		
		 //TODO check 2 minutes -> no action stop alarm
		
		u_short pressed = get_pressed_once();
		switch(pressed)
		{
			case RAW_KEY_ESC:
				snooze();
				LcdBackLight(LCD_BACKLIGHT_OFF);
				//Doesn't work, so it's in comments
				//NutTimerStop(timer);
				//NutTimerStop(timer2);
				alarm_busy = 0;
			break;
			
			case RAW_KEY_ALT:
				stop_alarm();
				LcdBackLight(LCD_BACKLIGHT_OFF);
				//Doesn't work, so it's in comments
				//NutTimerStop(timer);
				//NutTimerStop(timer2);
				alarm_busy = 0;
			break;
		}
		NutSleep(10);
	}
	
	printf("\n end of alarm_state_task \n");
	display_clear();
	mayCheck = 1;
}
