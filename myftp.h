/*********************Copyright(c)************************************************************************
 * myftp.h
 *
 * date:20180417
 *
*********************************************************************************************************/
#ifndef MYFTP_H 
#define MYFTP_H

#define HOST "220.180.240.174"
#define PORT 10085
#define USER "root"
#define PWD  "root13579"

#define STORNAME "/myftp/SLM150.xlsx"
//#define STORNAME "/123/test.txt"
#define PATH     "/opt/test.txt"
//#define PATH     "/opt/ftptest.txt"
#define OFFSET   "0"
#define STOP     "0"


 
//¿¿¿¿¿
int ftp_connect( char *host, int port, char *user, char *pwd );
//¿¿¿¿¿
int ftp_quit( int c_sock);
 
//¿¿¿¿¿¿
int ftp_type( int c_sock, char mode );
 
//¿¿¿¿¿¿
int ftp_cwd( int c_sock, char *path );
//¿¿¿¿¿¿¿
int ftp_cdup( int c_sock );
//¿¿¿¿
int ftp_mkd( int c_sock, char *path );
//¿¿
int ftp_list( int c_sock, char *path, void **data, unsigned long long *data_len);
 
//¿¿¿¿
int ftp_retrfile( int c_sock, char *s, char *d ,unsigned long long *stor_size, int *stop);
//¿¿¿¿
int ftp_storfile( int c_sock, char *s, char *d ,unsigned long long *stor_size, int *stop);
 
//¿¿¿¿¿&¿¿¿¿
int ftp_renamefile( int c_sock, char *s, char *d );
//¿¿¿¿
int ftp_deletefile( int c_sock, char *s );
//¿¿¿¿
int ftp_deletefolder( int c_sock, char *s ); 

void local_update();

int ftp_update();

#endif //end of MYFTP_H

