/*
* NAME: idle
* DESCRIPTION: .c file for the idle menu
* AUTHORS: Group A1
*/

#include <string.h>
#include <stdio.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include "idle.h"
#include "displayhandler.h"
#include "list.h"
#include "state_machine.h"
#include "keyboard.h"
#include "keyboardhandler.h"
#include "display.h"
#include "customcharhandler.h"
#include "audio.h"
#include "rtchandler.h"
#include "schedular.h"
#include "structs.h"
#include "memoryhandler.h"
#include "log.h"
#include "internet_connector.h"
#include "uxml.h"
#include "rss.h"
#include "stream.h"

#define LOG_MODULE  LOG_MAIN_MODULE
#define OK 1
#define NOK -1

int animation = 0;

/*
* NAME: idleC_task
* DESCRIPTION: main function for the idle menu
* PARAMS: 
* RETURNS: void 
*/
void idleC_task()
{
	// LogMsg_P(LOG_INFO, PSTR("before unlock"));
	// NutMutexUnlock(&mutex);
	// LogMsg_P(LOG_INFO, PSTR("unlock"));
	disable_cursor();
	default_custom_chars();
	
	display_clear();
	
	if(idleinit <= 0)
	{
		idleinit = 1;
		loadSettingsFromFlash();
	}
	
	if(idleinit == 1)
	{
		synchronize();
		idleinit = 2;
		saveSettingsToFlash(current_node_ptr);
	}
	
	busy = 1;
	while(busy)
	{

		if(mayCheck == 1)
		{

			show_time();
			show_icons();
			show_date();
			
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
					down_state(&currentNode);
					busy = 0;
					//TODO kadir individueel
					//Temporary flash memory check button
					//saveSettingsToFlash(current_node_ptr);
					break;
					
					case RAW_KEY_02:
					//Temporary flash memory check button
					//loadSettingsFromFlash(); //printf("");
					break;
					
					case RAW_KEY_03:
						navigate_state(&currentNode, 2);
						busy = 0;
					break;
					
					case RAW_KEY_04:
						//check if stream is playing. If playing is 0 get the weather data
						if(playing == 0)
						{
							if(node != NULL)
							{
								free(node);
							}
							
							//create a xml node
							node = malloc(sizeof(UXML_NODE));

							//get the weather data
							get_weather();
						}
					
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
							if(mayCheck == 1)
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
							NutSleep(10);
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
				
			}
		}
		
		NutSleep(10);
	}
}

/*
* NAME: show_time
* DESCRIPTION: get and display the current time on the display
* PARAMS:  
* RETURNS: void
*/
void show_time()
{
	//TODO get current time
	
	Calendar* time;
	time = rtc_get_time();
	
	display_time(*time);
	blinking_colon(&blink_counter, 7);
}


/*
* NAME: show_icons
* DESCRIPTION: check which icons should be shown and display them at the screen
* PARAMS: 
* RETURNS: void
*/
void show_icons(void)
{
	//arrows
	show_char_custom('3', 16);
	show_char_custom('4', 31);
	if(current_node_ptr != NULL)
	{
		show_char_custom('0', 14);
	}
	if(internet_result == 1)
	{
		show_char_custom('1', 15);
	}
	
}
	

/*
* NAME: show_date
* DESCRIPTION: get and display the date on the screen
* PARAMS: 
* RETURNS: void
*/
void show_date(void)
{
	//TODO get current date
	
	Calendar* date;
	date = rtc_get_time();	
	display_date(*date);
}

/*
* NAME: show_weather
* DESCRIPTION: get and display the weather information on the screen. Display the temperature, humidity and the clouds
* PARAMS: 
* RETURNS: void
*/
void show_weather(void)
{
	UXML_NODE* node2 = node;
	
	//display city
	display_string(node2->xmln_attribs->xmla_value,0);
	node2 = node2->xmln_next;
	
	//display temperature
	display_string(node2->xmln_attribs->xmla_value, 18);
	node2 = node2->xmln_next;
	
	//display humidity
	display_string(node2->xmln_attribs->xmla_value, 28);
	display_string("%", 30);
	node2 = node2->xmln_next;
	
	//display clouds
	if(strcmp(node2->xmln_attribs->xmla_value, "clear sky") ==0)
	{
		sun_chars();
		show_animation();
	}
	if(strcmp(node2->xmln_attribs->xmla_value, "few clouds") ==0)
	{
		sun_chars();
		show_animation();
	}
	if(strcmp(node2->xmln_attribs->xmla_value, "scattered cloudsy") ==0)
	{
		weather_custom_chars();
		show_char_custom('0', 9); 
		show_char_custom('1', 10);
		show_char_custom('4', 23);
	}
	if(strcmp(node2->xmln_attribs->xmla_value, "overcast clouds") ==0)
	{
		weather_custom_chars();
		show_char_custom('0', 9); 
		show_char_custom('1', 10);
		show_char_custom('4', 23);
	} 
	if(strcmp(node2->xmln_attribs->xmla_value, "broken clouds") ==0)
	{
		weather_custom_chars();
		show_char_custom('0', 9); 
		show_char_custom('1', 10);
		show_char_custom('4', 23);
	}
	if(strcmp(node2->xmln_attribs->xmla_value, "shower rain") ==0)
	{
		rain_chars();
		show_animation();
	}
	if(strcmp(node2->xmln_attribs->xmla_value, "rain") ==0)
	{
		rain_chars();
		show_animation();
	}
	if(strcmp(node2->xmln_attribs->xmla_value, "thunderstorm") ==0)
	{
		weather_custom_chars();
		show_char_custom('2', 9); 
		show_char_custom('3', 10);
		show_char_custom('4', 23);
	}
	if(strcmp(node2->xmln_attribs->xmla_value, "snow") ==0)
	{
		rain_chars();
		show_animation();
	}
	if(strcmp(node2->xmln_attribs->xmla_value, "mist") ==0)
	{
		weather_custom_chars();
		show_char_custom('0', 9); 
		show_char_custom('1', 10);
		show_char_custom('4', 23);
	}
}

/*
* NAME: get_weather
* DESCRIPTION: connect to the api and get de xml weather data. Store the data in node, a UXML_NODE*.
				NOTE!! you can acces the api 60 times a day. So the programm calls this function when navigates to the idle screen.
* PARAMS: 
* RETURNS: void
*/
void get_weather(void)
{
	node = connect_to_api("136.144.128.124", 80, "/weather/");
}

/*
* NAME: show_animation
* DESCRIPTION: show the left and right icon of the cloud. Animation is the counter to pick the right two chars
* PARAMS: 
* RETURNS: void
*/
void show_animation(void)
{
	switch(animation)
	{
		case 0:
		show_char_custom('0', 9); 
		show_char_custom('1', 10);
		break;
		
		case 1:
		show_char_custom('2', 9); 
		show_char_custom('3', 10);
		break;
		
		case 2:
		show_char_custom('4', 9); 
		show_char_custom('5', 10);
		break;
		
		case 3:
		show_char_custom('6', 9); 
		show_char_custom('7', 10);
		break;
	}
		
	animation++;
	if(animation >3)
	{
		animation = 0;
	}
}


void show_rss(int position)
{	
	//display_clear();
	display_string(rss_item, position);
	//display_string("RSS FEED:", 0);
}

