#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "system.h"
#include "flash.h"
#include "memoryhandler.h"
#include "rtchandler.h"
#include "rss.h"

#include <fs/fs.h>
#include <dev/urom.h>
#include <fs/uromfs.h>
#include <io.h>

#define OK 1
#define NOK -1


char buffer[127]; 

// Init flash 
int initUserData(void)
{
    return At45dbInit();
}

int saveSettingsToFlash(Node *src)
{
	int result = NOK;
	Settings settings;
	settings.alarm_count = list_size();
	settings.timezoneMem = timezone;
	settings.alarm1 = 0;
	settings.alarm2 = 0;
	settings.alarm3 = 0;
	settings.alarm4 = 0;
	settings.alarm5 = 0;
	settings.alarm6 = 0;
	settings.rss_settings = rss_settings;
	if(settings.alarm_count>0)
	{
		Node * i_ptr = src;
		settings.alarm1 = 1;
		settings.cal1 = *i_ptr->schedular_item->date;
		settings.sch1 = *i_ptr->schedular_item;
		if(i_ptr->next != NULL)
		{
			i_ptr = i_ptr->next;
			settings.alarm2 = 1;
			settings.cal2 = *i_ptr->schedular_item->date;
			settings.sch2 = *i_ptr->schedular_item;
			if(i_ptr->next != NULL)
			{
				i_ptr = i_ptr->next;
				settings.alarm3 = 1;
				settings.cal3 = *i_ptr->schedular_item->date;
				settings.sch3 = *i_ptr->schedular_item;
				if(i_ptr->next != NULL)
				{
					i_ptr = i_ptr->next;
					settings.alarm4 = 1;
					settings.cal4 = *i_ptr->schedular_item->date;
					settings.sch4 = *i_ptr->schedular_item;
					if(i_ptr->next != NULL)
					{
						i_ptr = i_ptr->next;
						settings.alarm5 = 1;
						settings.cal5 = *i_ptr->schedular_item->date;
						settings.sch5 = *i_ptr->schedular_item;
						if(i_ptr->next != NULL)
						{
							i_ptr = i_ptr->next;
							settings.alarm6 = 1;
							settings.cal6 = *i_ptr->schedular_item->date;
							settings.sch6 = *i_ptr->schedular_item;
						}
					}
				}
			}
		}
	}
	unsigned char *storage = (unsigned char *) malloc(sizeof(unsigned char) * sizeof(Settings));
	if( storage != NULL )
	{
		memcpy( (unsigned char *)storage, &settings, sizeof(Settings) );
		At45dbPageWrite(0x04, (unsigned char *)storage, sizeof(Settings));

		result = OK;
	}
	free(storage);
	return result;
}

int loadSettingsFromFlash()
{
	int result = NOK;
	Settings settings;
	settings.alarm1 = 0;
	settings.alarm2 = 0;
	settings.alarm3 = 0;
	settings.alarm4 = 0;
	settings.alarm5 = 0;
	settings.alarm6 = 0;
	unsigned char *storage = (unsigned char *) malloc(sizeof(unsigned char) * sizeof(Settings));
	if( storage != NULL )
	{
		At45dbPageRead(0x04, (unsigned char *)storage, sizeof(Settings));
		memcpy( (Settings *) &settings, (unsigned char *)storage, sizeof(Settings) );
		remove_all();
		timezone = settings.timezoneMem;
		rss_settings = settings.rss_settings;
		
		if(settings.alarm_count>0)
		{
			Calendar *cal1 = malloc(sizeof(Calendar));
			*cal1 = settings.cal1;
			add(cal1, settings.sch1.stream, settings.sch1.alarm_type, settings.sch1.snoozecount);
		}
		if(settings.alarm_count>1)
		{
			Calendar *cal2 = malloc(sizeof(Calendar));
			*cal2 = settings.cal2;
			add(cal2, settings.sch2.stream, settings.sch2.alarm_type, settings.sch2.snoozecount);
		}
		if(settings.alarm_count>2)
		{
			Calendar *cal3 = malloc(sizeof(Calendar));
			*cal3 = settings.cal3;
			add(cal3, settings.sch3.stream, settings.sch3.alarm_type, settings.sch3.snoozecount);
		}
		if(settings.alarm_count>3)
		{
			Calendar *cal4 = malloc(sizeof(Calendar));
			*cal4 = settings.cal4;
			add(cal4, settings.sch4.stream, settings.sch4.alarm_type, settings.sch4.snoozecount);
		}
		if(settings.alarm_count>4)
		{
			Calendar *cal5 = malloc(sizeof(Calendar));
			*cal5 = settings.cal5;
			add(cal5, settings.sch5.stream, settings.sch5.alarm_type, settings.sch5.snoozecount);
		}
		if(settings.alarm_count>5)
		{
			Calendar *cal6 = malloc(sizeof(Calendar));
			*cal6 = settings.cal6;
			add(cal6, settings.sch6.stream, settings.sch6.alarm_type, settings.sch6.snoozecount);
		}
		
		result = OK;
	}
	free(storage);
	return result;
}
