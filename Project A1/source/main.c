/*! \mainpage SIR firmware documentation
 *
 *  \section intro Introduction
 *  A collection of HTML-files has been generated using the documentation in the sourcefiles to
 *  allow the developer to browse through the technical documentation of this project.
 *  \par
 *  \note these HTML files are automatically generated (using DoxyGen) and all modifications in the
 *  documentation should be done via the sourcefiles.
 */

/*! \file
 *  COPYRIGHT (C) STREAMIT BV 2010
 *  \date 19 december 2003
 */
 
 
 

#define LOG_MODULE  LOG_MAIN_MODULE

/*--------------------------------------------------------------------------*/
/*  Include files                                                           */
/*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/version.h>
#include <dev/irqreg.h>

#include "system.h"
#include "portio.h"
#include "display.h"
#include "remcon.h"
#include "keyboard.h"
#include "keyboardhandler.h"
#include "led.h"
#include "log.h"
#include "uart0driver.h"
#include "mmc.h"
#include "watchdog.h"
#include "flash.h"
#include "spidrv.h"
#include "displayhandler.h"
#include "list.h"
#include "alarmmenu.h"
#include "rtchandler.h"
#include "idle.h"
#include "uxml.h"

#include "internet_connector.h"

#include "schedular.h"
#include "structs.h"


#include <time.h>
#include "rtc.h"
#include "state_machine.h"
#include "rtchandler.h"
#include "rss.h"
#include "memoryhandler.h"

/*-------------------------------------------------------------------------*/
/* global variable definitions                                             */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* local variable definitions                                              */
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/* local routines (prototyping)                                            */
/*-------------------------------------------------------------------------*/
static void SysMainBeatInterrupt(void*);
static void SysControlMainBeat(u_char);

/*-------------------------------------------------------------------------*/
/* Stack check variables placed in .noinit section                         */
/*-------------------------------------------------------------------------*/

/*!
 * \addtogroup System
 */

/*@{*/


/*-------------------------------------------------------------------------*/
/*                         start of code                                   */
/*-------------------------------------------------------------------------*/


/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
/*!
 * \brief ISR MainBeat Timer Interrupt (Timer 2 for Mega128, Timer 0 for Mega256).
 *
 * This routine is automatically called during system
 * initialization.
 *
 * resolution of this Timer ISR is 4,448 msecs
 *
 * \param *p not used (might be used to pass parms from the ISR)
 */
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
static void SysMainBeatInterrupt(void *p)
{

    /*
     *  scan for valid keys AND check if a MMCard is inserted or removed
     */
    KbScan();
    CardCheckCard();
}


/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
/*!
 * \brief Initialise Digital IO
 *  init inputs to '0', outputs to '1' (DDRxn='0' or '1')
 *
 *  Pull-ups are enabled when the pin is set to input (DDRxn='0') and then a '1'
 *  is written to the pin (PORTxn='1')
 */
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
void SysInitIO(void)
{
    /*
     *  Port B:     VS1011, MMC CS/WP, SPI
     *  output:     all, except b3 (SPI Master In)
     *  input:      SPI Master In
     *  pull-up:    none
     */
    outp(0xF7, DDRB);

    /*
     *  Port C:     Address bus
     */

    /*
     *  Port D:     LCD_data, Keypad Col 2 & Col 3, SDA & SCL (TWI)
     *  output:     Keyboard colums 2 & 3
     *  input:      LCD_data, SDA, SCL (TWI)
     *  pull-up:    LCD_data, SDA & SCL
     */
    outp(0x0C, DDRD);
    outp((inp(PORTD) & 0x0C) | 0xF3, PORTD);

    /*
     *  Port E:     CS Flash, VS1011 (DREQ), RTL8019, LCD BL/Enable, IR, USB Rx/Tx
     *  output:     CS Flash, LCD BL/Enable, USB Tx
     *  input:      VS1011 (DREQ), RTL8019, IR
     *  pull-up:    USB Rx
     */
    outp(0x8E, DDRE);
    outp((inp(PORTE) & 0x8E) | 0x01, PORTE);

    /*
     *  Port F:     Keyboard_Rows, JTAG-connector, LED, LCD RS/RW, MCC-detect
     *  output:     LCD RS/RW, LED
     *  input:      Keyboard_Rows, MCC-detect
     *  pull-up:    Keyboard_Rows, MCC-detect
     *  note:       Key row 0 & 1 are shared with JTAG TCK/TMS. Cannot be used concurrent
     */
#ifndef USE_JTAG
    sbi(JTAG_REG, JTD); // disable JTAG interface to be able to use all key-rows
    sbi(JTAG_REG, JTD); // do it 2 times - according to requirements ATMEGA128 datasheet: see page 256
#endif //USE_JTAG

    outp(0x0E, DDRF);
    outp((inp(PORTF) & 0x0E) | 0xF1, PORTF);

    /*
     *  Port G:     Keyboard_cols, Bus_control
     *  output:     Keyboard_cols
     *  input:      Bus Control (internal control)
     *  pull-up:    none
     */
    outp(0x18, DDRG);
}

