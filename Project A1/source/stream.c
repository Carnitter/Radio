/*
* NAME: stream
* DESCRIPTION: .c file which has functions to perform a stream menu item task and stream play menu item task
* AUTHORS: Group A1
*/
#define LOG_MODULE LOG_MAIN_MODULE
#include "stream.h"
#include "internet_connector.h"
#include "stream_play.h"
#include <sys/timer.h>
#include <sys/thread.h>
#include "list.h"
#include "log.h"

int playing = 0;
int station = 0;
int station_playing = 0;



/*
* NAME: stream_task
* DESCRIPTION: Function for the stream selection menu item
* PARAMS: None
* RETURNS: void
*/

void stream_task(void)
{
	stream_task_chars();
	int busy = 1;
	display_clear();
	init_list("BlackDeath");
	add_to_list("Radio X-MAS");
	add_to_list("Radio 538");
	
	display_string("Radio Stream",0);
	display_string(get_selected() ,16);
	show_char_custom('6',31);
	station = 0;
	while(busy == 1)
	{
		if(go_idle)
		{
			home_state(&currentNode);
		}

		if(mayCheck == 1)
		{
			display_string("Radio Stream",0);
			display_string(get_selected() ,16);
			show_char_custom('6',31);
			
			switch(get_pressed_once()) 
			{

			case RAW_KEY_UP  :
				display_clear();
				previous_item();
				display_string("Radio Stream",0);
				display_string(get_selected(),16);
				station -= 1;
				if(station < 0) station = 0;
				show_char_custom('6',31);
			break;
			case RAW_KEY_DOWN  :
				display_clear();
				next_item();
				display_string("Radio Stream",0);
				display_string(get_selected(),16);
				station++;
				if(station > 2) station = 2;
				show_char_custom('6',31);
			break;
			case RAW_KEY_ESC  :
				up_state(&currentNode);
				busy = 0;
			break;
			case RAW_KEY_OK  :
				down_state(&currentNode);
				busy = 0;
			break;
			}
		}
		NutSleep(10);
	}
}

/*
* NAME: stream_play_task
* DESCRIPTION: Function for the stream play menu item
* PARAMS: None
* RETURNS: void
*/
void set_station(int number)
{
	station = number;
}

void stop_stream()
{
	playing = 0;
	stop_thread();
}

void play(void)
{
	switch(station)
	{
		case 0:
			if( 1 == connectToStream("31.12.65.200", 80, "/DeathMetal-"))
			{
				playing = 1;
				station_playing = station;
				playStream();
			}
		break;
		case 1:
			if( 1 == connectToStream("77.75.16.238:", 443, "/xmas"))
			{
				playing = 1;
				station_playing = station;
				playStream();
			}
		break;
		case 2:
			if( 1 == connectToStream("37.48.116.150", 80, "/RADIO538_MP3"))
			{
				playing = 1;
				station_playing = station;
				playStream();
			}
		break;						
	}
	
}

void stream_play_task(void)
{
	stream_task_chars();
	int radioCursorPos = 0;
	int radioOption = 0;
	int busy = 1;
	display_clear();
	LogMsg_P(LOG_INFO, PSTR("Station: %i Playing: %i Station playing: %i"),station, playing, station_playing);
	if(station != station_playing)
	{
		playing = 0;
		stop_thread();
	}
	display_string(get_selected(),0);
				show_char_custom('0', 15);
	while(busy == 1)
	{
		if(go_idle)
		{
			home_state(&currentNode);
		}

		if(mayCheck == 1)
		{
			display_string(get_selected(),0);
			//show_char_custom('0', 15);
			
			switch(get_pressed_once()) {

			case RAW_KEY_LEFT  :
				radioCursorPos = (radioCursorPos-1)%3;
				if(radioCursorPos < 0)
					radioCursorPos = 2;
				display_clear();
				display_string(get_selected(),0);
				switch(radioOption) {
				case 0  :
					show_char_custom('0', 15);				
				break;	
				case 1  :
					radioOption = 1;
					show_char_custom('2', 15);				
				break;
		
				case 2  :
					radioOption = 2;
					show_char_custom('4', 15);
					
				break;
				}
			break;
			case RAW_KEY_RIGHT  :
				radioCursorPos = (radioCursorPos+1)%3;
				display_clear();
				display_string(get_selected(),0);
				switch(radioOption) {
				case 0  :
					radioOption = 0;
					show_char_custom('0', 15);
				break;
		
				case 1  :
					radioOption = 1;
					show_char_custom('2', 15);
				break;
		
				case 2  :
					radioOption = 2;
					show_char_custom('4', 15);
				break;
				}
			break;
			case RAW_KEY_ESC  :
				up_state(&currentNode);
				busy = 0;
			break;
			case RAW_KEY_OK  :
				switch(radioCursorPos) {
				case 0  :
					radioOption = 0;				
					stop_thread();
					playing = 0;
					printf("stop\n");
					show_char_custom('0', 15);
				break;
		
				case 1  :
					radioOption = 1;
					if(playing == 0){
						play();
						printf("play\n");
						show_char_custom('2', 15);
					}
					else if(playing == 1)
					{
						volumemax();
					}
				break;
		
				case 2  :
					if(playing == 1)
					{
						radioOption = 2;
						pausestream();
						show_char_custom('4', 15);
						printf("pauze\n");
					}
					
				break;
				}
			break;
			}
			switch(radioCursorPos) {

			case 0  :
				show_char_custom('7', 16);
				show_char_custom('1', 17);
				show_char_custom('7', 18);
				show_char_custom('2', 23);
				show_char_custom('4', 30);
			break;
		
			case 1  :
				show_char_custom('0', 17);
				show_char_custom('7', 22);
				show_char_custom('3', 23);
				show_char_custom('7', 24);
				show_char_custom('4', 30);
			break;
		
			case 2  :
				show_char_custom('0', 17);
				show_char_custom('2', 23);
				show_char_custom('7', 29);
				show_char_custom('5', 30);
				show_char_custom('7', 31);
			break;
			}
		}
		NutSleep(10);
	}
}
