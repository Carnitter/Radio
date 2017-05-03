#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <sys/thread.h>
#include <sys/heap.h>

#include "system.h"
#include "portio.h"
#include "display.h"
#include "keyboard.h"
#include "log.h"
#include "customcharhandler.h"

/*-------------------------------------------------------------------------*/
/* local defines                                                           */
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/* local variable definitions                                              */
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/* local routines (prototyping)                                            */
/*-------------------------------------------------------------------------*/
static void LcdWriteByte(u_char, u_char);
static void LcdWriteNibble(u_char, u_char);
static void LcdWaitBusy(void);
/*@{*/ 

/*-------------------------------------------------------------------------*/
/*                         start of code                                   */
/*-------------------------------------------------------------------------*/

void alarm_task_daily_chars(void)
{
	char dailyIcons[64] = { 
 0b00011,0b01101,0b01110,0b01110,0b01110,0b01101,0b00011,0b11111, //D - inverted
 0b10001,0b01110,0b01110,0b01110,0b00000,0b01110,0b01110,0b11111, //A - inverted
 0b10001,0b11011,0b11011,0b11011,0b11011,0b11011,0b10001,0b11111, //I - inverted
 0b01111,0b01111,0b01111,0b01111,0b01111,0b01111,0b00000,0b11111, //L - inverted
 0b01110,0b01110,0b01110,0b10101,0b11011,0b11011,0b11011,0b11111, //Y - inverted
 0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111, //Filled in
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, //EMPTY
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000  //EMPTY	
	};
 push_custom_glyph_to_ram(dailyIcons,64);
}

void alarm_task_spec_chars(void)
{
	char specIcons[64] = {
 0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111, //Filled in
 0b10000,0b01111,0b01111,0b10001,0b11110,0b11110,0b00001,0b11111, //S - inverted
 0b00001,0b01110,0b01110,0b00001,0b01111,0b01111,0b01111,0b11111, //P - inverted
 0b00000,0b01111,0b01111,0b00001,0b01111,0b01111,0b00000,0b11111, //E - inverted
 0b10001,0b01110,0b01111,0b01111,0b01111,0b01110,0b10001,0b11111, //C - inverted
 0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111, //Filled in
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, //EMPTY
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000  //EMPTY
	};
 push_custom_glyph_to_ram(specIcons,64);
}

void alarm_task_week_chars(void)
{
	char weekIcons[64] = {
 0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111, //Filled in
 0b01110,0b01110,0b01110,0b01010,0b01010,0b01010,0b10101,0b11111, //W - inverted
 0b00000,0b01111,0b01111,0b00001,0b01111,0b01111,0b00000,0b11111, //E - inverted
 0b00000,0b01111,0b01111,0b00001,0b01111,0b01111,0b00000,0b11111, //E - inverted
 0b01110,0b01101,0b01011,0b00111,0b01011,0b01101,0b01110,0b11111, //K - inverted
 0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111, //Filled in
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, //EMPTY
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000  //EMPTY
	};
 push_custom_glyph_to_ram(weekIcons,64);
}

void stream_task_chars(void)
{
	char weekIcons[64] = {
 0b00000,0b11111,0b11111,0b11111,0b11111,0b11111,0b00000,0b00000, //STOP
 0b11111,0b00000,0b00000,0b00000,0b00000,0b00000,0b11111,0b11111, //STOP - inverted
 0b00000,0b01000,0b01100,0b01110,0b01100,0b01000,0b00000,0b00000, //PLAY
 0b11111,0b10111,0b10011,0b10001,0b10011,0b10111,0b11111,0b11111, //PLAY - inverted
 0b00000,0b11011,0b11011,0b11011,0b11011,0b11011,0b00000,0b00000, //PAUSE
 0b11111,0b00100,0b00100,0b00100,0b00100,0b00100,0b11111,0b11111, //PAUSE - inverted
 0b00100,0b01110,0b11111,0b00000,0b00000,0b11111,0b01110,0b00100, //Vertical Scroll Arrowss
 0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111  //Filled
	};
 push_custom_glyph_to_ram(weekIcons,64);
}

void default_custom_chars(void)
{
	char idleIcons[64] = {
 0b00100,0b01110,0b01110,0b01110,0b11111,0b00000,0b00100,0b00000, //Alarm is ON
 0b00000,0b00000,0b00001,0b00011,0b00111,0b01111,0b11111,0b00000, //Ethernet Connection Present
 0b00100,0b01110,0b11111,0b00000,0b00000,0b11111,0b01110,0b00100, //Vertical Scroll Arrows
 0b00000,0b00100,0b01100,0b11111,0b01100,0b00100,0b00000,0b00000, //Arrow Left
 0b00000,0b00100,0b00110,0b11111,0b00110,0b00100,0b00000,0b00000, //Arrow Right
 0b01110,0b10001,0b11111,0b10001,0b00000,0b11011,0b10101,0b10001, //AM Icon
 0b11110,0b10001,0b11110,0b10000,0b00000,0b11011,0b10101,0b10001, //PM Icon
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000  //EMPTY
	};
 push_custom_glyph_to_ram(idleIcons,64);
}

