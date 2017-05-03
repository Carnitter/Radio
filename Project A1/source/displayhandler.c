/*
* NAME: displayhandler
* DESCRIPTION: link between the display and the upper layer source files
* AUTHORS: Group A1
*/

#include "display.h"
#include "displayhandler.h"
#include "customcharhandler.h"
#include "structs.h"

int blink_counter = 0;

/*
* NAME: show_char
* DESCRIPTION: show a character at a given position on the lcd screen.
*			   0x80 -> first line.
*			   0xC0 -> second line.
*			   position:
*			   left top corner -> 0.
*			   right top corner -> 15.
*			   left bottom corner -> 16.
*			   right bottom corner -> 31. 
* PARAMS: char character, int position
* RETURNS: void
*/
void show_char(char character, int position)
{
	char pos;
	if(position < 16)
	{
		pos = 0x80|position;
	}
	else
	{
		pos = 0xC0|(position-16);
	}
	set_cursor(pos);
	LcdChar(character);
}

/*
* NAME: show_char
* DESCRIPTION: show a character at a given position on the lcd screen.
*			   0x80 -> first line.
*			   0xC0 -> second line.
*			   position:
*			   left top corner -> 0.
*			   right top corner -> 15.
*			   left bottom corner -> 16.
*			   right bottom corner -> 31. 
* PARAMS: char character, int position
* RETURNS: void
*/
void show_char_custom(char character, int position)
{
	char pos;
	if(position < 16)
	{
		pos = 0x80|position;
	}
	else
	{
		pos = 0xC0|(position-16);
	}
	set_cursor(pos);
	LcdCustomChar(character);
}

/*
* NAME: display_clear
* DESCRIPTION: clear the display, so there a no character on it anymore 
* PARAMS: 
* RETURNS: void
*/
void display_clear()
{
	clear();
}

/*
* NAME: display_time
* DESCRIPTION: display the time at the right place on the screen.
*			   check for an extra 0 at the front of the hours and at the front of the minutes.
* PARAMS: Time time
* RETURNS: void
*/
void display_time(Calendar time)
{
	if(time.hours > 9)
	{
		char h1 = (char)(time.hours/10) + '0';
		char h2 = (char)(time.hours%10) + '0';
		show_char(h1, 5);
		show_char(h2, 6);
	}
	else
	{
		char h = (char)(time.hours) + '0';
		show_char('0', 5);
		show_char(h, 6);
	}
	
	show_char(':', 7);
	
	if(time.minutes > 9)
	{
		char m1 = (char)(time.minutes/10) + '0';
		char m2 = (char)(time.minutes%10) + '0';
		show_char(m1, 8);
		show_char(m2, 9);
	}
	else
	{
		char m = (char)(time.minutes) + '0';
		show_char('0', 8);
		show_char(m, 9);
	}
	
	if(AM == time.am_pm)
	{
		show_char_custom('5', 10);
	}
	else if(PM == time.am_pm)
	{
		show_char_custom('6', 10);
	}
}

/*
* NAME: display_time_onpos
* DESCRIPTION: display the time at the right place on the screen.
*			   check for an extra 0 at the front of the hours and at the front of the minutes.
* PARAMS: Time time, int position
* RETURNS: void
*/
void display_time_onpos(Calendar time, int position)
{
	if(time.hours > 9)
	{
		char h1 = (char)(time.hours/10) + '0';
		char h2 = (char)(time.hours%10) + '0';
		show_char(h1, position);
		show_char(h2, position+1);
	}
	else
	{
		char h = (char)(time.hours) + '0';
		show_char('0', position);
		show_char(h, position+1);
	}
	
	show_char(':', position+2);
	
	if(time.minutes > 9)
	{
		char m1 = (char)(time.minutes/10) + '0';
		char m2 = (char)(time.minutes%10) + '0';
		show_char(m1, position+3);
		show_char(m2, position+4);
	}
	else
	{
		char m = (char)(time.minutes) + '0';
		show_char('0', position+3);
		show_char(m, position+4);
	}
	
	if(AM == time.am_pm)
	{
		show_char_custom('5', position+5);
	}
	else if(PM == time.am_pm)
	{
		show_char_custom('6', position+5);
	}
}

/*
* NAME: display_date
* DESCRIPTION: display the date at the right place on the screen.
*			   check for an extra 0 at the front of the day and display month in the right format
* PARAMS: Calendar date
* RETURNS: void
*/
void display_date(Calendar date)
{
	if(date.day > 9)
	{
		char d1 = (char)(date.day/10) + '0';
		char d2 = (char)(date.day%10) + '0';
		show_char(d1, 18);
		show_char(d2, 19);
	}
	else
	{
		char d = (char)(date.day) + '0';
		show_char('0', 18);
		show_char(d, 19);
	}
	
	show_char(date.month[0], 21);
	show_char(date.month[1], 22);
	show_char(date.month[2], 23);
	show_char('.', 24);
	
	int year = date.year;
	char y1 = (char)(year/1000) + '0';
	year = year%1000;
	char y2 = (char)(year/100) + '0';
	year = year%100;
	char y3 = (char)(year/10) + '0';
	char y4 = (char)(year%10) + '0';
	
	show_char(y1, 26);
	show_char(y2, 27);
	show_char(y3, 28);
	show_char(y4, 29);
}

/*
* NAME: display_string
* DESCRIPTION: display a string on the screen.
* PARAMS: char *string, int position
* RETURNS: void
*/
void display_string(char *string, int position)
{
	while(*string)
	{
		show_char(*string, position);
		string++;
		position++;
	}
}

/*
* NAME: blinking_colon
* DESCRIPTION: shows a blinking colon at a position.
* PARAMS: int position
* RETURNS: void
*/
void blinking_colon(int *counter, int position)
{
	int blinkduration = 10;
	*counter = *counter+1;
	if(*counter < blinkduration)
	{
		show_char(':',position);
	}
	else if(*counter >= blinkduration && *counter < (blinkduration*2))
	{
		show_char(' ',position);
	}
	else
	{
		*counter = 0;
	}
}

