/*********************Copyright(c)************************************************************************
 * myuart.c
 *
 * date:20160507
 *
*********************************************************************************************************/
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <termios.h>
#include <errno.h>   
#include <asm/ioctls.h>
#include <time.h>
#include <strings.h>

#include "gx4g.h"

#define DATA_LEN                0xFF                                    /* test data's len              */

//mytempglobal.uartnum

int gxgk_open_serial(char *serialname)
{
    int fd;

    struct termios opt; 

    fd = open(serialname, O_RDWR | O_NOCTTY);                        
    if(fd < 0) {
        perror(serialname);
        return -1;
    }

    tcgetattr(fd, &opt);      

    cfsetispeed(&opt, B9600);
    cfsetospeed(&opt, B9600);

   // cfsetispeed(&opt, B115200);
   // cfsetospeed(&opt, B115200);

    
    /*
     * raw mode
     */
    opt.c_lflag   &=   ~(ECHO   |   ICANON   |   IEXTEN   |   ISIG);
    opt.c_iflag   &=   ~(BRKINT   |   ICRNL   |   INPCK   |   ISTRIP   |   IXON);
    opt.c_oflag   &=   ~(OPOST);
    opt.c_cflag   &=   ~(CSIZE   |   PARENB);
    opt.c_cflag   |=   CS8;

    /*
     * 'DATA_LEN' bytes can be read by serial
     */
    opt.c_cc[VMIN]   =   DATA_LEN;                                      
    opt.c_cc[VTIME]  =   150;

    if (tcsetattr(fd,   TCSANOW,   &opt)<0) {
        return   -1;
    }

    return fd;
    //return 1;
}

void gxgk_close_serial(char *serialname)
{
	if(serialname> 0) {	
		close(serialname);
	}

	return;
}


int gxgk_read_serial(int fd, char *buf, int len) 
{
	int ret = 0, i = 0;;

	ret = read(fd, buf, len);	
printf("ssssssssssssssssssssssssssssssss\n");
printf("ssssssssssssssssssssssssssssssss\n");
printf("ssssssssssssssssssssssssssssssss\n");
printf("ssssssssssssssssssssssssssssssss\n");
	printf("\n");
	printf("read %d -- %s", ret, buf);
	for(i = 0; i < ret; i++){
		printf("hellogotion %d\n", buf[i]);
		//printf("hellogotion %x\n", buf[i]);
	}
	printf("\n");
	
	return ret;
}

char uart_get_byte(int fd) 
{
	int ret = 0, i = 0;
	char buf[2];

	ret = read(fd, buf, 1);	
	return buf[0];
}


int gxgk_write_serial(int fd, char buf[], int len)
{
	int write_count;
	int i =0;

	write_count = write(fd, buf, len);

	printf("write %d -- %s", write_count, buf);

	printf("\n");
	for(i = 0; i < write_count; i++){
		printf("0x%02x ", buf[i]);
		//printf("hellogotion %x\n", buf[i]);
	}

	printf("\n");
	
	return write_count;
}

/*
int gxgk_write_serial(int fd, char *buf, int len)
{
	char data[16];
	int write_count;
	int i =0;

	bzero(data, 16);

	data[0] = 0x01;
	data[1] = 0x01;
	data[2] = 0x02;
	data[3] = 0x03;
	data[4] = 0x04;
	data[5] = 0x05;
	data[6] = 0x05;

	write_count = write(fd, data, 8);

	printf("\n");
	printf("write %d -- %s", write_count, data);
	for(i = 0; i < write_count; i++){
		printf("he %d\n", data[i]);
		//printf("hellogotion %x\n", buf[i]);
	}

	printf("\n");
	
	return write_count;
}
*/
