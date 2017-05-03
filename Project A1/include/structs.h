/*
* NAME: enum am_pm ;
* DESCRIPTION: enum which contains AM or PM.
*/
#ifndef RICKMOETGEENHFILESMAKEN
#define RICKMOETGEENHFILESMAKEN
#include "uxml.h"
enum am_pm_enum {AM, PM};

typedef struct
{
	int year;
	char month[10];
	int day;
	int hours;
	int minutes;
	enum am_pm_enum am_pm;
	int day_of_week;
}Calendar;
#endif
