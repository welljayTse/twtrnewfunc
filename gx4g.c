#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "gx4g.h"


struct_temp_global 	mytempglobal;

gxgk_stru_share *psh;// DON'T MODIFY IT

INT8U bmu_cell_cnt[BMU_CELL_CNT]={0};

INT16U bmu_cell_num_start[BMU_CELL_CNT]={0};

INT16U cell_cnt_per_bmu[BMU_CELL_CNT]={0};


void gxgk_share_params_init()
{

	memset(psh, 0, sizeof(gxgk_stru_share));
	psh->uartnum = 2;     
    psh->sd = 0;	
	psh->count = 0;
	psh->pid_caninfo_get = 0;
	psh->pid_gpsinfo_get = 0;
	psh->pid_server_recv = 0;
	psh->pid_data_send = 0;
    psh->pid_time_count = 0;
	psh->pid_ping_connect = 0;

	psh->status_machine = GX4G_SM_MAIN;

   	extrm_info gs_extrminfo_recv = {
            0x0C,0X80,        //batterytv[2]; 		
	    0x0C,0x80,        //current[2];		      
	    0x00,0x60,        //soc[2];		 	
	    0x0c,0xbd,        //maxcellvolt[2];	        
	    0x0c,0xb0,        //mincellvolt[2] ;	
	    0x01,0x0a,        //maxcellvoltno[2];       
	    0x02,0x14,        //mincellvoltno[2] ;    
	    0x0c,0xb6,        //averagecellvoltage[2];
	    0,0x0d,           //vtgsub[2];
	    0x41,             //maxtempera;			
	    0x41,             //mintempera;		
	    0x09,             //maxtemperano;           
	    0x12,             //mintemperano;
	    0x41,             //averagetemp;		
        0,                //tempsub;		
        0,0,0,0,0,0,0,0,  //alarminfo[8];	
        12,5,12,1,55,50,  //time[6];		
        38                //len;		
	};
	memcpy(&psh->gs_extrminfo_recv,&gs_extrminfo_recv, sizeof(extrm_info));

   	extrm_info gs_extrminfo_send = {
	    0x0C,0X80,        //batterytv[2]; 		
	    0x0C,0x80,        //current[2];		      
	    0x00,0x60,        //soc[2];		 	
	    0x01,0x0a,        //maxcellvoltno[2];       
	    0x0c,0xbd,        //maxcellvolt[2];	        
	    0x02,0x14,        //mincellvoltno[2] ;    
	    0x0c,0xb0,        //mincellvolt[2] ;	
	    0x0c,0xb6,        //averagecellvoltage[2];
	    0,0x0d,           //vtgsub[2];
	    0x41,             //maxtempera;		
	    0x41,             //mintempera;		
	    0x09,             //maxtemperano;           
	    0x12,             //mintemperano;
	    0x41,             //averagetemp;		
	    0,                //tempsub;		
        0,0,0,0,0,0,0,0,  //alarminfo[8];	
        12,5,12,1,55,50,  //time[6];		
        38                //len;		
	};                     
	memcpy(&psh->gs_extrminfo_send, &gs_extrminfo_send, sizeof(extrm_info));

	gps_info gs_gpsinfo_recv = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	memcpy (&psh->gs_gpsinfo_recv,&gs_gpsinfo_recv,sizeof(gps_info));

	gps_info gs_gpsinfo_send = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	memcpy (&psh->gs_gpsinfo_send,&gs_gpsinfo_send,sizeof(gps_info));

	car_info gs_carinfo_recv = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	memcpy (&psh->gs_carinfo_recv,&gs_carinfo_recv,sizeof(car_info));

	car_info gs_carinfo_send = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	memcpy (&psh->gs_carinfo_send,&gs_carinfo_send,sizeof(car_info));

	cell_info gs_cellinfo_recv = {0x0f000000,0xffffffff,0xffff7f00,0xffffffff,0xff000000,0,0,0,0,0,0,0,0,0,0,0,0,
	                              0x5f00,0x5f00};
	memcpy(&psh->gs_cellinfo_recv,&gs_cellinfo_recv, sizeof(cell_info));

	cell_info gs_cellinfo_send = {0x0f000000,0xffffffff,0xffff7f00,0xffffffff,0xff000000,0,0,0,0,0,0,0,0,0,0,0,0,
	                              0x5f00,0x5f00};
	memcpy(&psh->gs_cellinfo_send,&gs_cellinfo_send, sizeof(cell_info));

	gov_info gs_govinfo_recv = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	memcpy (&psh->gs_govinfo_recv,&gs_govinfo_recv,sizeof(gov_info));

	gov_info gs_govinfo_send = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	memcpy (&psh->gs_govinfo_send,&gs_govinfo_send,sizeof(gov_info));


	remotecfg_info  gs_remotecfg_send    = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                            0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};//for test
	remotecfg_info  gs_remotecfg_sendall = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                            0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};//for test
	
	memcpy(&psh->gs_remotecfg_send,&gs_remotecfg_send, sizeof(remotecfg_info));
	memcpy(&psh->gs_remotecfg_sendall,&gs_remotecfg_sendall, sizeof(remotecfg_info));

   	config_info gs_config_info = {"11111111",0x91,0,0,0,0,0,0,0,0,0,0,0,0,
                                    0,0,10,10,200,200,200,200,20,20};
	memcpy(&psh->gs_config_info,&gs_config_info, sizeof(config_info));

	memset (psh->string,0,256);
	memset (psh->gs_transferbuf,0,512);
	memset (psh->gs_recvbuf,0,512);

        
	psh->sockfd =0;
 	bzero(&psh->addr_remote, sizeof(psh->addr_remote));              	
    bzero(&psh->peer, sizeof(psh->peer));              	
	psh->addrlen =0;

	psh->recv_flag = 0;
	psh->wait_flag = 0;
	

	psh->gps_recv_flag =0;
	psh->gps_fd =0;

    psh->can_fd = 0;
    psh->can_recv_flag = 0;

	psh->extrminfo_send_flag = 0;
	psh->carinfo_send_flag  = 0;
	psh->cellinfo_send_flag = 0;
	psh->gpsinfo_send_flag  = 0;
	psh->govinfo_send_flag  = 0;
	psh->heartbeat_send_flag  = 0;
	
	psh->heartbeat_check_count = 3;

	memset(psh->localtime,0,6);
	psh->aliveflag = 0;

	psh->ftp_fd = 0;
	psh->handle = 0;
	psh->d_sock = 0;
}

