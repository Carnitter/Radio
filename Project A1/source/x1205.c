#define LOG_MODULE  LOG_X1205_MODULE

#include <cfg/os.h>
#include <dev/twif.h>
#include <sys/event.h>

#include <stdlib.h>

#include "log.h"
#include "x1205.h"

// x1205 slave address, see source code (rtc.*) streamit
#define I2C_SLA_RTC 0x6F


/* ******************************************************************* */
void x1205Init(void)
/*
 * Initialiseer TWI, gebruik NUT/OS functionaliteit. 
 * see: http://www.ethernut.de/api/ of http://www.ethernut.de/nutwiki/I2C
 *
 */
{
	TwInit(0);   
	// 0 betekent dat wij master zijn, alle andere devices op de bus
	// zijn slaves.
	
	// Optioneel:
	//TwIOCtr(TWI_SETSPEED, &....);
	
	 LogMsg_P(LOG_INFO, PSTR("x1205Init()"));
}

/* ******************************************************************* */
int x1205ReadNByte(u_char addr, u_char *buf, u_char cnt )
/*
 * Reads N bytes from x1205 starting at addr. 
 *
 */
{
	int retval = 0;
	
	u_char writeBuffer[2];	
	writeBuffer[0] = 0;
	writeBuffer[1] = addr;  // SC register in x1205 RTC, datasheet page 10
	
	// TwMasterTransact schrijft eerst en leest daarna. Dit is wat we willen.
	// Schrijf eerst werk register we willen lezen, daarna lees het register
	if( TwMasterTransact(I2C_SLA_RTC, writeBuffer, 2, buf, cnt, NUT_WAIT_INFINITE) != cnt )
	{
		retval = -1;
		LogMsg_P(LOG_INFO, PSTR("error x1205readByte()"));
	}
	return retval;
}


/* ******************************************************************* */
int x1205Enable(void)
/*
 * Enable writing to x1205, see datasheet for details.
 *
 */
{
	u_char writeBuffer[5];
	
	writeBuffer[0] = 0;	
	writeBuffer[1] = 0x3f;
	writeBuffer[2] = 0x02;
	TwMasterTransact(I2C_SLA_RTC, writeBuffer, 3, 0, 0, NUT_WAIT_INFINITE);
	writeBuffer[2] = 0x06;
	TwMasterTransact(I2C_SLA_RTC, writeBuffer, 3, 0, 0, NUT_WAIT_INFINITE);
	return 1;
}

/* ******************************************************************* */
int x1205WriteNBytes(unsigned char addr, unsigned char *data, u_char cnt ) 
/*
 * Write N bytes to RTC starting at addr. 
 *
 */
{
	int retval = 0;
	int idx  = 0;
	
	u_char *wrBuf = (u_char *)malloc ((cnt+3) * sizeof(u_char));
	
	wrBuf[0] = 0;	
	wrBuf[1] = addr;
	for( idx = 0; idx < cnt; idx++ )
	{
		wrBuf[idx+2] = data[idx];
	}
	
	TwMasterTransact(I2C_SLA_RTC, wrBuf, cnt+2, 0, 0, NUT_WAIT_INFINITE);

	free(wrBuf);
	
	return retval;
}

int x1205WriteBuffer(unsigned char addr, unsigned char *data)
{
	return x1205WriteNBytes(addr, data, 8);	
}
