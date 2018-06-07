/*********************Copyright(c)************************************************************************
 * myftp.c
 *
 * date:20180416
 *
*********************************************************************************************************/
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "gx4g.h"
//#include "myftp.h"

/*
#define HOST "192.168.1.190"
#define PORT 69
#define USER NULL
#define PWD  NULL

#define STORNAME "uImagefortftptest"
#define PATH     "/root/mytest/123.test"
#define OFFSET   "0"
#define STOP     "0"

*/

//����һ��socket������
int socket_connect(char *host,int port)
{
    struct sockaddr_in address;
    int s, opvalue;
    socklen_t slen;
     
    opvalue = 8;
    slen = sizeof(opvalue);
    memset(&address, 0, sizeof(address));
     
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ||
        setsockopt(s, IPPROTO_IP, IP_TOS, &opvalue, slen) < 0)
    {
	printf ("s=%d\n",s);
        return -1;
    }     
    //���ý��պͷ��ͳ�ʱ
    struct timeval timeo = {15, 0};
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo));
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(timeo));
     
    address.sin_family = AF_INET;
    address.sin_port = htons((unsigned short)port);
     

    address.sin_addr.s_addr = inet_addr(host);
  //  struct hostent* server = gethostbyname(host);

  //  if (!server)
  //      return -1;
  //  memcpy(&address.sin_addr.s_addr, server->h_addr, server->h_length);
     
    if (connect(s, (struct sockaddr*) &address, sizeof(address)) == -1){
	printf("!!!!!!!!!!!\n");    
	    return -1;
    }
    return s;
}
 
//���ӵ�һ��ftp�ķ�����������socket
int connect_server( char *host, int port )
{    
    int       ctrl_sock;
    char      buf[512];
    int       result;
    ssize_t   len;
     
    ctrl_sock = socket_connect(host, port);
    printf("ctrl_sock=%d\n",ctrl_sock);
    if (ctrl_sock == -1) {
        return -1;
    }
     
    len = recv( ctrl_sock, buf, 512, 0 );
    buf[len] = 0;
    sscanf( buf, "%d", &result );
    if ( result != 220 ) {
        close( ctrl_sock );
        return -1;
    }
     
    return ctrl_sock;
}
 
//��������,���ؽ��
int ftp_sendcmd_re( int sock, char *cmd, void *re_buf, ssize_t *len)
{
    char        buf[512];
    ssize_t     r_len;
     
    if ( send( sock, cmd, strlen(cmd), 0 ) == -1 )
        return -1;
     
    r_len = recv( sock, buf, 512, 0 );
    if ( r_len < 1 ) return -1;
    buf[r_len] = 0;
     
    if (len != NULL) *len = r_len;
    if (re_buf != NULL) sprintf(re_buf, "%s", buf);
     
    return 0;
}
 
//��������,���ر��
int ftp_sendcmd( int sock, char *cmd )
{
    char     buf[512];
    int      result;
    ssize_t  len;
     
    result = ftp_sendcmd_re(sock, cmd, buf, &len);
    if (result == 0)
    {
        sscanf( buf, "%d", &result );
    }
     
    return result;
}
 
//��¼ftp������
int login_server( int sock, char *user, char *pwd )
{
    char    buf[128];
    int     result;
     
    sprintf( buf, "USER %s\r\n", user );
    result = ftp_sendcmd( sock, buf );
    if ( result == 230 ) return 0;
    else if ( result == 331 ) {
        sprintf( buf, "PASS %s\r\n", pwd );
        if ( ftp_sendcmd( sock, buf ) != 230 ) return -1;
        return 0;
    }
    else
        return -1;
}
 
