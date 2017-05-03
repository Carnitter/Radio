#ifndef INTERNET_CONNECTOR_H_
#define INTERNET_CONNECTOR_H_
#include "uxml.h"

u_long metaint;

int initInet(void);
int connectToStream(char*, int , char* );
int playStream(void);
int stopStream(void);
UXML_NODE* connect_to_api(char* ip_addr, int port, char* extra);
char * connect_to_rss(char* ip_addr, int port, char* extra, char* f_tags, char* f_attribs);
#endif 