void weather_custom_chars(void)
{
	char weatherIcons[64] = {
 0b00011,0b01111,0b11111,0b11111,0b01111,0b00000,0b00000,0b00000, //Cloud left
 0b10000,0b11100,0b11110,0b11111,0b11111,0b00000,0b00000,0b00000, //Cloud right
 0b00011,0b01110,0b11100,0b11111,0b01110,0b00000,0b00000,0b00000, //Cloud Thunder left
 0b10000,0b11100,0b00110,0b01111,0b11111,0b00000,0b00000,0b00000, //Cloud Thunder right
 0b10000,0b00011,0b00100,0b00100,0b00011,0b00000,0b00000,0b00000, //graden celcius
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, 
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, 
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000 
	};
 push_custom_glyph_to_ram(weatherIcons,64);
}

// rain chars
void rain_chars(void)
{
	char idleIcons[64] = {
 0b00011,0b00111,0b11111,0b11111,0b01111,0b00100,0b00000,0b00000, //left fase 0
 0b10000,0b11100,0b11110,0b11111,0b11111,0b10010,0b00000,0b00000, //right fase 0
 0b00011,0b00111,0b11111,0b11111,0b01111,0b00100,0b01001,0b00000, //left fase 1
 0b10000,0b11100,0b11110,0b11111,0b11111,0b10010,0b00100,0b00000, //right fase 1
 0b00011,0b00111,0b11111,0b11111,0b01111,0b00000,0b01001,0b00000, //left fase 2
 0b10000,0b11100,0b11110,0b11111,0b11111,0b00000,0b00100,0b00000, //right fase 2
 0b00011,0b00111,0b11111,0b11111,0b01111,0b00000,0b00000,0b00000, //left fase 3
 0b10000,0b11100,0b11110,0b11111,0b11111,0b00000,0b00000,0b00000  //right fase 3
	};
 push_custom_glyph_to_ram(idleIcons,64);
}

//sun chars
void sun_chars(void)
{
	char weatherIcons[64] = {
 0b00000,0b00001,0b00011,0b00111,0b00011,0b00001,0b00000,0b00000, //left fase 0
 0b00000,0b10000,0b11000,0b11100,0b11000,0b10000,0b00000,0b00000, //right fase 0
 0b00000,0b00101,0b00011,0b01111,0b00011,0b00101,0b00001,0b00000, //left fase 1
 0b00000,0b10100,0b11000,0b11110,0b11000,0b10100,0b10000,0b00000, //right fase 1
 0b01000,0b00101,0b00011,0b11111,0b00011,0b00101,0b01001,0b00001, //left fase 2
 0b00010,0b10100,0b11000,0b11111,0b11000,0b10100,0b10010,0b10000, //right fase 2
 0b01000,0b00001,0b00011,0b10111,0b00011,0b00001,0b01000,0b00001, //left fase 3
 0b00010,0b10000,0b11000,0b11101,0b11000,0b10000,0b00010,0b10000  //right fase 3
	};
 push_custom_glyph_to_ram(weatherIcons,64);
}

void memory_game_chars(void)
{
	char memoryGameIcons[64] = {
 0b11111,0b10001,0b10001,0b10001,0b10001,0b10001,0b10001,0b11111, //[ ]
 0b11111,0b10001,0b10101,0b10101,0b10101,0b10101,0b10001,0b11111, //[|]
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, //EMPTY
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, //EMPTY
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, //EMPTY
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, //EMPTY
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000, //EMPTY
 0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000,0b00000 //EMPTY
	};
 push_custom_glyph_to_ram(memoryGameIcons,64);
}

/* ??????????????????????????????????????????????????????????????????????? */
/*!
 * \brief Write a single custom character on the LCD
 *
 * Writes a single custom character on the LCD on the current cursor position
 *
 * \param number the index of the custom character to write.
 */
/* ??????????????????????????????????????????????????????????????????????? */
void LcdCustomChar(char number)
{
	int customCharIndex = number - '0';			//Converts the (char)number into a position in the custom glyph memory
    LcdWriteByte(WRITE_DATA, customCharIndex);
}