int create_datasock( int ctrl_sock )
{
    int     lsn_sock;
    int     port;
    int     len;
    struct sockaddr_in sin;
    char    cmd[128];
     
    lsn_sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( lsn_sock == -1 ) return -1;
    memset( (char *)&sin, 0, sizeof(sin) );
    sin.sin_family = AF_INET;
    if( bind(lsn_sock, (struct sockaddr *)&sin, sizeof(sin)) == -1 ) {
        close( lsn_sock );
        return -1;
    }
     
    if( listen(lsn_sock, 2) == -1 ) {
        close( lsn_sock );
        return -1;
    }
     
    len = sizeof( struct sockaddr );
    if ( getsockname( lsn_sock, (struct sockaddr *)&sin, (socklen_t *)&len ) == -1 )
    {
        close( lsn_sock );
        return -1;
    }
    port = sin.sin_port;
     
    if( getsockname( ctrl_sock, (struct sockaddr *)&sin, (socklen_t *)&len ) == -1 )
    {
        close( lsn_sock );
        return -1;
    }
     
    sprintf( cmd, "PORT %d,%d,%d,%d,%d,%d\r\n",
            sin.sin_addr.s_addr&0x000000FF,
            (sin.sin_addr.s_addr&0x0000FF00)>>8,
            (sin.sin_addr.s_addr&0x00FF0000)>>16,
            (sin.sin_addr.s_addr&0xFF000000)>>24,
            port>>8, port&0xff );
     
    if ( ftp_sendcmd( ctrl_sock, cmd ) != 200 ) {
        close( lsn_sock );
        return -1;
    }
    return lsn_sock;
}
 
//���ӵ�PASV�ӿ�
int ftp_pasv_connect( int c_sock )
{
    int     r_sock;
    int     send_re;
    ssize_t len;
    int     addr[6];
    char    buf[512];
    char    re_buf[512];
     
    //����PASV����ģʽ
    bzero(buf, sizeof(buf));
    sprintf( buf, "PASV\r\n");
    send_re = ftp_sendcmd_re( c_sock, buf, re_buf, &len);
    if (send_re == 0) {
        sscanf(re_buf, "%*[^(](%d,%d,%d,%d,%d,%d)",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5]);
    }
     
    //����PASV�˿�
    bzero(buf, sizeof(buf));
    sprintf( buf, "%d.%d.%d.%d",addr[0],addr[1],addr[2],addr[3]);
    r_sock = socket_connect(buf,addr[4]*256+addr[5]);
     
    return r_sock;
}
 
//��ʾ����
int ftp_type( int c_sock, char mode )
{
    char    buf[128];
    sprintf( buf, "TYPE %c\r\n", mode );
    if ( ftp_sendcmd( c_sock, buf ) != 200 )
        return -1;
    else
        return 0;
}
 
//�ı乤��Ŀ¼
int ftp_cwd( int c_sock, char *path )
{
    char    buf[128];
    int     re;
    sprintf( buf, "CWD %s\r\n", path );
    re = ftp_sendcmd( c_sock, buf );
    if ( re != 250 )
        return -1;
    else
        return 0;
}
 
//�ص���һ��Ŀ¼
int ftp_cdup( int c_sock )
{
    int     re;
    re = ftp_sendcmd( c_sock, "CDUP\r\n" );
    if ( re != 250 )
        return re;
    else
        return 0;
}
 
//����Ŀ¼
int ftp_mkd( int c_sock, char *path )
{
    char    buf[512];
    int     re;
    sprintf( buf, "MKD %s\r\n", path );
    re = ftp_sendcmd( c_sock, buf );
    if ( re != 257 )
        return re;
    else
        return 0;
}
 
//�б�
int ftp_list( int c_sock, char *path, void **data, unsigned long long *data_len)
{
    int d_sock;
    char    buf[512];
    int     send_re;
    int     result;
    ssize_t     len,buf_len,total_len;
     
    //���ӵ�PASV�ӿ�
    d_sock = ftp_pasv_connect(c_sock);
    if (d_sock == -1) {
        return -1;
    }
     
    //����LIST����
    bzero(buf, sizeof(buf));
    sprintf( buf, "LIST %s\r\n", path);
    send_re = ftp_sendcmd( c_sock, buf );
    if (send_re >= 300 || send_re == 0)
        return send_re;
     
    len=total_len = 0;
    buf_len = 512;
    void *re_buf = malloc(buf_len);
    while ( (len = recv( d_sock, buf, 512, 0 )) > 0 )
    {
        if (total_len+len > buf_len)
        {
            buf_len *= 2;
            void *re_buf_n = malloc(buf_len);
            memcpy(re_buf_n, re_buf, total_len);
            free(re_buf);
            re_buf = re_buf_n;
        }
        memcpy(re_buf+total_len, buf, len);
        total_len += len;
    }
    close( d_sock );
     
    //����������շ���ֵ
    bzero(buf, sizeof(buf));
    len = recv( c_sock, buf, 512, 0 );
    buf[len] = 0;
    sscanf( buf, "%d", &result );
    if ( result != 226 )
    {
        free(re_buf);
        return result;
    }
     
    *data = re_buf;
    *data_len = total_len;
     
    return 0;
}
 
