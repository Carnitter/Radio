
#define LOG_MODULE  LOG_MAIN_MODULE

#include <sys/heap.h>
#include <sys/bankmem.h>
#include <sys/thread.h>
#include <sys/timer.h>

#include "stream_play.h"
#include "internet_connector.h"
#include "stream.h"
#include "vs10xx.h"
#include "log.h"
#include "rss.h"

#define OK			1
#define NOK			0

int threadStop = 0;
THREAD(StreamPlayer, arg);

int initPlayerstream(void)
{
	return OK;
}

int stop_thread(void)
{
	threadStop = OK;
	return OK;
}

int pausestream(void)
{
	VsSetVolume(100,100);
	return OK;
}

int volumemax(void)
{
	VsSetVolume(12, 12);
	return OK;
}

int playstream(FILE *stream)
{
	rss_set = 0;
	threadStop = NOK;
	NutThreadCreate("Streamplayer", StreamPlayer, stream, 512);
	LogMsg_P(LOG_INFO, PSTR("Play thread created. Device is playing stream now !\n"));	
	return OK;
}

/*
* NAME: ProcessMetaData
* DESCRIPTION: processing the metadata
* PARAMS: File * stream. the current stream.
* RETURNS: int 0
*/
int ProcessMetaData(FILE *stream)
{
	//printf("in procces\n");
    u_char blks = 0;
    u_short cnt;
    int got;
    int rc = 0;
    u_char *mbuf;

    /*
     * Wait for the lenght byte.
     */
    got = fread(&blks, 1, 1, stream);
    if(got != 1) {
        return -1;
    }
	//printf("1 \n");
    if (blks) {
        if (blks > 32) {
            printf("Error: Metadata too large, %u blocks\n", blks);
            return -1;
        }	
		printf("2 \n");
        cnt = blks * 16;
        if ((mbuf = malloc(cnt + 1)) == 0) {
            return -1;
        }
		printf("3 \n");
        /*
         * Receive the metadata block.
         */
        for (;;) {
            if ((got = fread(mbuf + rc, 1, cnt, stream)) <= 0) {
                return -1;
            }
            if ((cnt -= got) == 0) {
                break;
            }
            rc += got;
            mbuf[rc] = 0;
        }

        printf("\nMeta='%s'\n", mbuf);
		printf("END META");
        free(mbuf);
    }
    return 0;
}

/*
* NAME: THREAD 
* DESCRIPTION: Thread which plays a stream
* PARAMS: Threadname StreamPlayer, arg: arguments -> FILE * 
* RETURNS:
*/
THREAD(StreamPlayer, arg)
{
	FILE *stream = (FILE *) arg;
	size_t rbytes = 0;
	char *mp3buf;
	int result = NOK;
	int nrBytesRead = 0;
	unsigned char iflag;
	u_long mp3left = metaint;
	
	//NutThreadSetPriority(32);
	
	
	//
	// Init MP3 buffer. NutSegBuf is een globale, systeem buffer
	//
	if( 0 != NutSegBufInit(8192) )
	{
		// Reset global buffer
		iflag = VsPlayerInterrupts(0);
		NutSegBufReset();
		VsPlayerInterrupts(iflag);
		
		result = OK;
	}
	
	// Init the Vs1003b hardware
	if( OK == result )
	{
		if( -1 == VsPlayerInit() )
		{
			LogMsg_P(LOG_INFO, PSTR("Retry init Vs1003b \n"));
			if( -1 == VsPlayerReset(0) )
			{
				result = NOK;
				LogMsg_P(LOG_INFO, PSTR("Failed init Vs1003b \n"));
			}
			else
			{
				LogMsg_P(LOG_INFO, PSTR("Vs1003b retry initialised! \n"));
			}
		}
		else
		{
			LogMsg_P(LOG_INFO, PSTR("Vs1003b initialised! \n"));
		}
	}
	
	VsSetVolume(12,12);
	
	while(1 == 1)
	{
		/*
		 * Query number of byte available in MP3 buffer.
		 */
        iflag = VsPlayerInterrupts(0);
        mp3buf = NutSegBufWriteRequest(&rbytes);
        VsPlayerInterrupts(iflag);
		
		//LogMsg_P(LOG_INFO, PSTR("buffer"));
		
		if(threadStop == OK)
		{	
			VsPlayerStop();
			stopStream();
			printf("STOP THREAD HELP\n");
			NutThreadExit();
		}
		
		// Bij de eerste keer: als player niet draait maak player wakker (kickit)
		if( VS_STATUS_RUNNING != VsGetStatus() )
		{
			LogMsg_P(LOG_INFO, PSTR("Player not running - %zu\n"), rbytes);
			// if( rbytes < 1024 ) //Turned off for quick start of player.
			// {
				printf("VsPlayerKick()\n");
				VsPlayerKick();
			//}
		}
		
		/* 
         * Do not read pass metadata. 
         */
        if (metaint && rbytes > mp3left) {
            rbytes = mp3left;
        }
		
		while( rbytes )
		{
			//LogMsg_P(LOG_INFO, PSTR("streaming"));
			// Copy rbytes (van 1 byte) van stream naar mp3buf.
			nrBytesRead = fread(mp3buf,1,rbytes,stream);
			
			if( nrBytesRead > 0 )
			{
				iflag = VsPlayerInterrupts(0);
				mp3buf = NutSegBufWriteCommit(nrBytesRead);
				VsPlayerInterrupts(iflag);
				
				 if (metaint) {
                    mp3left -= nrBytesRead;
                    if (mp3left == 0) {
                        ProcessMetaData(stream);
                        mp3left = metaint;
                    }
                }
				
				if( nrBytesRead < rbytes && nrBytesRead < 512 )
				{
					NutSleep(250);
				}
			}
			else
			{
				LogMsg_P(LOG_INFO, PSTR("break1"));
				break;
			}
			rbytes -= nrBytesRead;
			
			if( nrBytesRead <= 0 )
			{
				LogMsg_P(LOG_INFO, PSTR("break2"));
				break;
			}				
		}
	}	
}
