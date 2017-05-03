/*
* NAME: internet_connector
* DESCRIPTION: .c file which handles the internet connection and mp3 stream connection
* AUTHORS: Group A1
*/

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <dev/nicrtl.h>
#include <arpa/inet.h>
#include <sys/timer.h>
#include <sys/confnet.h>
#include <pro/dhcp.h>
#include <pro/sntp.h>
#include "displayhandler.h"
#include <main_menu.h>
#include "stream_play.h"
#include <io.h>
#define LOG_MODULE  LOG_MAIN_MODULE
#include "log.h"
#define ETH0_BASE	0xC300
#define ETH0_IRQ	5
#include <sys/thread.h>
#include "internet_connector.h"
#include "uxml.h"
#include "list.h"

#define OK			1
#define NOK			0

/*!
 * \brief Size of the header line buffer.
 */
#define MAX_HEADERLINE 512

static char eth0IfName[9] = "eth0";
FILE *stream;


/*!
 * Method for connecting to the internet
 * - Registering network device named "eth0" to ethernut/os
 * - Asking for dynamic ip from dhcp server
 * Printing and returning dynamic ip
 *  \return character array like string which corresponds to device ip (null when can't connect)
 */
  
int initInet(void)
{
	uint8_t mac_addr[6] = { 0x00, 0x06, 0x98, 0x30, 0x02, 0x76 };
	static char eth0IfName[9] = "eth0";
	
	LogMsg_P(LOG_INFO, PSTR("Setting up new internet conection..."));
	int result = OK;

	// Registreer NIC device (located in nictrl.h)
	if( 0 != NutRegisterDevice(&DEV_ETHER, ETH0_BASE, ETH0_IRQ) )
	{
		LogMsg_P(LOG_ERR, PSTR("Error: >> NutRegisterDevice()"));
		result = NOK;
	} 
		
	
	if( OK == result )
	{
		//calls NutNetIfConfig() with specified mac adress and device name to configure ip adress
		if( 0 != NutDhcpIfConfig(eth0IfName, mac_addr, 0) )
		{	
			
			LogMsg_P(LOG_ERR, PSTR("Error: >> NutDhcpIfConfig()"));
			result = NOK;
		}
		
	}
	
	if( OK == result )
	{
		LogMsg_P(LOG_INFO, PSTR("Networking setup OK, new settings are:\n") );	

		LogMsg_P(LOG_INFO, PSTR("if_name: %s"), confnet.cd_name);	
		LogMsg_P(LOG_INFO, PSTR("ip-addr: %s"), inet_ntoa(confnet.cdn_ip_addr) );
		LogMsg_P(LOG_INFO, PSTR("ip-mask: %s"), inet_ntoa(confnet.cdn_ip_mask) );
		LogMsg_P(LOG_INFO, PSTR("gw     : %s"), inet_ntoa(confnet.cdn_gateway) );
	}
	
	NutSleep(1000);
	
	return result;
}

 /*
	NAME: connect_to_api
	DESCRIPTION: connect with an api. Get the xml data from the api. Next parse the xml data and place it in an UXML_NODE *. To parse give the tags you want to parse. If the tags have an attribute put this attribute in the f_attribs.
	PARAMS: char* ip_addr: Ip-adress to the api website.
			int port
			char* extra: extra URL information
	RETURNS: UXML_NODE*: UXML_NODE* with the parsed xml data.
 */
UXML_NODE* connect_to_api(char* ip_addr, int port, char* extra)
{
	
	LogMsg_P(LOG_INFO, PSTR("Trying to create socket..."));
	int result = OK;
	char *data;
	char *line;
	char *cp;
	metaint = 0;
	
	TCPSOCKET *sock;
	
	sock = NutTcpCreateSocket();
	if(0 != NutTcpConnect(	sock,
						inet_addr(ip_addr), 
						port) )
	{
		LogMsg_P(LOG_ERR, PSTR("Error: >> NutTcpConnect()"));
		return(0);
	}
	stream = _fdopen( (int) sock, "r+b" );
	
	fprintf(stream, "GET %s HTTP/1.0\r\n", extra);
	fprintf(stream, "Host: %s\r\n", ip_addr);
	fprintf(stream, "User-Agent: Ethernut\r\n");
	fprintf(stream, "Accept: */*\r\n");
	fprintf(stream, "Icy-MetaData: 1\r\n");
	fprintf(stream, "Connection: close\r\n\r\n");
	fflush(stream);
	
	//---------------
	/*
     * Receive the HTTP header.
     */
    line = malloc(MAX_HEADERLINE);
    while(fgets(line, MAX_HEADERLINE, stream)!=NULL) 
	{

        /*
         * Chop off the carriage return at the end of the line. If none
         * was found, then this line was probably too large for our buffer.
         */
        cp = strchr(line, '\r');
        if(cp == 0) {
            puts("Warning: Input buffer overflow");
            continue;
        }
        *cp = 0;

        /*
         * The header is terminated by an empty line.
         */
        if(*line == 0) {
            break;
        }
        if(strncmp(line, "icy-metaint:", 12) == 0) {
            metaint = atol(line + 8);
        }
        printf("%s\n", line);
    }
    putchar('\n');

    free(line);
	
	LogMsg_P(LOG_INFO,PSTR("Successfully created socket!\n"));
	
	/* These are the tags we want to parse. Unspecified tags are NOT parsed to save resources. */
   char *f_tags[] = { "city", "temperature", "humidity", "weather", NULL };
   /* These are the attributes we want to parse. Unspecified attributes are NOT parsed to save resources. */
   char *f_attribs[] = {"name", "value" };
	
	/* Temporary node pointers. We'll use them to iterate through our XML tree later on. */
	UXML_NODE *node = malloc(sizeof(UXML_NODE));
	UXML_NODE *products_tree = UxmlParseStream(stream, &f_tags, &f_attribs);
	node = products_tree;
	stopStream();
	return node;
}

