/*********************Copyright(c)************************************************************************
 * myuart.h
 *
 * date:20160507
 *
*********************************************************************************************************/
#ifndef MYUART_H
#define MYUART_H

//int gxgk_open_serial(char *serialname);		  

//void gxgk_close_serial(char *serialname);

//int gxgk_read_serial(char *serialname, char *buf);		  

//int gxgk_write_serial(char *serialname, int len);		  

int gxgk_open_serial(char *serialname);
void gxgk_close_serial(char *serialname);
int gxgk_read_serial(int fd, char *buf, int len); 
char uart_get_byte(int fd); 
int gxgk_write_serial(int fd, char buf[], int len);
void gxgk_uart2bwt();




#endif // end of MYUART_H