//�����ļ�
int ftp_retrfile( int c_sock, char *s, char *d ,unsigned long long *stor_size, int *stop)
{
	ssize_t     len,write_len;
	char    buf[512];
	int     result;
	int     whence;

	//�򿪱����ļ�
	//psh->handle = open( d,  O_WRONLY|O_CREAT|O_TRUNC, S_IREAD|S_IWRITE );
	psh->handle = open( d,  O_WRONLY|O_CREAT|O_APPEND, S_IREAD|S_IWRITE );
	if ( psh->handle == -1 ) return -1;

	//���ô���ģʽ
	ftp_type(c_sock, 'I');

	//���ӵ�PASV�ӿ�
	psh->d_sock = ftp_pasv_connect(c_sock);
	printf("d_sock=%d\n",psh->d_sock);

	if (psh->d_sock == -1){
		close(psh->handle);
		return -1;
	}

	//����REST������жϵ���������
	bzero(buf, sizeof(buf));
	sprintf( buf, "REST %d\r\n",*stor_size);
	result = ftp_sendcmd( c_sock, buf );
	printf ("\nduanxian chonglian result=%d \n",result);
	if (result !=350 || result == 0)
	{
		printf("REST failed!\n");
		close(psh->handle);
		return result;
	}

	//����STOR����
	bzero(buf, sizeof(buf));
	sprintf( buf, "RETR %s\r\n", s );
	result = ftp_sendcmd( c_sock, buf );
	if (result >= 300 || result == 0)
	{
		close(psh->handle);
		return result;
	}

	//��ʼ��PASV��ȡ����
	bzero(buf, sizeof(buf));
	while ( (len = recv( psh->d_sock, buf, 512, 0 )) > 0 ) { 
		printf("\nhandle = %d",psh->handle);
		printf(" d_sock=%d ",psh->d_sock);
		write_len = write( psh->handle, buf, len );

		lseek(psh->handle,0,whence);
		stop = &whence;

		printf("11111111write_len=%d,len=%d,stor_size=%d\n",write_len,len,*stor_size);

		// if (write_len != len)
		if (write_len != len || (stop == NULL && *stop))
		{
			printf("write localfile failed!\n");
			close( psh->d_sock );
            close( psh-> handle );
		    return -1;
		}
		 
		if (stor_size != NULL)
		{
		    *stor_size += write_len;
		}
	}
	printf("locale file write success!\n");
	fsync(psh->handle);

	close( psh->d_sock );
	close( psh->handle );

	//����������շ���ֵ
	bzero(buf, sizeof(buf));
	len = recv( c_sock, buf, 512, 0 );
	buf[len] = 0;
	sscanf( buf, "%d", &result );
	if ( result >= 300 ) {
	return result;
	}
	return 0;
}
 
//�ϴ��ļ�
int ftp_storfile( int c_sock, char *s, char *d ,unsigned long long *stor_size, int *stop)
{
    int     d_sock;
    ssize_t     len,send_len;
    char    buf[512];
    int     handle;
    int send_re;
    int result;
     
    //�򿪱����ļ�
    handle = open( s,  O_RDONLY);
    if ( handle == -1 ) return -1;
     
    //���ô���ģʽ
    ftp_type(c_sock, 'I');
     
    //���ӵ�PASV�ӿ�
    d_sock = ftp_pasv_connect(c_sock);
    if (d_sock == -1)
    {
        close(handle);
        return -1;
    }
     
    //����STOR����
    bzero(buf, sizeof(buf));
    sprintf( buf, "STOR %s\r\n", d );
    send_re = ftp_sendcmd( c_sock, buf );
    if (send_re >= 300 || send_re == 0)
    {
        close(handle);
        return send_re;
    }
     
    //��ʼ��PASVͨ��д����
    bzero(buf, sizeof(buf));
    while ( (len = read( handle, buf, 512)) > 0)
    {
        send_len = send(d_sock, buf, len, 0);
        if (send_len != len ||
            (stop != NULL && *stop))
        {
            close( d_sock );
            close( handle );
            return -1;
        }
         
        if (stor_size != NULL)
        {
            *stor_size += send_len;
        }
    }
    close( d_sock );
    close( handle );
     
    //����������շ���ֵ
    bzero(buf, sizeof(buf));
    len = recv( c_sock, buf, 512, 0 );
    buf[len] = 0;
    sscanf( buf, "%d", &result );
    if ( result >= 300 ) {
        return result;
    }
    return 0;
}
 
