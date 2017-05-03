#ifndef USERDATA_INC
#define USERDATA_INC

#include "schedular.h"

typedef struct
{
	int timezoneMem;
	int alarm_count;
	int alarm1;
	int rss_settings;
	Schedular_Item sch1;
	Calendar cal1;
	int alarm2;
	Schedular_Item sch2;
	Calendar cal2;
	int alarm3;
	Schedular_Item sch3;
	Calendar cal3;
	int alarm4;
	Schedular_Item sch4;
	Calendar cal4;
	int alarm5;
	Schedular_Item sch5;
	Calendar cal5;
	int alarm6;
	Schedular_Item sch6;
	Calendar cal6;
} Settings;

int initUserData(void);
int saveSettingsToFlash(Node *src);
int loadSettingsFromFlash(void);
void showPage(u_long pgn);

void testRomFs(void);

#endif
