/*
* NAME: rtchandler
* DESCRIPTION: .c file which has functions to write and read data to or from the RTC-chip
* AUTHORS: Group A1
*/

#include <stdio.h>
#include <string.h>

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/version.h>
#include <dev/irqreg.h>
#include <pro/dhcp.h>
#include <arpa/inet.h>
#include <net/route.h>
#include "log.h"
#include "x1205.h"
#include "rtchandler.h"
#include "structs.h"
#include <pro/sntp.h>
#include <time.h>
#include <sys/mutex.h>

#define LOG_MODULE  LOG_X1205_MODULE

//local variables
time_t ntp_time = 0;
tm *ntp_datetime;
uint32_t timeserver = 0;

MUTEX mutex;

/*
* NAME: rtc_init
* DESCRIPTION: function to initialize and enable the rtc chip
* PARAMS: NONE
* RETURNS: int
*/
int rtc_init(void)
{
	x1205Init();
	NutMutexInit(&mutex);
	NutSleep(100);
	x1205Enable();
	return 1;
}

/*
* NAME: rtc_get_time
* DESCRIPTION: function to get the time on the rtc chip in a calendar format
* PARAMS: NONE
* RETURNS: Calendar struct with the stored time on the rtc
*/
Calendar* rtc_get_time(void)
{
	u_char buf[24];
	x1205ReadNByte(0x30, buf, 8 );
	Calendar* calptr;
	Calendar cal;
	calptr = &cal;
	cal.year = BCD2DEC(buf[7]);
	cal.day_of_week = BCD2DEC(buf[6]);
	cal.year = (cal.year * 100) + BCD2DEC(buf[5]);
	strcpy(cal.month, get_month(BCD2DEC(buf[4])));
	cal.day = BCD2DEC(buf[3]);
	cal.hours = BCD2DEC(buf[2]);
	if(cal.hours > 12)
	{
		cal.hours = cal.hours -12;
		cal.am_pm = PM;
	}
	else
	{
		cal.am_pm = AM;
	}
	cal.minutes = BCD2DEC(buf[1]);
	NutSleep(10);
	return calptr;
	
}

/*
* NAME: rtc_get_alarm1
* DESCRIPTION: function to get the time for alarm1 on the rtc chip in a calendar format
* PARAMS: NONE
* RETURNS: Calendar struct with the stored time on the rtc
*/
Calendar* rtc_get_alarm1(void)
{
	u_char buf[24];
	x1205ReadNByte(0x00, buf, 8 );
	Calendar* calptr;
	Calendar cal;
	calptr = &cal;
	cal.year = BCD2DEC(buf[7]);
	cal.day_of_week = BCD2DEC(buf[6]);
	cal.year = (cal.year * 100) + BCD2DEC(buf[5]);
	strcpy(cal.month, get_month(BCD2DEC(buf[4])));
	cal.day = BCD2DEC(buf[3]);
	cal.hours = BCD2DEC(buf[2]);
	if(cal.hours > 12)
	{
		cal.hours = cal.hours -12;
		cal.am_pm = PM;
	}
	else
	{
		cal.am_pm = AM;
	}
	cal.minutes = BCD2DEC(buf[1]);
	NutSleep(10);
	return calptr;
}

/*
* NAME: rtc_get_alarm2
* DESCRIPTION: function to get the time for alarm2 on the rtc chip in a calendar format
* PARAMS: NONE
* RETURNS: Calendar struct with the stored time on the rtc
*/
Calendar* rtc_get_alarm2(void)
{
	u_char buf[24];
	x1205ReadNByte(0x08, buf, 8 );
	Calendar* calptr;
	Calendar cal;
	calptr = &cal;
	cal.year = BCD2DEC(buf[7]);
	cal.day_of_week = BCD2DEC(buf[6]);
	cal.year = (cal.year * 100) + BCD2DEC(buf[5]);
	strcpy(cal.month, get_month(BCD2DEC(buf[4])));
	cal.day = BCD2DEC(buf[3]);
	cal.hours = BCD2DEC(buf[2]);
	if(cal.hours > 12)
	{
		cal.hours = cal.hours -12;
		cal.am_pm = PM;
	}
	else
	{
		cal.am_pm = AM;
	}
	cal.minutes = BCD2DEC(buf[1]);
	NutSleep(10);
	return calptr;
}

/*
* NAME: rtc_set_time
* DESCRIPTION: function to set the time on the rtc chip
* PARAMS: Calendar struct with the time and date to set the rtc
* RETURNS: int
*/
int rtc_set_time(Calendar cal)
{
	u_char buf[24];
	int year1 = cal.year / 100;
	int year2 = cal.year % 2000;
	buf[7] = DEC2BCD(year1);	// Y2K
	buf[6] = DEC2BCD(cal.day_of_week);	// DW
	buf[5] = DEC2BCD(year2);	// YR
	buf[4] = DEC2BCD(get_month_int(cal.month)); 	// MO
	buf[3] = DEC2BCD(cal.day); 	// DT
	buf[2] = DEC2BCD(cal.hours);	// HR
	buf[1] = DEC2BCD(cal.minutes);	// MN
	buf[0] = DEC2BCD(0); 	// SC
	x1205WriteNBytes(0x30, buf, 8);
	return 1;
}

/*
* NAME: rtc_set_alarm1
* DESCRIPTION: function to set the time for alarm1
* PARAMS: Calendar struct with the time and date to set the rtc
* RETURNS: int
*/
int rtc_set_alarm1(Calendar cal)
{
	u_char buf[24];
	int year1 = cal.year / 100;
	int year2 = cal.year % 2000;
	buf[7] = DEC2BCD(year1);	// Y2K
	buf[6] = DEC2BCD(cal.day_of_week);	// DW
	buf[5] = DEC2BCD(year2);	// YR
	buf[4] = DEC2BCD(get_month_int(cal.month)); 	// MO
	buf[3] = DEC2BCD(cal.day); 	// DT
	buf[2] = DEC2BCD(cal.hours);	// HR
	buf[1] = DEC2BCD(cal.minutes);	// MN
	buf[0] = DEC2BCD(0); 	// SC
	x1205WriteNBytes(0x00, buf, 8);
	return 1;
}

