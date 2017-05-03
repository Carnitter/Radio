/*
* NAME: idle
* DESCRIPTION: .c file for the idle menu
* AUTHORS: Group A1
*/
#define LOG_MODULE LOG_MAIN_MODULE
#include "log.h"
#include <string.h>
#include <stdio.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include "special.h"
#include "displayhandler.h"
#include "list.h"
#include "state_machine.h"
#include "keyboard.h"
#include "keyboardhandler.h"
#include "customcharhandler.h"
#include "schedular.h"
#include "structs.h"
#include "display.h"
#include "memoryhandler.h"
#include "rtchandler.h"

int hours= 0;
int minutes = 0;
enum am_pm_enum am_pm = AM;
int day = 1;
int year = 2017;
int valueToChange = 0;



/*
* NAME: spec_alarm_task
* DESCRIPTION: main function for the special alarm
* PARAMS:
* RETURNS: void
*/
void spec_alarm_task()
{
	enable_cursor();
	default_custom_chars();
	create_month_list();

	display_clear();

	display_string("Special", 0);
	show_info();
	
	busy = 1;
	while(busy)
	{
		if(go_idle)
		{
			home_state(&currentNode);
		}

		if(mayCheck == 1)
		{
			display_string("Special", 0);
			show_info();
	
			u_short pressed = get_pressed_once();
			switch(pressed)
			{
				case RAW_KEY_LEFT:
					valueToChange--;
					if(valueToChange < 0)
						valueToChange = 5;
						show_info();
				break;

				case RAW_KEY_RIGHT:
					valueToChange++;
					if(valueToChange > 5)
						valueToChange = 0;
						show_info();
				break;

				case RAW_KEY_UP:
					change_value(1);
					show_info();
				break;

				case RAW_KEY_DOWN:
					change_value(0);
					show_info();
				break;

				case RAW_KEY_ESC:
					up_state(&currentNode);
					busy = 0;
					disable_cursor();
				break;

				case RAW_KEY_OK:
					specalarm_set();
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

/*
* NAME: show_time_s
* DESCRIPTION: display the time
* PARAMS:
* RETURNS: void
*/
void show_time_s()
{
	Calendar time;
	time.hours = hours;
	time.minutes = minutes;
	time.am_pm = am_pm;

	display_time_onpos(time, 8);
}

/*
* NAME: show_date_s
* DESCRIPTION: display the date
* PARAMS:
* RETURNS: void
*/
void show_date_s(void)
{
	//TODO get current date

	Calendar date;
	date.day = day;
	date.year = year;
	strcpy(date.month, get_selected());

	display_date(date);
}

/*
* NAME: create_month_list
* DESCRIPTION: create the list with all the months
* PARAMS:
* RETURNS: void
*/
void create_month_list()
{
	init_list("January");
	add_to_list("February");
	add_to_list("March");
	add_to_list("April");
	add_to_list("May");
	add_to_list("June");
	add_to_list("July");
	add_to_list("August");
	add_to_list("September");
	add_to_list("October");
	add_to_list("November");
	add_to_list("December");
}

/*
* NAME: change_value
* DESCRIPTION: change the selected value;
* PARAMS: int increase if 0 value decrease. if 1 value increase.
* RETURNS: void
*/
void change_value(int increase)
{
	switch(valueToChange)
	{
		case 0:
			if(increase)
			{
				day++;
				if(day > 31)
				{
					day = 1;
				}
			}
			else
			{
				day--;
				if(day < 1)
				{
					day = 31;
				}
			}
		break;

		case 1:
			if(increase)
			{
				next_item();
			}
			else
			{
				previous_item();
			}
		break;

		case 2:
			if(increase)
			{
				year++;
			}
			else
			{
				year--;
				if(year < 2017)
				{
					year = 2017;
				}
			}
		break;

		case 3:
			if(increase)
			{
				hours++;
				if(hours > 12)
				{
					hours = 0;
				}
			}
			else
			{
				hours--;
				if(hours < 0)
				{
					hours = 12;
				}
			}
		break;

		case 4:
			if(increase)
			{
				minutes++;
				if(minutes > 59)
				{
					minutes = 0;
				}
			}
			else
			{
				minutes--;
				if(minutes < 0)
				{
					minutes = 59;
				}
			}
		break;

		case 5:
			if(AM == am_pm)
				am_pm = PM;
			else
				am_pm = AM;
		break;
	}
}

//set alarm
void specalarm_set()
{
	Calendar *d = malloc(sizeof(Calendar));
	d->day = day;
	strcpy(d->month, get_selected());
	d->year = year;
	d->hours = hours;
	d->minutes = minutes;
	d->am_pm = am_pm;
	d->day_of_week = 0;
	int stream = 0;
	add(d, stream, special, 0);
}


void show_info()
{
	    show_time_s();
		show_date_s();
		
		if(valueToChange == 0)
		{
			show_char('D',31);
			set_cursor(0x13);
		}
		else if(valueToChange == 1)
		{
			show_char('M',31);
			set_cursor(0x18);
		}
		else if(valueToChange == 2)
		{
			show_char('Y',31);
			set_cursor(0x1D);
		}
		else if(valueToChange == 3)
		{
			show_char(' ',31);
			set_cursor(0x09);
		}
		else if(valueToChange == 4)
		{
			show_char(' ',31);
			set_cursor(0x0C);
		}
		else if (valueToChange == 5)
		{
			show_char(' ',31);
			set_cursor(0x0D);
		}		
}
