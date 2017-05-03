
#ifndef rtchandler_made
#define rtchandler_made

#include "structs.h"

int timezone;

int rtc_init(void);
Calendar* rtc_get_time(void);
Calendar* rtc_get_alarm1(void);
Calendar* rtc_get_alarm2(void);
int rtc_set_time(Calendar);
int rtc_set_alarm1(Calendar);
int rtc_set_alarm2(Calendar);
char* get_month(int);
int get_month_int(char* month);
void start_time_synchronizing(void);
void synchronize(void);


#endif