char * connect_to_rss(char* ip_addr, int port, char* extra, char* f_tags, char* f_attribs)
{
	LogMsg_P(LOG_INFO, PSTR("Trying to create socket..."));
	int result = OK;
	char *data;
	char *line;
	char *cp;
	metaint = 0;
	
	TCPSOCKET *sock;
	
	sock = NutTcpCreateSocket();
	if(0 != NutTcpConnect(	sock,
						inet_addr(ip_addr), 
						port) )
	{
		LogMsg_P(LOG_ERR, PSTR("Error: >> NutTcpConnect()"));
		return(0);
	}
	stream = _fdopen( (int) sock, "r+b" );
	
	fprintf(stream, "GET %s HTTP/1.0\r\n", extra);
	fprintf(stream, "Host: %s\r\n", ip_addr);
	fprintf(stream, "User-Agent: Ethernut\r\n");
	fprintf(stream, "Accept: */*\r\n");
	fprintf(stream, "Icy-MetaData: 1\r\n");
	fprintf(stream, "Connection: close\r\n\r\n");
	fflush(stream);
	
	//---------------
	/*
     * Receive the HTTP header.
     */
    line = malloc(MAX_HEADERLINE);
    while(fgets(line, MAX_HEADERLINE, stream)!=NULL) 
	{

        /*
         * Chop off the carriage return at the end of the line. If none
         * was found, then this line was probably too large for our buffer.
         */
        cp = strchr(line, '\r');
        if(cp == 0) {
            puts("Warning: Input buffer overflow");
            continue;
        }
        *cp = 0;

        /*
         * The header is terminated by an empty line.
         */
        if(*line == 0) {
            break;
        }
        if(strncmp(line, "icy-metaint:", 12) == 0) {
            metaint = atol(line + 8);
        }
    }
    free(line);
	
	LogMsg_P(LOG_INFO,PSTR("Successfully created socket!\n"));
	

	/* Temporary node pointers. We'll use them to iterate through our XML tree later on. */
	UXML_NODE *node;
	UXML_ATTRIB *attr;
	UXML_NODE *products_tree = UxmlParseStream(stream, f_tags, f_attribs);
	node = products_tree;
	char * text = malloc(sizeof(char) *30);
	int count = 0;
	while (node) {
		if(count++ == 2)
			text = node->xmln_content;
		attr = node->xmln_attribs;
		while (attr) {
			attr = attr->xmla_next;
		}
		node = node->xmln_next;
	}
	UxmlTreeDestroy(products_tree);
	return text;
}



 /*
	NAME: connectToStream
	DESCRIPTION: Method to connect to a stream with the specified IP-address
	PARAMS: ip_addr: The ip to connect to, port: the port number to connect to.
	RETURNS: returns -1 if failed and 0 if succes
 */

 
int connectToStream(char* ip_addr, int port, char* extra)
{
	LogMsg_P(LOG_INFO, PSTR("Trying to create socket..."));
	int result = OK;
	char *data;
	char *line;
	char *cp;
	metaint = 0;
	
	TCPSOCKET *sock;
	
	sock = NutTcpCreateSocket();
	if(0 != NutTcpConnect(	sock,
						inet_addr(ip_addr), 
						port) )
	{
		LogMsg_P(LOG_ERR, PSTR("Error: >> NutTcpConnect()"));
		return(0);
	}
	stream = _fdopen( (int) sock, "r+b" );
	
	fprintf(stream, "GET %s HTTP/1.0\r\n", extra);
	fprintf(stream, "Host: %s\r\n", ip_addr);
	fprintf(stream, "User-Agent: Ethernut\r\n");
	fprintf(stream, "Accept: */*\r\n");
	fprintf(stream, "Icy-MetaData: 1\r\n");
	fprintf(stream, "Connection: close\r\n\r\n");
	fflush(stream);
	
	//---------------
	/*
     * Receive the HTTP header.
     */
    line = malloc(MAX_HEADERLINE);
    while(fgets(line, MAX_HEADERLINE, stream)!=NULL) 
	{

        /*
         * Chop off the carriage return at the end of the line. If none
         * was found, then this line was probably too large for our buffer.
         */
        cp = strchr(line, '\r');
        if(cp == 0) {
            puts("Warning: Input buffer overflow");
            continue;
        }
        *cp = 0;

        /*
         * The header is terminated by an empty line.
         */
        if(*line == 0) {
            break;
        }
        if(strncmp(line, "icy-metaint:", 12) == 0) {
            metaint = atol(line + 12);
        }
        printf("%s\n", line);
    }
    putchar('\n');

    free(line);
	// -------------------------
	
	LogMsg_P(LOG_INFO,PSTR("Successfully created socket!\n"));
	return 1;
	
	// Server stuurt nu HTTP header terug, catch in buffer
	data = (char *) malloc(512 * sizeof(char));
	
	while( fgets(data, 512, stream) )
	{
		if( 0 == *data )
			break;

		
		printf("%s", data);
	}
	
	free(data);
	
	return result;
}

int playStream(void)
{
	playstream(stream);
	
	return OK;
}

int stopStream(void)
{
	fclose(stream);	
	
	return OK;
}

