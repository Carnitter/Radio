/*	NAME: list.h
	DESCRIPTION: Combines items into a list. You can scroll through the list and get the selected item.
	AUTHORS: group A1
 */

typedef struct Item
{
	struct Item *next;
	struct Item *previous;
	char text[30];
}Item;

void init_list(char * text_to_add);
void add_to_list(char * text_to_add);
char * get_selected(void);
Item * get_selected_item(void);
void next_item(void);
void previous_item(void);
