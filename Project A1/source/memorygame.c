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
#include "memorygame.h"
#include "state_machine.h"
#include "customcharhandler.h"

int tiles[32] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
char content[32];
char gameIcons[32] = "00112233445566778899AABBCCDDEEFF";

int game_cursorPos = 0;
int first_choice = -1;
int second_choice = -1;
int couples = 0;
int fails = 0;

void memory_game_task()
{
	randomize_content();
	disable_cursor();
	memory_game_chars();
	fill_screen();
	busy = 1;
	while(busy == 1)
	{
		if(go_idle)
		{
			home_state(&currentNode);
			busy = 0;
			disable_cursor();
		}

		if(mayCheck == 1)
		{
			fill_screen();
			
			u_short pressed = get_pressed_once();
			switch(pressed)
			{
				case RAW_KEY_DOWN:
				game_cursorPos = (game_cursorPos + 16)%32;
				if(tiles[game_cursorPos] > 1)
				{
					game_cursorPos = (game_cursorPos - 16);
					if(game_cursorPos<0)
						game_cursorPos=game_cursorPos+32;
				}
				break;
				
				case RAW_KEY_UP:
				game_cursorPos = (game_cursorPos - 16);
				if(game_cursorPos<0)
					game_cursorPos=game_cursorPos+32;
				if(tiles[game_cursorPos] > 1)
				{
					game_cursorPos = (game_cursorPos + 16)%32;
				}
				break;
				
				case RAW_KEY_LEFT:
				game_cursorPos--;
				if(game_cursorPos<0)
					game_cursorPos = 31;
				while(tiles[game_cursorPos] > 1)
				{
					game_cursorPos--;
					if(game_cursorPos<0)
						game_cursorPos = 31;
				}
				break;
				
				case RAW_KEY_RIGHT:
				game_cursorPos++;
				if(game_cursorPos>31)
					game_cursorPos = 0;
				while(tiles[game_cursorPos] > 1)
				{
					game_cursorPos++;
					if(game_cursorPos>31)
						game_cursorPos = 0;
				}
				break;
				
				case RAW_KEY_ESC:
				up_state(&currentNode);
				busy = 0;
				disable_cursor();
				break;
				
				case RAW_KEY_OK:
				if(tiles[game_cursorPos] !=2)
				{
					if(couples == 16)
					{
						down_state(&currentNode);
						busy = 0;
					}
					else if(first_choice == -1)
					{
						first_choice = game_cursorPos;
						tiles[first_choice] = 10;
					}
					else if(second_choice == -1 && game_cursorPos != first_choice)
					{
						second_choice = game_cursorPos;
						tiles[second_choice] = 10;
					}
					else if (first_choice != -1 && second_choice != -1)
					{
						if(content[first_choice]==content[second_choice])
						{
							content[first_choice] = ' ';
							content[second_choice] = ' ';
							tiles[first_choice] = 2;
							tiles[second_choice] = 2;
							couples++;
							if(couples == 16)
								display_clear();
						}
						else
						{
							tiles[first_choice] = 0;
							tiles[second_choice] = 0;
							fails++;
						}
						first_choice = -1;
						second_choice = -1;
					}
				}
				break;
			}
		}
		NutSleep(10);
	}
}

void fill_screen()
{
	if(couples == 16)
	{
		char str[16];
		sprintf(str, "%d", couples+fails);
		display_string("Step count:", 0);
		display_string(str, 16);
	}
	else
	{
		int i = 0;
		for(;i<32;i++)
		{
			if(tiles[i]==10)
			{
				show_char(content[i],i);
			}
			else if(tiles[i]==2)
			{
				show_char(content[i],i);
			}
			else if(i == game_cursorPos)
			{
				show_char_custom('1',game_cursorPos);
			}
			else if(tiles[i]==-1)
			{
				show_char_custom('0',i);
			}
			else if(tiles[i]==0)
			{
				show_char_custom('0',i);
			}
			else if(tiles[i]==1)
			{
				show_char(content[i],i);
			}
		}
	}
}

void randomize_content()
{
	game_cursorPos = 0;
	first_choice = -1;
	second_choice = -1;
	couples = 0;
	fails = 0;
	int i = 0;
	for(;i<32;i++)
	{
		tiles[i] = -1;
	}
	i = 0;
	for(;i<32;i++)
	{
		int r = rand()%32;
		while(tiles[r]!=-1)
		{
			r=rand()%32;
		}
		tiles[r] = 0;
		content[i] = gameIcons[r];
	}
	printf("\n%s",content);
}