/* Test func using hardware led shine */
void shine_led()
{
    unsigned char cmd[256]; 

    bzero(cmd, 256);  
    sprintf(cmd, "%s", "/opt/shell_all_4g.sh ledshine"); 
    system(cmd); 
    usleep (100*1000); 
}

void caninit()
{
	unsigned char cmd[128];
	int ret = 0;

	bzero(cmd, 128);
	sprintf(cmd, "%s", "/opt/shell_all_4g.sh caninit");
	ret = system(cmd);
}

void longsunginit(void)
{
	unsigned char cmd[256];

	bzero(cmd, 256);
	sprintf(cmd, "%s", "/opt/shell_all_4g.sh 4ginit");
	system(cmd);
	sleep (30);
}

void pppdial(void)
{
	unsigned char cmd[256];

	bzero(cmd, 256);
	sprintf(cmd, "%s", "/opt/shell_all_4g.sh 4gdial ");
	system(cmd);
	sleep(30);
}

void netroute(void)
{
	unsigned char cmd[256];

	bzero(cmd, 256); 
   	sprintf(cmd, "%s", "/opt/shell_all_4g.sh 4groute");
	system(cmd);
	sleep(5);
}

void gps_init()
{
   	unsigned char cmd[256];

	bzero(cmd, 256);
	sprintf(cmd, "%s", "/opt/shell_all_4g.sh gpsinit");
	system(cmd);
	usleep (100*1000);
    
    /* do not use loongsung's gps */
    //	psh->gps_fd = open ("/dev/ttyUSB3",O_RDWR);
    //	printf("\npsh->gps_fd=%d\n",psh->gps_fd);
}

