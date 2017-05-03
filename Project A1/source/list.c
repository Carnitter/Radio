/*	NAME: list.c
	DESCRIPTION: Combines items into a list. You can scroll through the list and get the selected item.
	AUTHORS: group A1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

Item *current_item_ptr;

 /*
	NAME: init_list
	DESCRIPTION: Initilizes the list with the first item.
	PARAMS: char * text_to_add, pointer to the first string.
 */
void init_list(char * text_to_add)
{
	Item *i = malloc(sizeof(Item));
	strcpy(i->text, text_to_add);
	current_item_ptr = i;
	i-> previous = NULL;
	i-> next = NULL;
}

 /*
	NAME: add_to_list
	DESCRIPTION: Adds an item to a list after the list has been initilized.
	PARAMS: char * text_to_add, pointer to the string to add.
 */
void add_to_list(char * text_to_add)
{
	Item *i = malloc(sizeof(Item));
	Item *last_ptr = current_item_ptr;
	if(i != NULL)
	{
		while (last_ptr->next) last_ptr = last_ptr->next;
		last_ptr->next = i;
		strcpy(i->text, text_to_add);
		i->previous = last_ptr;
		i->next = NULL;
	}
}

 /*
	NAME: get_current_item
	DESCRIPTION: returns the selected item from the list.
	RETURNS: char * to the current selected items text.
 */
Item * get_selected_item()
{
	return current_item_ptr;
}

 /*
	NAME: get_selected
	DESCRIPTION: returns the selected item from the list.
	RETURNS: char * to the current selected items text.
 */
char * get_selected()
{
	return current_item_ptr->text;
}

 /*
	NAME: next_item
	DESCRIPTION: If the current item has a next item this one is selected as the current item.
 */
void next_item()
{
	Item *i = current_item_ptr;
	if(i->next)
	{
		current_item_ptr = i->next;
	}
}

 /*
	NAME: previous_item
	DESCRIPTION: If the current item has a previous item this one is selected as the current item.
 */
void previous_item()
{
	Item *i = current_item_ptr;
	if(i->previous)
	{
		current_item_ptr = i->previous;
	}
}

