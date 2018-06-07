/*********************Copyright(c)************************************************************************
 * mysoctekcan.h
 *
 * date:20160509
 *
*********************************************************************************************************/

#ifndef MYSOCKET_H 
#define MYSOCKET_H 

#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>

#ifndef AF_CAN
#define AF_CAN 29
#endif
#ifndef PF_CAN
#define PF_CAN AF_CAN
#endif

#define errout(_s)	fprintf(stderr, "error class: %s\n", (_s))
#define errcode(_d) fprintf(stderr, "error code: %02x\n", (_d))
#define myerr(str)	fprintf(stderr, "%s, %s, %d: %s\n", __FILE__, __func__, __LINE__, str)

void print_frame(struct can_frame *fr);

void handle_err_frame(struct can_frame *fr);

int pack_send(int cmd, int fd,unsigned char *str);
int pack_send_test(int cmd, int fd);

int pack_send_retry(int cmd, int fd,unsigned char *str);
int pack_send_retry_test(int cmd, int fd);

int can_remote_config(int fd,unsigned char *str);
int can_remote_config_test(int fd);

int can_read(int fd);

void can_data_deal(struct can_frame *fr);

void staticerrorchk(void);

int socketcan_init();




#define INF1	1
#define INF3	2
#define INF4	3
#define INF5	4
#define INF6	5
#define INF7	6
#define INF8	7
#define INF9	8
#define INF10	9
#define INF11	10
#define INF12	11
#define INF13	12
#define INF14	13
#define INF15	14
#define INF16	15
#define INF17	16
#define INF18	17
#define INF19	18
#define INF20	19



#endif // end of  MYSOCKET_H 