void ntp_update()
{
	unsigned char cmd[256];

	bzero(cmd, 256);
	sprintf(cmd, "%s", "/opt/shell_all_4g.sh ntpdate");
	system(cmd);
	usleep (100*1000);
}

int ping()
{
	char   psBuffer[512];
	FILE   *pPipe;
	int ret = 0;

	if( (pPipe = popen( "ping " TARGET, "r" )) == NULL ) exit( -1);

	while(fgets(psBuffer, 512, pPipe)){
		printf("%s", psBuffer);
		ret = 1;
		break;
	}
	pclose(pPipe);
	return ret;
}

void gxgk_init()
{
   	//int s,extrminfo_send_flag,carinfo_send_flag,cellinfo_send_flag,gpsinfo_send_flag,govinfo_send_flag;
	psh = mmap(NULL, sizeof(gxgk_stru_share), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
         
	if (psh == MAP_FAILED) {
		fprintf(stderr, "gxgk_init: mmap error about psh\n");
		return;
	}
	
#if 0
       caninit();
       longsunginit();
       pppdial();
       netroute();  
       ntp_update();
#endif

	gxgk_share_params_init();
	
	//gps_init();
	//tcpinit();
	udpinit();
	socketcan_init(); 

	storage_card_init();

	//can_up_config();
	printf("*******************************************************************\n");
	printf("*******************************************************************\n");
	printf("*******************************************************************\n");
	printf("*******************************************************************\n");
	printf("*******************************************************************\n");

	load_config(CONFIG_FILE_PATH); 

	sleep(1);

	update_config("/opt/123.ini");
	printf("*******************************************************************\n");
	printf("*******************************************************************\n");
	printf("*******************************************************************\n");
	printf("*******************************************************************\n");
	
	/* 2016-11-11 add serial fuc.*/
	mytempglobal.uartnum = gxgk_open_serial(GXGK_UART_NAME);		  
	printf("mytempglobal.uartnum = %d\n",mytempglobal.uartnum );
	printf("can fd = %d\n",psh->can_fd );
	 
	ubxconfig(mytempglobal.uartnum);


	/* Process1 4g reconnect */
   	psh->pid_ping_connect = fork();
	if (psh->pid_ping_connect< 0) {
        fprintf(stderr, "ping error: create child process error\n");
       	return;
	}
        else if (psh->pid_ping_connect == 0) {
		while (1){
		     if(ping() == 1){
			     if (psh->localtime[0]< 2000){
				     ntp_update();
				     get_local_time();
			     }
			     sleep (60);
			     continue;
		     }
		     sleep(1);
		     netroute();  
		}
	}

	/* Process2 time count */
   	psh->pid_time_count = fork();
	if (psh->pid_time_count < 0) {
        fprintf(stderr, "time_count: create child process error\n");
        return;
	}
    else if (psh->pid_time_count == 0) {
        while (1){
            /* 60000 means 20min */
            usleep(20000);
            if(psh->can_time_count++ > 60000) {
                psh->can_time_count = 0;
            }
            if(psh->extrminfo_send_flag ++ > 60000) {
                psh->extrminfo_send_flag = 0;
            }
            if(psh->carinfo_send_flag ++ > 60000) {
                psh->carinfo_send_flag = 0;
            }
            if(psh->cellinfo_send_flag ++ > 60000) {
                psh->cellinfo_send_flag = 0;
            }
            if(psh->gpsinfo_send_flag ++ > 60000) {
                psh->gpsinfo_send_flag = 0;
            }
            if(psh->heartbeat_send_flag ++ > 60000) {
                psh->heartbeat_send_flag = 0;
            }
        }	
	}

	/* Process3 gpsinfo deal */
	psh->pid_gpsinfo_get = fork();
	if (psh->pid_gpsinfo_get < 0) {
        fprintf(stderr, "gpsinfo_get: create child process error\n");
        return;
	}
    else if (psh->pid_gpsinfo_get == 0) {
		while (1){
			gpsinfoget(mytempglobal.uartnum);
			usleep(10);
		}	
	}


	/* Process3 block caninfo deal */
	psh->pid_caninfo_get = fork();
	if (psh->pid_caninfo_get < 0) {
        fprintf(stderr, "caninfo_get: create child process error\n");
       	return;
	}
    else if (psh->pid_caninfo_get == 0) {
        
       		while (1){
                can_read(psh->can_fd);
                usleep(10);
            }
	}

	/* Process4 data recv&send */
	psh->pid_data_send = fork();

  	if (psh->pid_data_send  < 0){
		fprintf(stderr, "data_send: create child process error\n");
		return;
  	}
  	else if (psh->pid_data_send  == 0) {
		fd_set readfs,writefs;
		struct timeval timeout={1,0};
  	       	
		while (1) {
			FD_ZERO(&readfs);
			FD_ZERO(&writefs);
			FD_SET(psh->sockfd,&readfs);
			FD_SET(psh->sockfd,&writefs);
			timeout.tv_sec=1;
			timeout.tv_usec=0;
            usleep(10);

		       /* block style with timeout function */ 
            if((select(psh->sockfd+1,&readfs,&writefs,(fd_set*)NULL,&timeout))>0){ 
                
                /* udp recv here */ 
                if(FD_ISSET(psh->sockfd,&readfs)){ 
                    if (udprecv(psh->gs_recvbuf,128) < 0){ 
                        continue ; 
                    } 
                }
                
                /* udp send here */ 
                if(FD_ISSET(psh->sockfd,&writefs)){

					/* test heatbeat send fre set 2s */
					if(psh->heartbeat_send_flag >= 2 * 50){
						transfercontrl((unsigned char *)(NULL),(unsigned char *)(&psh->gs_transferbuf),0xb115,0);
						udpsend(psh->gs_transferbuf,16);
						usleep(100);
						psh->heartbeat_send_flag = 0;
						
						psh->heartbeat_check_count --;// init 3
						if(psh->heartbeat_check_count < 0){
							psh->heartbeat_check_count = 0;
                            printf("^^^^^^^^^^^^^^^^^^ heart beat is break ^^^^^^^^^^^^^^^^^^^^^^\n");

                          //  shine_led();
						}
					}
                    /* extrminfo send */
					if((psh->extrminfo_send_flag >= psh->gs_config_info.extrem_send_freq * 50)\
                        && (psh->can_recv_flag ==1 )){

                        /* pack realtime data */
                        transfercontrl((unsigned char *)(&psh->gs_extrminfo_send),\
                            (unsigned char *)(&psh->gs_transferbuf),0xb110,38);

                        /* heartbeat is not break, send and save online package,
                           reissue offline package if necessary */
                        if (psh->heartbeat_check_count > 0){
                            saveinfo((unsigned char *)(&psh->gs_extrminfo_send),38,0);
                            udpsend(psh->gs_transferbuf,54); 
                            reissue_info_send(38,2);
                        }
                        /* heartbeat break, just store offline package */
                        else{
                            saveinfo((unsigned char *)(&psh->gs_extrminfo_send),38,2);
                        }
                        usleep(10);
                        psh->extrminfo_send_flag = 0;
                    }
                }//end fdset_write
#if 0
					if(psh->carinfo_send_flag >= psh->gs_config_info.car_send_freq * 50){
						transfercontrl((unsigned char *)(&psh->gs_carinfo_send),(unsigned char *)(&psh->gs_transferbuf),0xb111,34);
						udpsend(psh->gs_transferbuf,50);
						usleep(1000);
						psh->carinfo_send_flag = 0;

						/*it is for ftp update test*/
					//	ftp_update();
						
					}
					if(psh->gpsinfo_send_flag >= psh->gs_config_info.gps_send_freq * 50){
						if(psh->gps_recv_flag == 1){
							transfercontrl((unsigned char *)(&psh->gs_gpsinfo_send),(unsigned char *)(&psh->gs_transferbuf),0xb112,18);
							udpsend(psh->gs_transferbuf,34);
							psh->gps_recv_flag = 0;
							usleep(1000);
					}
						psh->gpsinfo_send_flag = 0;
				    }
					if(psh->cellinfo_send_flag >= psh->gs_config_info.cell_send_freq * 50){
						transfercontrl((unsigned char *)(&psh->gs_cellinfo_send),(unsigned char *)(&psh->gs_transferbuf),0xb113,psh->gs_config_info.totalcellcnt*3+86);
						udpsend(psh->gs_transferbuf,psh->gs_config_info.totalcellcnt*3+102);
						usleep(1000);
						psh->cellinfo_send_flag = 0;
					}
					if(psh->govinfo_send_flag >= 300000){
						transfercontrl((unsigned char *)(&psh->gs_govinfo_send),(unsigned char *)(&psh->gs_transferbuf),0xb114,33);
						udpsend(psh->gs_transferbuf,49);
						usleep(1000);
						psh->govinfo_send_flag = 0;
					}

					//usleep(50);
				       /*Ô¶³ÌÅäÖÃ*/
				    printf ("psh->gs_remotecfg_recvflag =%d\n",psh->gs_remotecfg_recvflag);
					if (psh->gs_remotecfg_recvflag == 1){
						psh->gs_remotecfg_recvflag = 0;
						transfercontrl((unsigned char *)(&psh->gs_remotecfg_sendall),(unsigned char *)(&psh->gs_transferbuf),0xb114,49);
						udpsend(psh->gs_transferbuf,65);
					}
					
					if (psh->gs_remotecfg_recvflag == 2){
						psh->gs_remotecfg_recvflag = 0;
						transfercontrl((unsigned char *)(&psh->gs_recvbuf),(unsigned char *)(&psh->gs_transferbuf),0xb115,49);
						udpsend(psh->gs_transferbuf,65);
					}
                }
            }
#endif
            } //end select
}
}
    
    /* Process 5 recv data from sever */ 
