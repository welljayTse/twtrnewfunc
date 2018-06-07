#ifndef  GX4G_H
#define  GX4G_H

#include "my4gmodule.h"
#include "mysocketcan.h"
#include "mygps.h"
#include "myuart.h"
#include "myftp.h"
#include "myconfig.h"
#include "mystorage.h"

#define  GXGK_UART_NAME	"/dev/ttySP0" 
#define  TARGET "www.baidu.com"

//#define  GXGK_UART_NAME	"/dev/ttyAM0" 

#define  DEV_NAME "/dev/ttyS0"
#define  MY_PARAMS "/opt/keywords.txt"
#define  CONFIG_FILE_PATH "/opt/config.ini"
//#define  MY_PARAMS "/media/mmcblk0p1/keywords.txt"

#define  GPIO_PATH       "/dev/gpio3-21"
#define  RESET_PATH      "/dev/Pin22-nRESET"
#define  USIN_DET_PATH   "/dev/Pin44-USIM_DET"
#define  VALUE_1          "1"




#define	 BMU_MAX_SERNUM   64	   //最大支持的从机数目
#define  BMU_CELL_CNT	    	60	   //每个从机监控的电池数目
#define	 BMU_TEMP_CNT      6     //每个从机监控的电池温度数目
#define  BMS_CAN_BAUD			BPS_250K
#define	 MAX_TOTAL_CELL		168	  // 最大总串数 
#define	 VTGCHKPOT			MAX_TOTAL_CELL << 8  // 电压检测点
#define	 TEMPCHKPOT			MAX_TOTAL_CELL << 8  // 温度检测点
#define  CELL_INFO_ENDABLE		1
#define  CAR_INFO_ENDABLE		1
#define  EXTREM_INFO_ENDABLE	1
#define  GPS_INFO_ENDABLE		1
#define  CELL_INFO_UPLOAD_TIME	    1801	// 单体信息上传间隔N
#define  CAR_INFO_UPLOAD_TIME	    41		// 整车信息上传间隔
#define  GPS_INFO_UPLOAD_TIME 	    16		// GPS 信息上传间隔
#define  EXTRM_INFO_UPLOAD_TIME  	31	    // 极值信息上传间隔

typedef unsigned char   INT8U;
typedef unsigned short  INT16U;
typedef unsigned int    INT32U;


typedef struct {
	int      uartnum;
	int      count;
}struct_temp_global; 

extern struct_temp_global mytempglobal;


typedef struct {
    INT32U   caseenable;
    INT32U	 modenable[16];
    INT8U 	 cellchkpt[2];
	INT8U	 tempchkpt[2];
	INT8U    cellvtg[MAX_TOTAL_CELL*2];
	INT8U 	 casetemp[MAX_TOTAL_CELL];
	INT8U    alarminfo[8];
	INT8U    time[6]; 
} cell_info;

typedef struct { 
    INT8U   lon[4];	
	INT8U	lat[4];	
	INT8U	cog[2];	
//	INT8U	tall[2];      
	INT8U	spd[2];	               	
	INT8U 	time[6];       
	INT8U	len;       
} gps_info;

typedef struct {

	INT8U   batterytv[2];
	INT8U   current[2];    	 
	INT8U   soc[2];	    	        
	INT8U	maxcellvoltno[2];
	INT8U   maxcellvolt[2];	      	 
	INT8U   mincellvoltno[2];       
	INT8U   mincellvolt[2];  
	INT8U   averagecellvoltage[2];
	INT8U   vtgsub[2];    	       
	INT8U   maxtemperano; 
 	INT8U   maxtempera;    	 
	INT8U   mintemperano;
	INT8U	mintempera;	
	INT8U 	averagetemp;
	INT8U   tempsub;    	        
	INT8U	alarminfo[8];
	INT8U   time[6];    		 
	INT8U   len;
} extrm_info;

typedef struct {
	INT8U   speed;
	INT8U   remainmile[2];	
	INT8U   totalmile[4];
	INT8U   totalusedenergy[4];
	INT8U   rings[2];         
	INT8U   tmtcon;
	INT8U   motorspeed[2];
	INT8U   mttorque[2];        
	INT8U   imtcon[2];    	  
	INT8U   alarmInfo[8];
	INT8U   time[6];         	
	INT8U   len;
} car_info;

typedef struct {

	INT8U   batterytv[2];
	INT8U   current[2];    	 
	INT8U   totalmile[4];
//	INT8U   remainmile[2];	
	INT8U   soc[2];	    	 
	INT8U   speed;

	INT8U 	time[6];       
    INT8U   lon[4];	
	INT8U	lat[4];	
	INT8U	cog[2];	

	INT8U   outtemp;       //waijie wendu  
	INT8U   ymdepth;       //youmen shendu  
	INT8U   motorspeed[2]; //dianji zhuansu
	INT8U   avertemp[2];   //pinjun wendu
	INT8U   len;
} gov_info;

