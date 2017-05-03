#include "structs.h"

/*
* NAME: displayhandler
* DESCRIPTION: link between the display and the upper layer source files
* AUTHORS: Group A1
*/

int blink_counter;

extern void show_char(char character, int position);
extern void show_char_custom(char character, int position);
extern void display_clear(void);
extern void display_time(Calendar);
extern void display_date(Calendar);
extern void display_string(char *,int);
extern void display_time_onpos(Calendar, int);
extern void blinking_colon(int *counter, int position);