//�޸��ļ���&�ƶ�Ŀ¼
int ftp_renamefile( int c_sock, char *s, char *d )
{
    char    buf[512];
    int     re;
     
    sprintf( buf, "RNFR %s\r\n", s );
    re = ftp_sendcmd( c_sock, buf );
    if ( re != 350 ) return re;
    sprintf( buf, "RNTO %s\r\n", d );
    re = ftp_sendcmd( c_sock, buf );
    if ( re != 250 ) return re;
    return 0;
}
 
//ɾ���ļ�
int ftp_deletefile( int c_sock, char *s )
{
    char    buf[512];
    int     re;
     
    sprintf( buf, "DELE %s\r\n", s );
    re = ftp_sendcmd( c_sock, buf );
    if ( re != 250 ) return re;
    return 0;
}
 
//ɾ��Ŀ¼
int ftp_deletefolder( int c_sock, char *s )
{
    char    buf[512];
    int     re;
     
    sprintf( buf, "RMD %s\r\n", s );
    re = ftp_sendcmd( c_sock, buf );
    if ( re != 250 ) return re;
    return 0;
}
 
//���ӷ�����
int ftp_connect( char *host, int port, char *user, char *pwd )
{
    int     c_sock;
    c_sock = connect_server( host, port );

     printf("c_sock=%d\n",c_sock);
    if ( c_sock == -1 ) return -1;
    if ( login_server( c_sock, user, pwd ) == -1 ) {
        close( c_sock );
        return -1;
    }
    return c_sock;
}
 
//�Ͽ�������
int ftp_quit( int c_sock)
{
    int re = 0;
    re = ftp_sendcmd( c_sock, "QUIT\r\n" );
    close( c_sock );
    return re;
}
 
void local_update()
{
	unsigned char cmd[256];

	bzero(cmd, 256);
	sprintf(cmd, "%s", "/opt/shell_all_4g.sh localupdate");
	system(cmd);
	usleep (10*1000);
}

void mymd5check()
{
    char   psBuffer[512];
	FILE   *pPipe;
    int ret = 0;

	if( (pPipe = popen( "md5sum " PATH, "r" )) == NULL ) exit( -1);

	while(fgets(psBuffer, 512, pPipe)){
		printf("%s", psBuffer);
		ret = 1;
		break;
	}
	pclose(pPipe);
	return ret;
}
//
int ftp_update()
{

	unsigned long long a1 =0;
	int a2 = 0;
	unsigned long long *offset= &a1;
	int *stop =&a2;
	int ret = 1;

	psh->ftp_fd = ftp_connect(HOST,PORT,USER,PWD);
    if (psh->ftp_fd < 0){
        printf("ftp_fd error!\n");
        return;
    }
    printf("HHHHHHHHHHHHHHHHHHHHHH Here is updating HHHHHHHHHHHHHHHHHHHH\n");
    ret = ftp_retrfile(psh->ftp_fd,STORNAME,PATH,offset,stop);
    printf("KKKKKKKKKKKKKKKKKKKK updating is finished KKKKKKKKKKKKKKKKKK\n");
    if (ret == 0){
        //�ļ�����У��
        mymd5check(); 
        
        printf("MMMMMMMMMMMMMMMMMMMMM local updateing MMMMMMMMMMMMMMMMMMMMMM\n");

        //
        local_update();
    }	

    ftp_quit(psh->ftp_fd);
        

    return 0;
}
