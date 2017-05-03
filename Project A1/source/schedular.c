#include <stdlib.h>  
#include "structs.h"
#include "schedular.h"
#include "rtchandler.h"
#include "log.h"
#define LOG_MODULE  LOG_MAIN_MODULE
#include "audio.h"
#include "state_machine.h"
#include "display.h"
#include "idle.h"
#include "displayhandler.h"
#include "stream.h"

#define SNOOZETIME 1
#define MAXSNOOZE 11

/*
	NAME: schedular_init
	DESCRIPTION: Function to intialize the linked list for the schedular
	PARAMS: None
	RETURNS: void
 */
void schedular_init()
{
	current_node_ptr = malloc(sizeof(Node));
	current_node_ptr = NULL;
	
	alarm_trigger_ptr = malloc(sizeof(Node));
	alarm_trigger_ptr = NULL;
	alarmThatIsGoingOff = normal;
}

/*
	NAME: add
	DESCRIPTION: Function to add a alarm to the schedular
	PARAMS: date -> pointer to Calendar struct with date and time of the alarm
			stream -> integer that indicates which stream will be played when the alarm goes off
			alarm_type -> type of the alarm
	RETURNS: void
 */
void add(Calendar * date, int stream, enum type alarm_type, int snoozecnt)
{
	if(list_size()<6)
	{
		Node *i = malloc(sizeof(Node));
		Node *last_ptr = current_node_ptr;

		Schedular_Item *alarm = malloc(sizeof(Schedular_Item));
		alarm->date = date;
		alarm->stream = stream;
		alarm->alarm_type = alarm_type;
		LogMsg_P(LOG_INFO, PSTR("snooze %i"), snoozecnt);
		alarm->snoozecount = snoozecnt;

		i->next = NULL;
		i->prev = NULL;
		i->schedular_item = alarm;

		if(i != NULL)
		{
			if(current_node_ptr == NULL)
			{
				current_node_ptr = i;
			}
			else
			{
				while(last_ptr->next != NULL)
				{
					last_ptr = last_ptr->next;
				}
				last_ptr->next = i;
				i->prev = last_ptr;
				i->next = NULL;
			}
		}
		
		Node *item_ptr = current_node_ptr;
		while(item_ptr != NULL)
		{
			LogMsg_P(LOG_INFO, PSTR("Alarm %i: d: %i, m: %s, y: %i, h: %i, m: %i, dw: %i"), item_ptr->schedular_item->alarm_type,
			item_ptr->schedular_item->date->day, item_ptr->schedular_item->date->month, item_ptr->schedular_item->date->year,
			item_ptr->schedular_item->date->hours, item_ptr->schedular_item->date->minutes, item_ptr->schedular_item->date->day_of_week);
			item_ptr = item_ptr->next;
		}
	}
	else
	{
		LogMsg_P(LOG_INFO, PSTR("There are already 6 alarms in the memory!"));
	}
}

/*
	NAME: equalItem
	DESCRIPTION: compare two schedular_items if they are equal and return 1 when equal and 0 when not equal
	PARAMS: Schedular_Item, Schedular_Item
	RETURNS: int
 */
int equalItem(Schedular_Item *to_remove, Schedular_Item *to_compare)
{
	if(to_remove->alarm_type == weekly)
	{
		if(to_remove->date->day_of_week == to_compare->date->day_of_week)
		{
			return 1;
		}
		return 0;
	}
	else
	{
		if(to_remove->date->year == to_compare->date->year)
		{
			if(strcmp(to_remove->date->month,to_compare->date->month)==0)
			{
				if(to_remove->date->day == to_compare->date->day)
				{
					if(to_remove->date->hours == to_compare->date->hours)
					{
						if(to_remove->date->minutes == to_compare->date->minutes)
						{
							if(to_remove->date->am_pm == to_compare->date->am_pm)
							{
								if(to_remove->alarm_type == to_compare->alarm_type)
								{
									return 1;
								}
							}
						}
					}
				}
			}
		}
		return 0;
	}
}

/*
	NAME: remove
	DESCRIPTION: remove the schedular_item from the list with alarms
	PARAMS: Schedular_Item 
	RETURNS: void
 */