/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
/*!
 * \brief Starts or stops the 4.44 msec mainbeat of the system
 * \param OnOff indicates if the mainbeat needs to start or to stop
 */
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
static void SysControlMainBeat(u_char OnOff)
{
    int nError = 0;

    if (OnOff==ON)
    {
        nError = NutRegisterIrqHandler(&OVERFLOW_SIGNAL, SysMainBeatInterrupt, NULL);
        if (nError == 0)
        {
            init_8_bit_timer();
        }
    }
    else
    {
        // disable overflow interrupt
        disable_8_bit_timer_ovfl_int();
    }
}

/*
* NAME: Thread alarmCheck;
* DESCRIPTION: Thread to check if an alarm should trigger
*/
THREAD(AlarmCheck, arg) 
{
	Calendar* time;
	//time = rtc_get_time();
	//int minutes = time->minutes;
	int newMinutes = 0;
	for(;;)
	{
		time = rtc_get_time();
		int minutes = time->minutes;
		if(minutes != newMinutes )
		{
			synchronize();
			LogMsg_P(LOG_INFO, PSTR("check"));
			check();
			newMinutes = minutes;
		}
		NutSleep(10);
	}
}

/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
/*!
 * \brief Main entry of the SIR firmware
 *
 * All the initialisations before entering the for(;;) loop are done BEFORE
 * the first key is ever pressed. So when entering the Setup (POWER + VOLMIN) some
 * initialisatons need to be done again when leaving the Setup because new values
 * might be current now
 *
 * \return \b never returns
 */
/* อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ */
int main(void)
{
	/* 
	 * Kroeske: time struct uit nut/os time.h (http://www.ethernut.de/api/time_8h-source.html)
	 *
	 */
	tm gmt;
	/*
	 * Kroeske: Ook kan 'struct _tm gmt' Zie bovenstaande link
	 */
	
    /*
     *  First disable the watchdog
     */
    WatchDogDisable();

    NutDelay(100);

    SysInitIO();
	
	SPIinit();
    
	LedInit();
	
	LcdLowLevelInit();

    Uart0DriverInit();
    Uart0DriverStart();
	LogInit();
	

	
	//Flash Memory init
	initUserData();
	
	LogMsg_P(LOG_INFO, PSTR("Greetings User"));
	loadSettingsFromFlash();
    CardInit();
	/*
	 * Kroeske: sources in rtc.c en rtc.h
	 */
    X12Init();
    if (X12RtcGetClock(&gmt) == 0)
    {
		LogMsg_P(LOG_INFO, PSTR("RTC time [%02d:%02d:%02d]"), gmt.tm_hour, gmt.tm_min, gmt.tm_sec );
    }

    if (At45dbInit()==AT45DB041B){}

    RcInit();    
	KbInit();	
    SysControlMainBeat(ON);             // enable 4.4 msecs hartbeat interrupt

    /*
     * Increase our priority so we can feed the watchdog.
     */
    //NutThreadSetPriority(1);

	/* Enable global interrupts */
	sei();
	
	NutDelay(100);
	
	LcdBackLight(LCD_BACKLIGHT_ON);
	
	internet_result = initInet();
	if( 1 != internet_result )
	{
		LogMsg_P(LOG_ERR, PSTR("initInetstream() = NOK, NO netwORK!"));
 	}

	// IF( 1 == CONNECTTOSTREAM("178.254.19.116", 6100) )
	// {
		// playStream();
	// }
	
	schedular_init();
	
	rtc_init();
	//start thread to get accurate time every hour ---not needed anymore---
	//start_time_synchronizing();
	
	NutDelay(100);
	start_checking_input();
	srand(time(NULL));   // should only be called once
	if(get_pressed() == RAW_KEY_ALT)
	{
		//display_clear();
		//	display_string("Clear memory", 0);
		timezone = -999;
		NutThreadCreate("alarmCheck", AlarmCheck, 0, 512);
		build_menu_structure();
	}
	else
	{
		NutThreadCreate("alarmCheck", AlarmCheck, 0, 512);
		build_menu_structure();
	}
	
	
	

	int t = 0;
    for (;;)
    {	
        NutSleep(100);
		if( !((t++)%15) )
		{
			LedControl(LED_TOGGLE);
		
			WatchDogRestart();
		}
    }
	
    return(0);      // never reached, but 'main()' returns a non-void, so.....
}
/* ---------- end of module ------------------------------------------------ */

/*@}*/
