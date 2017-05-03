/*	NAME: audio.h
	DESCRIPTION: Handles audio outputs to the vs1000.
	AUTHORS: group A1
 */

int audio_init(void);
u_short audio_getvolume(void);
int audio_fallbacksound(void);
