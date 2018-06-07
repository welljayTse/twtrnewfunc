/*********************Copyright(c)************************************************************************
 * my4gmodule.h
 *
 * date:20160512
 *
*********************************************************************************************************/

#ifndef MY4GMODULE_H
#define MY4GMODULE_H

#include "gx4g.h"

#define CID_EXTRM_INFO	0xb110
#define CID_CELL_INFO	0xb111
#define CID_GPS_INFO	0xb112

static void dealSigAlarm(int signo);

unsigned char chksumgen (const unsigned char *source, unsigned int len);

void transfercontrl(unsigned char *data, unsigned char *frame, unsigned short cid, unsigned short len);

void tcpinit();

void udpinit();

int  udpsend ( unsigned char sdbuf[],unsigned int len);

int  udprecv ( unsigned char revbuf[],unsigned int fd);

int udprecv_test(unsigned char revbuf[],unsigned int fd);

extern unsigned char g_ulBWTVersion; 
extern unsigned int  g_ulBWTAddr ; 
//extern unsigned char cell_total_count ;
extern char g_uctransferbuf[800];
//extern char revstr[];
//extern int      sockfd; 
//
//extern struct   sockaddr_in addr_remote,peer;    			// Host address information
//
//extern socklen_t  addrlen;
//extern char     revstr[];            		                 	// Receive buffer for byte2hexstr

#endif //  MY4GMODULE_H