/* ??????????????????????????????????????????????????????????????????????? */
/*!
 * \brief Function to push the custom chars to the memory.
 * 
 * This funciton pushes the char *glyph for a given int length to the ram
 * Basically, an array of 64 items, with each item existing of a value between
 * 0b00000 and 0b11111. Since 1 char is 8 rows 5 colums, each item indicates which
 * pixels in each row shoulw be on (1) or off (0). Items 0 to 7 items create custom
 * glyph index 0, items 8 to 15 create custom char 8 to 15 etc.
 */
/* ??????????????????????????????????????????????????????????????????????? */
void push_custom_glyph_to_ram(char *glyph, int length)
{
	//Select the custom glyph ram
	LcdWriteByte(WRITE_COMMAND, 0x40);
	
	int count;
	
	//Write customIcons array that contains the glyphs
	for(count = 0; count < length; count++)
	{
		LcdChar(glyph[count]);
		NutSleep(1);
	}
	
	//Go back to display RAM
	LcdWriteByte(WRITE_COMMAND, 0x80);
	NutDelay(5);
}

/* ??????????????????????????????????????????????????????????????????????? */
/*!
 * \brief Low-level routine to write a byte to LCD-controller
 *
 * Writes one byte to the LCD-controller (by  calling LcdWriteNibble twice)
 * CtrlState determines if the byte is written to the instruction register
 * or to the data register.
 *
 * \param CtrlState destination: instruction or data
 * \param LcdByte byte to write
 *
 */
/* ??????????????????????????????????????????????????????????????????????? */
static void LcdWriteByte(u_char CtrlState, u_char LcdByte)
{
    LcdWaitBusy();                      // see if the controller is ready to receive next byte
    LcdWriteNibble(CtrlState, LcdByte & 0xF0);
    LcdWriteNibble(CtrlState, LcdByte << 4);

}

/* ??????????????????????????????????????????????????????????????????????? */
/*!
 * \brief Low-level routine to write a nibble to LCD-controller
 *
 * Writes a nibble to the LCD-controller (interface is a 4-bit databus, so
 * only 4 databits can be send at once).
 * The nibble to write is in the upper 4 bits of LcdNibble
 *
 * \param CtrlState destination: instruction or data
 * \param LcdNibble nibble to write (upper 4 bits in this byte
 *
 */
/* ??????????????????????????????????????????????????????????????????????? */
static void LcdWriteNibble(u_char CtrlState, u_char LcdNibble)
{
    outp((inp(LCD_DATA_DDR) & 0x0F) | 0xF0, LCD_DATA_DDR);  // set data-port to output again

    outp((inp(LCD_DATA_PORT) & 0x0F) | (LcdNibble & 0xF0), LCD_DATA_PORT); // prepare databus with nibble to write

    if (CtrlState == WRITE_COMMAND)
    {
        cbi(LCD_RS_PORT, LCD_RS);     // command: RS low
    }
    else
    {
        sbi(LCD_RS_PORT, LCD_RS);     // data: RS high
    }

    sbi(LCD_EN_PORT, LCD_EN);

    asm("nop\n\tnop");                    // small delay

    cbi(LCD_EN_PORT, LCD_EN);
    cbi(LCD_RS_PORT, LCD_RS);
    outp((inp(LCD_DATA_DDR) & 0x0F), LCD_DATA_DDR);           // set upper 4-bits of data-port to input
    outp((inp(LCD_DATA_PORT) & 0x0F) | 0xF0, LCD_DATA_PORT);  // enable pull-ups in data-port
}

/* ??????????????????????????????????????????????????????????????????????? */
/*!
 * \brief Low-level routine to see if the controller is ready to receive
 *
 * This routine repeatetly reads the databus and checks if the highest bit (bit 7)
 * has become '0'. If a '0' is detected on bit 7 the function returns.
 *
 */
/* ??????????????????????????????????????????????????????????????????????? */
static void LcdWaitBusy()
{
    u_char Busy = 1;
	u_char LcdStatus = 0;

    cbi (LCD_RS_PORT, LCD_RS);              // select instruction register

    sbi (LCD_RW_PORT, LCD_RW);              // we are going to read

    while (Busy)
    {
        sbi (LCD_EN_PORT, LCD_EN);          // set 'enable' to catch 'Ready'

        asm("nop\n\tnop");                  // small delay
        LcdStatus =  inp(LCD_IN_PORT);      // LcdStatus is used elsewhere in this module as well
        Busy = LcdStatus & 0x80;            // break out of while-loop cause we are ready (b7='0')
    }

    cbi (LCD_EN_PORT, LCD_EN);              // all ctrlpins low
    cbi (LCD_RS_PORT, LCD_RS);
    cbi (LCD_RW_PORT, LCD_RW);              // we are going to write
}