void remove(Schedular_Item *to_remove)
{
    if(current_node_ptr != NULL)
    {
        if(equalItem(to_remove, current_node_ptr->schedular_item))
        {
            Node *last_ptr = current_node_ptr;
            current_node_ptr = last_ptr->next;
            current_node_ptr->prev = NULL;
			free(last_ptr->schedular_item->date);
			free(last_ptr->schedular_item);
            free(last_ptr);
        }
        else
        {
            Node *last_ptr = current_node_ptr;

            while(last_ptr != NULL)
            {
                if(equalItem(to_remove, last_ptr->schedular_item))
                {
                    Node *previous = last_ptr->prev;
                    previous->next = last_ptr->next;
                    Node *next = last_ptr->next;
                    if(next != NULL)
                    {
                        next->prev = previous;
                    }
                    free(last_ptr->schedular_item->date);
					free(last_ptr->schedular_item);
					free(last_ptr);
                    last_ptr = next;
                    if(last_ptr)
                    {
                        previous = last_ptr->prev;
                    }
                    continue;
                }
				else
                {
                    last_ptr = last_ptr->next;
                }
            }
        }
		Node *item_ptr = current_node_ptr;
		while(item_ptr != NULL)
		{
			LogMsg_P(LOG_INFO, PSTR("Alarm %i: d: %i, m: %s, y: %i, h: %i, m: %i, dw: %i"), item_ptr->schedular_item->alarm_type,
			item_ptr->schedular_item->date->day, item_ptr->schedular_item->date->month, item_ptr->schedular_item->date->year,
			item_ptr->schedular_item->date->hours, item_ptr->schedular_item->date->minutes, item_ptr->schedular_item->date->day_of_week);
			item_ptr = item_ptr->next;
		}
    }
}

/*
	NAME: check
	DESCRIPTION: Function to check if there is a alarm to go off, if there is a alarm to go off, play the stream of the alarm.
	PARAMS: None
	RETURNS: void
 */
void check()
{
	Calendar *current_time;
	current_time = rtc_get_time();

	if(current_node_ptr != NULL)
    {
		
		Node *last_ptr = current_node_ptr;
		 while(last_ptr != NULL)
		 {
			 
			 if(current_time->hours == last_ptr->schedular_item->date->hours 
			 && current_time->minutes == last_ptr->schedular_item->date->minutes
			 && current_time->am_pm == last_ptr->schedular_item->date->am_pm)
			 {
				 switch(last_ptr->schedular_item->alarm_type)
				 {
					 case normal:
						 LogMsg_P(LOG_INFO, PSTR("Daily: BEEP"));
						 alarm_trigger_ptr = last_ptr;						 
						 stop_stream();
						 LogMsg_P(LOG_INFO, PSTR("station to play: %i"), last_ptr->schedular_item->stream);
						 set_station(last_ptr->schedular_item->stream);
						 play();
						 alarmThatIsGoingOff = normal;
						 LcdBackLight(LCD_BACKLIGHT_ON);
						 navigate_alarm_state();						
					 break;
					 
					 case weekly:
					 if(last_ptr->schedular_item->date->day_of_week == current_time->day_of_week)
					 {
						 LogMsg_P(LOG_INFO, PSTR("Weekly: BEEP"));
						 alarm_trigger_ptr = last_ptr;
						 stop_stream();
						 set_station(last_ptr->schedular_item->stream);
						 play();
						 alarmThatIsGoingOff = weekly;
						 LcdBackLight(LCD_BACKLIGHT_ON);
						 navigate_alarm_state();
					 }
					 break;
					 
					 case special:
					 if(last_ptr->schedular_item->date->day == current_time->day &&  strcmp(last_ptr->schedular_item->date->month,current_time->month) == 0 &&
					 last_ptr->schedular_item->date->year == current_time->year)
					 {
						 LogMsg_P(LOG_INFO, PSTR("Special: BEEP"));
						 alarm_trigger_ptr = last_ptr;
						 stop_stream();
						 set_station(last_ptr->schedular_item->stream);
						 play();
						 alarmThatIsGoingOff = special;
						 navigate_alarm_state();
						 remove(last_ptr->schedular_item);
					 }
					 break;
				 }
			 }
			 last_ptr = last_ptr->next;
		 }
	}
}