typedef struct {

	INT8U	bmucnt;    
	INT8U	hsoc[2];   
	INT8U	lsoc[2];   
	INT8U	ulsoc[2];  
	INT8U	lt[2];     
	INT8U	ht[2];     
	INT8U	ocv[2];    
	INT8U	hv[2];     
	INT8U	lv[2];     
	INT8U	odv[2];         
	INT8U	nonimalcap[2];  
	INT8U	totalcap[2];    
	INT8U	remaincap[2];   
	INT8U	cstchgcur[2];   
	INT8U	maxchgcur[2];   
	INT8U	maxdchgcur[2];  
	INT8U	maxchgpwr[2];   
	INT8U	maxdchgpwr[2];  
	INT8U	maxchgvolt[2];  
	INT8U	mindchgvolt[2]; 
	INT8U	hofullcur[2];   
	INT8U	hofullvolt[2];  
	INT8U	hogain[2];      
	INT8U	chgwarmctl;     
	INT8U	chgctl;         
	INT8U   warmbrk;
	INT8U	cellseries;    
}remotecfg_info;



typedef struct {
	unsigned char   bwtaddr[8];
	unsigned char   bwtversion;
	unsigned char   cellcntperbmu[12];
   	INT8U           bmucount;
   	INT8U           totalcellcnt;
   	INT8U           extrem_send_freq;//frpuence use 1/s
   	INT8U           extrem_save_freq;
   	INT8U           car_send_freq;
   	INT8U           car_save_freq;
   	INT8U           gps_send_freq;
   	INT8U           gps_save_freq;
   	INT16U          cell_send_freq;
   	INT16U          cell_save_freq;
} config_info;



typedef struct {
	int status_machine;

	extrm_info gs_extrminfo_recv;
	extrm_info gs_extrminfo_send;

	gps_info gs_gpsinfo_recv;
	gps_info gs_gpsinfo_send;
	
	cell_info gs_cellinfo_recv;
	cell_info gs_cellinfo_send;

	car_info gs_carinfo_recv;
	car_info gs_carinfo_send;

	gov_info gs_govinfo_recv;
	gov_info gs_govinfo_send;

	remotecfg_info gs_remotecfg_recv;
	remotecfg_info gs_remotecfg_send;
	remotecfg_info gs_remotecfg_sendall;
    
    config_info gs_config_info;

	int      sockfd;
	struct   sockaddr_in addr_remote;
	struct   sockaddr_in  peer; 
	socklen_t  addrlen;

	int      uartnum;
	int      count;
	int      sd;
	int      pid_server_recv;
	int      pid_caninfo_get;
	int      pid_gpsinfo_get;
    int      pid_data_send ;
    int      pid_time_count;
	int      pid_ping_connect;
	
	int	 can_time_count;
	unsigned char string[256];
	//unsigned char gs_transferbuf[512];
	unsigned char gs_transferbuf[1024];
	unsigned char gs_recvbuf[512];

	int      gs_remotecfg_recvflag;
	int      recv_flag;
	int      wait_flag;
	int      can_fd;
	int      gps_fd;
	int      gps_recv_flag;
    int      can_recv_flag;
	int 	 ftp_fd;
	 
	int recv_flag_inf3;
	int recv_flag_inf4;
	int recv_flag_inf5;
	int recv_flag_inf6;
	int recv_flag_inf7;
	int recv_flag_inf8;
	int recv_flag_inf9;
	int recv_flag_inf10;
	int recv_flag_inf18;
	int recv_flag_inf19;
	

	int	extrminfo_send_flag;
	int	carinfo_send_flag;
	int	cellinfo_send_flag;
	int	gpsinfo_send_flag;
	int	govinfo_send_flag;
	int heartbeat_send_flag;

	int heartbeat_check_count; //set 3,if less than 0 means heartbeat break
	
	int localtime[6];//系统时间

	int aliveflag;//和服务端保持心跳请求应答的标识
	
	int handle;//远程升级本地临时文件的句柄
	int d_sock;//ftp升级句柄

//	unsigned char gs_gpsrecvtmp[256];
//	unsigned char gs_gpsrecvdata[256];
//	unsigned char gs_gpsrecvbuf[256];


}gxgk_stru_share;


/************************************************************************************
 *
 * global functions
 *
 * ********************************************************************************/

void gxgk_share_params_init();

void gxgk_init();
void gxgk_free();

/************************************************************************************
 *
 *  global varibals
 *
 * ********************************************************************************/

//extern cell_info gs_cellinfo;
//extern gps_info gs_gpsinfo ;
//extern extrm_info gs_extrminfo_recv;
extern car_info gs_carinfo ;
extern INT8U bmu_cell_cnt[BMU_CELL_CNT];
extern INT16U bmu_cell_num_start[BMU_CELL_CNT];
extern INT16U cell_cnt_per_bmu[BMU_CELL_CNT];

extern gxgk_stru_share *psh;

extern unsigned int cell_total_cnt;

#define GX4G_SM_MAIN		1
#define GX4G_SM_RUN		2
#define GX4G_SM_CONFIGURE	3

#define GX4G_SM_OVER		100

/*
extern INT8U g_ulBWTVersion; 
extern INT32U g_ulBWTAddr; 
extern unsigned char cell_total_count ;

extern char _uctransferbuf[800];
*/




#endif //nend of GX4G_H
