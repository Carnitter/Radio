/*	NAME: audio.c
	DESCRIPTION: Handles audio outputs to the vs1000.
	AUTHORS: group A1
 */

#include "vs10xx.h"
#include "audio.h"
#include <stdio.h>
#include "system.h"


 /*
	NAME: audio_init
	DESCRIPTION: Initilizes the audio systems
	PARAMS: none
	RETURNS: retuns -1 if failed and 0 if succes
 */
int audio_init(void)
{
	int i = VsPlayerInit();
	VsSetVolume(12,12);
	return i;
}

 /*
	NAME: audio_fallbacksound
	DESCRIPTION: Plays the fallback sound
	PARAMS: None
	RETURNS: returns -1 if failed and 0 if succes
 */
int audio_fallbacksound(void)
{
	int succes = -1;
	
	if (VsGetStatus() != VS_STATUS_RUNNING) {
		VsPlayerKick();
	}
	int i = 0;
	for(i = 0; i < 5; i++)
	{
		succes = VsBeep(50, 5000);
	}
		for(i = 0; i < 5; i++)
	{
		succes = VsBeep(10, 5000);
	}
		for(i = 0; i < 5; i++)
	{
		succes = VsBeep(50, 5000);
	}
	return succes;
}

 /*
	NAME: audio_getvolume
	DESCRIPTION: returns the volume value
	PARAMS: None 
	RETURNS: u_short value of the volume
 */
u_short audio_getvolume(void)
{
	u_short volume = VsGetVolume();
	return volume;
}
