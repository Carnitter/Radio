/*	NAME: rss.c
	DESCRIPTION: Handles the menu for rss settings
	AUTHORS: group A1
 */
 
#include <stdio.h>
#include <string.h>

#include "display.h"
#include "keyboard.h"
#include "keyboardhandler.h"
#include "log.h"
#include "displayhandler.h"
#include "list.h"
#include "memoryhandler.h"
#include "stream.h"
#include "state_machine.h"
#include "rss.h"
#include "main_menu.h"
#include "customcharhandler.h"
#include "internet_connector.h"

#define LOG_MODULE  LOG_MAIN_MODULE
/*
	NAME: rss_task
	DESCRIPTION: Function which handles the rss menu
	PARAMS: NONE
	RETURNS: void
 */
 
int rss_settings;
int rss_set = 0;
 
void rss_task()
{
	default_custom_chars();
	display_clear();
	display_string("RSS", 0);
	init_list("Digital");
	add_to_list("Science");
	add_to_list("News");
	show();
	int busy = 1;
	int selected = 0;
	while(busy == 1)
	{
		if(go_idle)
		{
			home_state(&currentNode);
		}
		
		u_short pressed = get_pressed_once();
		switch(pressed)
		{
			case RAW_KEY_DOWN:
			next_item();
			selected++;
			show();
			break;
			
			case RAW_KEY_UP:
			previous_item();
			selected--;
			show();
			break;
			
			case RAW_KEY_ESC:
			up_state(&currentNode);
			busy = 0;
			break;
			
			case RAW_KEY_OK:
			rss_settings = selected;
			load_rss();
			if(!playing)
			{
				rss_set = 1;
			}
			saveSettingsToFlash(current_node_ptr);
			up_state(&currentNode);
			busy = 0;
			break;
			
			
		}
	}		
}

/*
	NAME: show
	DESCRIPTION: Hulp function to show the current mode on screen.
	PARAMS: NONE
	RETURNS: void
 */
void show()
{
	display_clear();
	display_string("RSS mode:", 0);
	show_char_custom('2', 31);
	display_string(get_selected(), 16);
}

void load_rss()
{
	//free(rss_item);
	rss_item = malloc(sizeof(char) * 80);
	/* These are the tags we want to parse. Unspecified tags are NOT parsed to save resources. */
	char *f_tags[] = { "title", NULL };
	/* These are the attributes we want to parse. Unspecified attributes are NOT parsed to save resources. */
	char *f_attribs[] = { NULL };
	switch(rss_settings)
	{
		case 0:
			rss_item = connect_to_rss("136.144.128.124", 80, "/rss/", f_tags, f_attribs);
		break;
		case 1:
			rss_item = connect_to_rss("136.144.128.124", 80, "/rss_wetenschap/", f_tags, f_attribs);
		break;
		case 2:
			rss_item = connect_to_rss("136.144.128.124", 80, "/rss_buitenland/", f_tags, f_attribs);
		break;
		
	}
	int count = 0;
	while(rss_item[count] != '\0')
	{
		count++;
	}
	while(count < 80)
	{
		strcat(rss_item, " ");
		count++;
	}
	stopStream();
	
}