/*
* NAME: snooze;
* DESCRIPTION: snooze the alarm that triggers
* PARAMS:  
* RETURNS: void
*/
void snooze()
{
	if(alarm_trigger_ptr != NULL)
	{
		int new_minutes = alarm_trigger_ptr->schedular_item->date->minutes;
		new_minutes += SNOOZETIME;
		int new_hours = alarm_trigger_ptr->schedular_item->date->hours;
		enum am_pm_enum new_am_pm = alarm_trigger_ptr->schedular_item->date->am_pm;
		if(new_minutes > 59)
		{
			new_hours += 1;
			if(new_hours > 12)
			{
				new_hours -= 12;
				if(new_am_pm == AM)
				{
					new_am_pm = PM;
				}
				else
				{
					new_am_pm = AM;
				}
			}
			new_minutes -= 60;
		}
		
		switch(alarm_trigger_ptr->schedular_item->alarm_type)
		{
			case normal:
				LogMsg_P(LOG_INFO, PSTR("SNOOZE"));
				
				stop_stream();
				LcdBackLight(LCD_BACKLIGHT_OFF);
				
				if(alarm_trigger_ptr->schedular_item->snoozecount < MAXSNOOZE)
				{
					Calendar *d = malloc(sizeof(Calendar));
					d->day = 0;
					strcpy(d->month, "");
					d->year = 0;
					d->hours = new_hours;
					d->minutes = new_minutes;
					d->am_pm = new_am_pm;
					d->day_of_week = 0;
					
					int snoozec = alarm_trigger_ptr->schedular_item->snoozecount;
					LogMsg_P(LOG_INFO, PSTR("%i"), snoozec);
					add(d, alarm_trigger_ptr->schedular_item->stream, normal, snoozec+1);
					
					if(alarm_trigger_ptr->schedular_item->snoozecount > 0)
					{
						LogMsg_P(LOG_INFO, PSTR("REMOVE a snooze"));
						remove(alarm_trigger_ptr->schedular_item);
					}
					
					alarm_trigger_ptr = NULL;
				}
				else
				{
					if(alarm_trigger_ptr->schedular_item->snoozecount > 0)
					{
						LogMsg_P(LOG_INFO, PSTR("REMOVE a snooze"));
						remove(alarm_trigger_ptr->schedular_item);
					}
					
					alarm_trigger_ptr = NULL;
				}
			break;
			
			case weekly:
				LogMsg_P(LOG_INFO, PSTR("SNOOZE"));
				stop_stream();
				
				LcdBackLight(LCD_BACKLIGHT_OFF);
				
				if(alarm_trigger_ptr->schedular_item->snoozecount < MAXSNOOZE)
				{
					Calendar *d = malloc(sizeof(Calendar));
					d->day = 0;
					strcpy(d->month, "");
					d->year = 0;
					d->hours = new_hours;
					d->minutes = new_minutes;
					d->am_pm = new_am_pm;
					d->day_of_week = alarm_trigger_ptr->schedular_item->date->day_of_week;	

					int snoozec = alarm_trigger_ptr->schedular_item->snoozecount;
					add(d, alarm_trigger_ptr->schedular_item->stream, weekly, snoozec+1);	

					if(alarm_trigger_ptr->schedular_item->snoozecount > 0)
					{
						LogMsg_P(LOG_INFO, PSTR("REMOVE a snooze"));
						remove(alarm_trigger_ptr->schedular_item);
					}					
				
					alarm_trigger_ptr = NULL;
				}
				else
				{
					if(alarm_trigger_ptr->schedular_item->snoozecount > 0)
					{
						LogMsg_P(LOG_INFO, PSTR("REMOVE a snooze"));
						remove(alarm_trigger_ptr->schedular_item);
					}
					
					alarm_trigger_ptr = NULL;
				}
			break;
			
			case special:
			break;
		}		
	}
}

int list_size()
{
	Node * i;
	int result = 0;
	if(current_node_ptr == NULL)
		return result;
	else
	{
		result++;
		i = current_node_ptr;
		while(i->next != NULL)
		{
			result++;
			i = i->next;
		}
		return result;
	}
}

void remove_all()
{
	if(current_node_ptr !=NULL)
	{
		Node *last_ptr = current_node_ptr;
		while(last_ptr->next != NULL)
		{
			last_ptr = last_ptr->next;
		}
		while(last_ptr->prev != NULL)
		{
			last_ptr = last_ptr->prev;
			free(last_ptr->next->schedular_item->date);
			free(last_ptr->next->schedular_item);
			free(last_ptr->next);
			last_ptr->next = NULL;
		}
		current_node_ptr = NULL;
	}
}

/*
* NAME: stop_alarm;
* DESCRIPTION: stop the alarm that triggers
* PARAMS: 
* RETURNS: void 
*/
void stop_alarm()
{
	if(alarm_trigger_ptr != NULL)
	{
		switch(alarm_trigger_ptr->schedular_item->alarm_type)
		{
			case normal:
				LogMsg_P(LOG_INFO, PSTR("STOP"));
				stop_stream();
				
				LcdBackLight(LCD_BACKLIGHT_OFF);
				
				
				if(alarm_trigger_ptr->schedular_item->snoozecount > 0)
				{
					LogMsg_P(LOG_INFO, PSTR("REMOVE a snooze"));
					remove(alarm_trigger_ptr->schedular_item);
				}
				alarm_trigger_ptr = NULL;
			break;
			
			case weekly:
				LogMsg_P(LOG_INFO, PSTR("STOP"));
				stop_stream();
				
				LcdBackLight(LCD_BACKLIGHT_OFF);
				
				if(alarm_trigger_ptr->schedular_item->snoozecount > 0)
				{
					LogMsg_P(LOG_INFO, PSTR("REMOVE a snooze"));
					remove(alarm_trigger_ptr->schedular_item);
				}
				alarm_trigger_ptr = NULL;
			break;
			
			case special:
			break;
		}
	}
}
