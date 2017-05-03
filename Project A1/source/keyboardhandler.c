/*	NAME: keyboardhandler.c
*	DESCRIPTION: Handles the key reading. Saves the states of the keys.
*	AUTHORS: group A1
*/



#include <stdio.h>
#include <string.h>
#include "keyboard.h"
#include "keyboardhandler.h"

#include "log.h"

#define LOG_MODULE  LOG_MAIN_MODULE

u_short lastkeypressed = -1;

 /*
	NAME: get_pressed
	DESCRIPTION: returns the key that is currently pressed.
	RETURNS: returns u_short of the pressed key.
 */
u_short get_pressed(void)
{	
	u_short i = get_keyfound();
	return i;
}

 /*
	NAME: is_pressed
	DESCRIPTION: checks if the requested key is pressed.
	PARAMS: key_requested = key to check.
	RETURNS: returns 0 if false and 1 if true.
 */
int is_pressed(u_short key_requested)
{
	u_short key_pressed = get_pressed_once();
	if(key_pressed != -1)
	{
		LogMsg_P(LOG_INFO, PSTR("Pressed: %i Requested: %i"), key_pressed, key_requested);
	}
	
	if(key_pressed == key_requested)
	{
		return 1;
	}else
	{
		return 0;
	}
}

 /*
	NAME: get_pressed_once
	DESCRIPTION: returns the key that is pressed, but if the key has been pressed last it returns -1
	this way the key only triggers once.
	RETURNS: returns -1 or the value of the key.
 */
u_short get_pressed_once(void)
{	
	LogInit();
	u_short i = get_keyfound();
	
	u_short return_value = -1;
	if(lastkeypressed != i)
	{
		return_value = i;
	}	
	lastkeypressed = i;
	
	return return_value;
}