//    psh->pid_server_recv = fork(); 
//    if (psh->pid_server_recv < 0){
//        fprintf(stderr, "server_recv: create child process error\n");            
//        return;               
//    }   
//    else if (psh->pid_server_recv == 0) {
//        while (1) {
//            usleep(1000);    
//            printf("\n***************************udp recving*********************************\n");
//            // memset (psh->gs_recvbuf,0,512); 
//            udprecv(psh->gs_recvbuf,psh->can_fd); 
//        }     
//    }

	close (psh->can_fd);
}

void gxgk_free()
{
	/* kill process to get system stat */
	kill(psh->pid_time_count, SIGKILL);
	kill(psh->pid_caninfo_get, SIGKILL);
	kill(psh->pid_gpsinfo_get, SIGKILL);
	kill(psh->pid_data_send, SIGKILL);
	kill(psh->pid_server_recv, SIGKILL);

    /* free global share memory */
	munmap(psh, sizeof(gxgk_stru_share));
}

int main(int argc, char ** argv[]) 

{
	int status_machine = GX4G_SM_MAIN ;
	
    gxgk_init();
    for( ;; )
        sleep(10); 

/*
	while(1){
		switch(status_machine){
			case GX4G_SM_MAIN: gxgk_init();
                               usleep(500000);
                               status_machine = GX4G_SM_RUN;
		                       break;
			case GX4G_SM_RUN:  usleep(500000);
			                   printf("flying!!!!!!!\n");
			                   break;
			case GX4G_SM_CONFIGURE:	
                               usleep(500000);
                               status_machine = GX4G_SM_MAIN;
				               break;

            default: usleep(500000);
		             break;
		}

	}
*/
}