/*
* NAME: rtc_set_alarm2
* DESCRIPTION: function to get the time on the rtc chip in a calendar format
* PARAMS: Calendar struct with the time and date to set the rtc
* RETURNS: Calendar struct with the stored time on the rtc
*/
int rtc_set_alarm2(Calendar cal)
{
	u_char buf[24];
	int year1 = cal.year / 100;
	int year2 = cal.year % 2000;
	buf[7] = DEC2BCD(year1);	// Y2K
	buf[6] = DEC2BCD(cal.day_of_week);	// DW
	buf[5] = DEC2BCD(year2);	// YR
	buf[4] = DEC2BCD(get_month_int(cal.month)); 	// MO
	buf[3] = DEC2BCD(cal.day); 	// DT
	buf[2] = DEC2BCD(cal.hours);	// HR
	buf[1] = DEC2BCD(cal.minutes);	// MN
	buf[0] = DEC2BCD(0); 	// SC
	x1205WriteNBytes(0x08, buf, 8);
	return 1;
}

/*
* NAME: get_month
* DESCRIPTION: Function to convert a digit month to a char month
*			   example: 1 -> January
* PARAMS: int month
* RETURNS: pointer to string which has the name of the month stored
*/
char* get_month(int month)
{
	switch(month)
	{
		case 1:
		return "January";
		break;
		case 2:
		return "February";
		break;
		case 3:
		return "March";
		break;
		case 4:
		return "April";
		break;
		case 5:
		return "May";
		break;
		case 6:
		return "June";
		break;
		case 7:
		return "July";
		break;
		case 8:
		return "August";
		break;
		case 9:
		return "September";
		break;
		case 10:
		return "October";
		break;
		case 11:
		return "November";
		break;
		case 12:
		return "December";
		break;
		default:
		return "Error";
		break;
	}
}

/*
* NAME: get_month_int
* DESCRIPTION: function to convert string of a month to a digit month
* PARAMS: Name of the month
* RETURNS: digit of the month
*/
int get_month_int(char* month)
{
	if(strcmp(month, "January") == 0)	{return 1;}
	if(strcmp(month, "February") == 0)	{return 2;}
	if(strcmp(month, "March") == 0)		{return 3;}
	if(strcmp(month, "April") == 0)		{return 4;}
	if(strcmp(month, "May") == 0)		{return 5;}
	if(strcmp(month, "June") == 0)		{return 6;}
	if(strcmp(month, "July") == 0)		{return 7;}
	if(strcmp(month, "August") == 0)	{return 8;}
	if(strcmp(month, "September") == 0) {return 9;}
	if(strcmp(month, "October") == 0)	{return 10;}
	if(strcmp(month, "November") == 0)	{return 11;}
	if(strcmp(month, "December") == 0)	{return 12;}
	return 0;
}

/*
* NAME: synhronize_time_thread
* DESCRIPTION: thread which synchronizes rtc time with ntp time every hour
* PARAMS: Name of the month
* RETURNS: digit of the month
*/
THREAD(synhronize_time_thread, arg)
{
	NutThreadSetPriority(128);
	
	while(1)
	{
			synchronize();
			NutSleep(3600000);
	}
}


/*
* NAME: start_time_synchronizing
* DESCRIPTION: method to create a thread, which synchronizes rtc time with ntp time
* PARAMS: None
* RETURNS: void
*/
void start_time_synchronizing(void)
{
	NutThreadCreate("time thread", synhronize_time_thread, 0, 512);
}

/*
* NAME: synchronize
* DESCRIPTION: function to get the accurate time from a ntp server
*				convert the fetched time to a calendar struct
*				write the converted calendar struct to the rtc chip
* PARAMS: None
* RETURNS: void
*/
void synchronize(void)
{
	//todo timezone from memory	
	_timezone = timezone*60*60;
	
	timeserver = inet_addr("85.214.216.1");
	NutSNTPGetTime(&timeserver, &ntp_time);
	
	LogMsg_P(LOG_INFO, PSTR("synchronizing time"));
	if(NutSNTPGetTime(&timeserver, &ntp_time) == 0)
	{
		ntp_datetime = localtime(&ntp_time);
				 
		Calendar cal;
		cal.year = ntp_datetime->tm_year+1900;	
		strcpy(cal.month, get_month(ntp_datetime->tm_mon+1)); 
		cal.day = ntp_datetime->tm_mday;
		cal.minutes = ntp_datetime->tm_min;
		cal.day_of_week = ntp_datetime->tm_wday;
		cal.hours = ntp_datetime->tm_hour;
		cal.am_pm = 0;
		
		LogMsg_P(LOG_INFO, PSTR("SET: date: %i-%s-%i \n time: %i:%i AM_PM:%i"), cal.day, cal.month, cal.year, cal.hours, cal.minutes, cal.am_pm);
		rtc_set_time(cal);
		cal  = * rtc_get_time();
		LogMsg_P(LOG_INFO, PSTR("RECIEVED: date: %i-%s-%i \n time: %i:%i AM_PM:%i"), cal.day, cal.month, cal.year, cal.hours, cal.minutes, cal.am_pm);
	}
	else
	{
		NutSleep(500);
		LogMsg_P(LOG_ERR, PSTR("Error: synchronize()"));
	}
	
	
	
}


