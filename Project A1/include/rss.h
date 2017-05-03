/*	NAME: rss.h
	DESCRIPTION: Builds the screen for the rss settings.
	AUTHORS: group A1
 */
#ifndef RSS
#define RSS
char * rss_item;
extern int rss_settings;
extern int rss_set;
 
extern void rss_task(void);
extern void show(void);
extern void load_rss(void);
#endif 
