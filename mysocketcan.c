/*********************Copyright(c)************************************************************************
 * mysocketcan.c
 *
 * date:20160509
 *
*********************************************************************************************************/
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include "gx4g.h"

#define RETRYCOUNT	10

unsigned char bmu_cnt=0;
unsigned short cell_vtg1,cell_vtg2,cell_vtg3,cell_vtg4;
//unsigned short caseenable[9]  = {0,40,80,120,160,200,248,296,336};// 8 xiang che
//static unsigned short caseenable[10] = {0,32,64,96,128,172,216,260,304,336};// 9 xiang che

static unsigned short caseenable[10] = {0,32,64,112,144,180,216,252,288,336};//jiu 9 xiang che
//extern char revstr[];

unsigned int cell_total_cnt = 24; 



void print_frame(struct can_frame *fr)
{
	int i;
	printf("%08x\n", fr->can_id & CAN_EFF_MASK);
//	printf("%08x\n", fr->can_id);
	printf("dlc = %d\n", fr->can_dlc);
	printf("data = ");
	for (i = 0; i < fr->can_dlc; i++)
		printf("%02x ", fr->data[i]);
	printf("\n");
}

void handle_err_frame(struct can_frame *fr)
{
	if (fr->can_id & CAN_ERR_TX_TIMEOUT){
		errout("CAN_ERR_TX_TIMEOUT");
	}
	if (fr->can_id & CAN_ERR_LOSTARB) {
		errout("CAN_ERR_LOSTARB");
		errcode(fr->data[0]);
	}
	if (fr->can_id & CAN_ERR_CRTL) {
		errout("CAN_ERR_CRTL");
		errcode(fr->data[1]);
	}
	if (fr->can_id & CAN_ERR_PROT) {
		errout("CAN_ERR_PROT");
		errcode(fr->data[2]);
		errcode(fr->data[3]);
	}
	if (fr->can_id & CAN_ERR_TRX) {
		errout("CAN_ERR_TRX");
		errcode(fr->data[4]);
	}
	if (fr->can_id & CAN_ERR_ACK) {
		errout("CAN_ERR_ACK");
	}
	if (fr->can_id & CAN_ERR_BUSOFF) {
		errout("CAN_ERR_BUSOFF");
	}
	if (fr->can_id & CAN_ERR_BUSERROR) {
		errout("CAN_ERR_BUSERROR");
	}
	if (fr->can_id & CAN_ERR_RESTARTED) {
		errout("CAN_ERR_RESTARTED");
	}
}

int pack_send(int cmd, int fd,unsigned char *str)
{
        int i=0;
	struct can_frame fr;
	psh->can_time_count = 0;
	psh->recv_flag=0;
	
	fr.can_id  = 0x98f203f0;
	fr.can_dlc = 8;	
	switch (cmd){
	case INF1:
		psh->wait_flag = INF1;
		fr.can_id = 0x98f103f0;

		fr.data[0] = 0x47;
		fr.data[1] = 0x58;
		fr.data[2] = 0x42;
		fr.data[3] = 0x4d;
		fr.data[4] = 0x53;
		fr.data[5] = 0;
		fr.data[6] = 0;
		break;

	case INF3:
		psh->wait_flag = INF3;

		fr.data[0] = 0x41;
		fr.data[1] = *(str+ 14)|0x80;//bmu=5
		fr.data[2] = *(str+ 16);     //soc_h=232*0.4= 93
		fr.data[3] = *(str+ 15)|0x80;
		fr.data[4] = *(str+ 18);     //soc_l=54*0.4 = 22
		fr.data[5] = *(str+ 17)|0x80;	
		fr.data[6] = 0;
		break;

	case INF4:
		psh->wait_flag = INF4;

		fr.data[0] = 0x42; 
		fr.data[1] = *(str+20);                     
		fr.data[2] = *(str+19)|0x80;//soc superlowalarm=4
		fr.data[3] = *(str+22);
		fr.data[4] = *(str+21)|0x80;//cell lowT=-10
		fr.data[5] = *(str+24);
		fr.data[6] = *(str+23)|0x80;//cell highT=61
		break;	

	case INF5:
		psh->wait_flag = INF5;
		
		fr.data[0] = 0x43;
		fr.data[1] = *(str+ 26);     //guochong-l=3.9
		fr.data[2] = *(str+ 25)|0x80;//guochong-h
		fr.data[3] = *(str+ 28);     //gaoya-l=3.7
		fr.data[4] = *(str+ 27)|0x80;//gaoya-h
		fr.data[5] = *(str+ 30);     //qianya-l=2.7
		fr.data[6] = *(str+ 29)|0x80;//qianya-h
		break;	

	case INF6:
		psh->wait_flag = INF6;
		
		fr.data[0] = 0x44;
		fr.data[1] = *(str+ 32);     //guofang-l=2.6
		fr.data[2] = *(str+ 31)|0x80;//guofang-h
		fr.data[3] = *(str+ 34);     //biaochen-l=800
		fr.data[4] = *(str+ 33)|0x80;//biaochen-h
		fr.data[5] = *(str+ 36);     //zongrongliang-l=900
		fr.data[6] = *(str+ 35)|0x80;//zongrongliang-h
		break;	

	case INF7:
		psh->wait_flag = INF7;
		
		fr.data[0] = 0x45;
		fr.data[1] = *(str+ 38);     //shenyurongliang-l=200
		fr.data[2] = *(str+ 37)|0x80;
		fr.data[3] = *(str+ 40);     //chixuchongdianliu-l=250
		fr.data[4] = *(str+ 39)|0x80;
		fr.data[5] = *(str+ 42);     //zuidashunhui-l=400
		fr.data[6] = *(str+ 41)|0x80;	
		break;	

	case INF8:
		psh->wait_flag = INF8;
		
		fr.data[0] = 0x46;
		fr.data[1] = *(str+ 44);     //zuidafangdianliu-l=600
		fr.data[2] = *(str+ 43)|0x80;
		fr.data[3] = *(str+ 46);     //zuidashunhui-l=260
		fr.data[4] = *(str+ 45)|0x80;
		fr.data[5] = *(str+ 48);     //zuidafanggonglv-l=300
		fr.data[6] = *(str+ 47)|0x80;	
		break;

	case INF9:
		psh->wait_flag = INF9;
		
		fr.data[0] = 0x47;
		fr.data[1] = *(str+ 50);     //zuigaochongdianya-l=600
		fr.data[2] = *(str+ 49)|0x80;
		fr.data[3] = *(str+ 52);     //zuidifangdianya-l=400
		fr.data[4] = *(str+ 51)|0x80;
		fr.data[5] = 0;
		fr.data[6] = 0;	
		break;	

	case INF10:
		psh->wait_flag = INF10;
		
		fr.data[0] = 0x48;
		fr.data[1] = *(str+ 54);     //huoermandianliu-l=600
		fr.data[2] = *(str+ 53)|0x80;
		fr.data[3] = *(str+ 56);     //huoermandianya-l=4.25
		fr.data[4] = *(str+ 55)|0x80;
		fr.data[5] = *(str+ 58);     //huoerzengyi-l=2.567
		fr.data[6] = *(str+ 57)|0x80;	
		break;	

	case INF13:
		psh->wait_flag = INF13;
		
		fr.data[0] = 0x4b;
		fr.data[1] = 0x00;//dont use 0x01
		fr.data[2] = 0x00;
		fr.data[3] = 0x00;
		fr.data[4] = 0x00;
		fr.data[5] = 0x00;
		fr.data[6] = 0x00;	
		break;	
		
//	case INF18:
//		psh->wait_flag = INF18;
//	
//		
//		fr.data[0] = 0x50;
//		fr.data[1] = *(str+ 59);    //jiare youxiao
//	//	fr.data[1] = 0xaa;//jiare pinbi
//
//		fr.data[2] = *(str+ 60);//chongdian youxiao
//	//	fr.data[2] = 0xaa;//chongdian tingzhi
//		fr.data[3] =  *(str+ 61);
//		fr.data[4] = 0x00;
//		fr.data[5] = 0x00;
//		fr.data[6] = 0x00;	
//		break;	

//wj:cg for o! chglib,20161213
	case INF18:
		psh->wait_flag = INF18;
	
		fr.can_id = 0x98ff0983;
		
		fr.data[0] = 0x50;
		fr.data[1] = *(str+ 59);    //jiare youxiao
	//	fr.data[1] = 0xaa;//jiare pinbi

		fr.data[2] = *(str+ 60);//chongdian youxiao
	//	fr.data[2] = 0xaa;//chongdian tingzhi
		fr.data[3] =  *(str+ 61);
		fr.data[4] = 0x00;
		fr.data[5] = 0x00;
		fr.data[6] = 0x00;	
		break;	

	case INF19:
		psh->wait_flag = INF19;
		
		fr.data[0] = 0x51;
		fr.data[1] = *(str+ 62);//dianchichuanshu=183
		fr.data[2] = 0x00;
		fr.data[3] = 0x00;
		fr.data[4] = 0x00;
		fr.data[5] = 0x00;
		fr.data[6] = 0x00;	
		break;	

	default:
		break;

	}

	fr.data[7] = 0;
	for(i = 0; i < 7; i++){
		fr.data[7]+=fr.data[i];
	}
	write(fd, &fr, sizeof(fr));
	usleep(10000);
	while(psh->recv_flag != 1) {
		usleep(10000);

		if(psh->wait_flag == INF13){
			if(psh->can_time_count > 100) { 
				printf("77777777777777777777777777777777777777777777\n");
				return -1;
			}
		}
		else{
			if(psh->can_time_count > 45) { 
				printf("999999999999999999999999999999999999999999999999999\n");
				printf("psh->can_time_count = %d\n",psh->can_time_count );
				return -1;
			}
		}

	}

	printf("8888888888888888888888888888888888888888888888888\n");
	return 1;

}

int pack_send_test(int cmd, int fd)
{
        int i=0;
	struct can_frame fr;
	psh->can_time_count = 0;
	psh->recv_flag=0;
	
	fr.can_id  = 0x98f203f0;
	fr.can_dlc = 8;	
	switch (cmd){
	case INF1:
		psh->wait_flag = INF1;
		fr.can_id = 0x98f103f0;

		fr.data[0] = 0x47;
		fr.data[1] = 0x58;
		fr.data[2] = 0x42;
		fr.data[3] = 0x4d;
		fr.data[4] = 0x53;
		fr.data[5] = 0;
		fr.data[6] = 0;
		break;
	case INF3:
		psh->wait_flag = INF3;

		fr.data[0] = 0x41;
		fr.data[1] = psh->gs_recvbuf[10]|0x80;//bmu=5
		fr.data[2] = psh->gs_recvbuf[12];     //soc_h=232*0.4= 93
		fr.data[3] = psh->gs_recvbuf[11]|0x80;
		fr.data[4] = psh->gs_recvbuf[14];     //soc_l=54*0.4 = 22
		fr.data[5] = psh->gs_recvbuf[13]|0x80;	
		fr.data[6] = 0;
		break;
	case INF4:
		psh->wait_flag = INF4;

		fr.data[0] = 0x42; 
		fr.data[1] = psh->gs_recvbuf[16];                     
		fr.data[2] = psh->gs_recvbuf[15]|0x80;//soc superlowalarm=4
		fr.data[3] = psh->gs_recvbuf[18];
		fr.data[4] = psh->gs_recvbuf[17]|0x80;//cell lowT=-10
		fr.data[5] = psh->gs_recvbuf[20];
		fr.data[6] = psh->gs_recvbuf[19]|0x80;//cell highT=61
		break;	
	case INF5:
		psh->wait_flag = INF5;
		
		fr.data[0] = 0x43;
		fr.data[1] = psh->gs_recvbuf[22];     //guochong-l=3.9
		fr.data[2] = psh->gs_recvbuf[21]|0x80;//guochong-h
		fr.data[3] = psh->gs_recvbuf[24];     //gaoya-l=3.7
		fr.data[4] = psh->gs_recvbuf[23]|0x80;//gaoya-h
		fr.data[5] = psh->gs_recvbuf[26];     //qianya-l=2.7
		fr.data[6] = psh->gs_recvbuf[25]|0x80;//qianya-h

		break;	
	case INF6:
		psh->wait_flag = INF6;
		
		fr.data[0] = 0x44;
		fr.data[1] = psh->gs_recvbuf[28];     //guofang-l=2.6
		fr.data[2] = psh->gs_recvbuf[27]|0x80;//guofang-h
		fr.data[3] = psh->gs_recvbuf[30];     //biaochen-l=800
		fr.data[4] = psh->gs_recvbuf[29]|0x80;//biaochen-h
		fr.data[5] = psh->gs_recvbuf[32];     //zongrongliang-l=900
		fr.data[6] = psh->gs_recvbuf[31]|0x80;//zongrongliang-h
		break;	
	case INF7:
		psh->wait_flag = INF7;
		
		fr.data[0] = 0x45;
		fr.data[1] = psh->gs_recvbuf[34];     //shenyurongliang-l=200
		fr.data[2] = psh->gs_recvbuf[33]|0x80;
		fr.data[3] = psh->gs_recvbuf[36];     //chixuchongdianliu-l=250
		fr.data[4] = psh->gs_recvbuf[35]|0x80;
		fr.data[5] = psh->gs_recvbuf[38];     //zuidashunhui-l=400
		fr.data[6] = psh->gs_recvbuf[37]|0x80;	
		break;	
	case INF8:
		psh->wait_flag = INF8;
		
		fr.data[0] = 0x46;
		fr.data[1] = psh->gs_recvbuf[40];     //zuidafangdianliu-l=600
		fr.data[2] = psh->gs_recvbuf[39]|0x80;
		fr.data[3] = psh->gs_recvbuf[42];     //zuidashunhui-l=260
		fr.data[4] = psh->gs_recvbuf[41]|0x80;
		fr.data[5] = psh->gs_recvbuf[44];     //zuidafanggonglv-l=300
		fr.data[6] = psh->gs_recvbuf[43]|0x80;	
		break;	
	case INF9:
		psh->wait_flag = INF9;
		
		fr.data[0] = 0x47;
		fr.data[1] = psh->gs_recvbuf[46];     //zuigaochongdianya-l=600
		fr.data[2] = psh->gs_recvbuf[45]|0x80;
		fr.data[3] = psh->gs_recvbuf[48];     //zuidifangdianya-l=400
		fr.data[4] = psh->gs_recvbuf[47]|0x80;
		fr.data[5] = 0;
		fr.data[6] = 0;	
		break;	
	case INF10:
		psh->wait_flag = INF10;
		
		fr.data[0] = 0x48;
		fr.data[1] = psh->gs_recvbuf[50];     //huoermandianliu-l=600
		fr.data[2] = psh->gs_recvbuf[49]|0x80;
		fr.data[3] = psh->gs_recvbuf[52];     //huoermandianya-l=4.25
		fr.data[4] = psh->gs_recvbuf[51]|0x80;
		fr.data[5] = psh->gs_recvbuf[54];     //huoerzengyi-l=2.567
		fr.data[6] = psh->gs_recvbuf[53]|0x80;	
		break;	


	case INF13:
		psh->wait_flag = INF13;
		
		fr.data[0] = 0x4b;
		fr.data[1] = 0x00;//dont use 0x01
		fr.data[2] = 0x00;
		fr.data[3] = 0x00;
		fr.data[4] = 0x00;
		fr.data[5] = 0x00;
		fr.data[6] = 0x00;	
		break;	
		
//	case INF18:
//		psh->wait_flag = INF18;
//	
//		fr.data[0] = 0x50;
//		fr.data[1] = psh->gs_recvbuf[55];    //jiare youxiao
//	//	fr.data[1] = 0xaa;//jiare pinbi
//
//		fr.data[2] = psh->gs_recvbuf[56]; //chongdian youxiao
//	//	fr.data[2] = 0xaa;//chongdian tingzhi
//	
//		fr.data[3] = psh->gs_recvbuf[57]; //jiare guzhang
//		//fr.data[3] = 0x00; //jiare guzhang
//		fr.data[4] = 0x00;
//		fr.data[5] = 0x00;
//		fr.data[6] = 0x00;	
//		break;	

//wj: cg for test without lib to chg.
	case INF18:
		psh->wait_flag = INF18;
	
		fr.can_id = 0x98ff0983;
		fr.data[0] = 0x50;
		fr.data[1] = psh->gs_recvbuf[55];    //jiare youxiao
	//	fr.data[1] = 0xaa;//jiare pinbi

		fr.data[2] = psh->gs_recvbuf[56]; //chongdian youxiao
	//	fr.data[2] = 0xaa;//chongdian tingzhi
	
		fr.data[3] = psh->gs_recvbuf[57]; //jiare guzhang
		//fr.data[3] = 0x00; //jiare guzhang
		fr.data[4] = 0x00;
		fr.data[5] = 0x00;
		fr.data[6] = 0x00;	
		break;	

	case INF19:
		psh->wait_flag = INF19;
		
		fr.data[0] = 0x51;
		fr.data[1] = psh->gs_recvbuf[58];//dianchichuanshu=183
		fr.data[2] = 0x00;
		fr.data[3] = 0x00;
		fr.data[4] = 0x00;
		fr.data[5] = 0x00;
		fr.data[6] = 0x00;	
		break;	
	default:
		break;

	}

	fr.data[7] = 0;
	for(i = 0; i < 7; i++){
		fr.data[7]+=fr.data[i];
	}
	write(fd, &fr, sizeof(fr));
	usleep(10000);
	while(psh->recv_flag != 1) {
		usleep(10000);

		if(psh->wait_flag == INF13){
			if(psh->can_time_count > 200) { 
				printf("77777777777777777777777777777777777777777777\n");
				return -1;
			}
		}
		else{
			if(psh->can_time_count > 45) { 
				printf("999999999999999999999999999999999999999999999999999\n");
				printf("psh->can_time_count = %d\n",psh->can_time_count );
				return -1;
			}
		}

	}

	printf("8888888888888888888888888888888888888888888888888\n");
	return 1;

}




int pack_send_retry_test(int cmd,int fd)
{
	int i,ret = 0;
	if (cmd == INF13){
		psh->recv_flag_inf3 = 0;
		psh->recv_flag_inf4 = 0;
		psh->recv_flag_inf5 = 0;
		psh->recv_flag_inf6 = 0;
		psh->recv_flag_inf7 = 0;
		psh->recv_flag_inf8 = 0;
		psh->recv_flag_inf9 = 0;
		psh->recv_flag_inf10 = 0;
		psh->recv_flag_inf18 = 0;
		psh->recv_flag_inf19 = 0;
	}
	for(i = 0; i < RETRYCOUNT; i++) {

		ret = pack_send_test(cmd, fd);

		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ret=%d\n",ret);
		if(ret > 0){
			printf("4444444444444444444444444444444444444444444\n");
			return 1;
		}
	}

	printf("55555555555555555555555555555555555555555555555\n");
	return -1;
}


int pack_send_retry(int cmd,int fd,unsigned char *str)
{
	int i,ret = 0;
	if (cmd == INF13){
		psh->recv_flag_inf3 = 0;
		psh->recv_flag_inf4 = 0;
		psh->recv_flag_inf5 = 0;
		psh->recv_flag_inf6 = 0;
		psh->recv_flag_inf7 = 0;
		psh->recv_flag_inf8 = 0;
		psh->recv_flag_inf9 = 0;
		psh->recv_flag_inf10 = 0;
		psh->recv_flag_inf18 = 0;
		psh->recv_flag_inf19 = 0;
	}
	for(i = 0; i < RETRYCOUNT; i++) {

		ret = pack_send(cmd, fd,str);

		printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ret=%d\n",ret);
		if(ret > 0){
			printf("4444444444444444444444444444444444444444444\n");
			return 1;
		}
	}

	printf("55555555555555555555555555555555555555555555555\n");
	return -1;
}


/*wj 0803
 consider only 2 posbilities:
 1.recv in the time dur.
 2.not recv means time out.
*/

int can_remote_config(int fd,unsigned char *str)
{
	int ret,count;
	struct can_frame fr;
	
	for (count = 0;count < 10; count++ ){
		ret = pack_send(INF1, fd,str);
		if(ret > 0){
			printf("11111111111111111111111111111111111111111111111111\n");
			printf("handshake success!!!\n");
			if (pack_send_retry(INF3,fd,str) < 0){
	                       printf("22222222222222222222222222222222222222222222222222222\n");
			       continue; 
			}	       
			printf("333333333333333333333333333333333333333333333333333333\n");
			if (pack_send_retry(INF4,fd,str) < 0){
			       continue;	
			}	
			if (pack_send_retry(INF5,fd,str) < 0){
			       continue;	
			}
			if (pack_send_retry(INF6,fd,str) < 0){
			       continue;	
			}
			if (pack_send_retry(INF7,fd,str) < 0){
			       continue;	
			}
			if (pack_send_retry(INF8,fd,str) < 0){
			       continue;	
			}
			if (pack_send_retry(INF9,fd,str) < 0){
			       continue;	
			}
			if (pack_send_retry(INF10,fd,str) < 0){
			       continue;	
			}
			if (pack_send_retry(INF18,fd,str) < 0){
			       continue;	
			}
			if (pack_send_retry(INF19,fd,str) < 0){
			       continue;	
			}
                        return 1;
	//		break;
		}
		
		else {
			printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
			usleep(50000);
		}
	}
	
	printf("??????????????????????????????????????handshake failed!!!\n");
	return 0;
}

int can_remote_config_test(int fd)
{
	int ret,count;
	struct can_frame fr;

	for (count = 0;count < 10; count++ ){
			if (pack_send_retry_test(INF3,fd) < 0){
	                       printf("2222222222222222222222222\n");
			       continue; 
			}	       
			printf("3333333333333333333333\n");
			if (pack_send_retry_test(INF4,fd) < 0){
			       continue;	
			}	
			if (pack_send_retry_test(INF5,fd) < 0){
			       continue;	
			}
			if (pack_send_retry_test(INF6,fd) < 0){
			       continue;	
			}
			if (pack_send_retry_test(INF7,fd) < 0){
			       continue;	
			}
			if (pack_send_retry_test(INF8,fd) < 0){
			       continue;	
			}
			if (pack_send_retry_test(INF9,fd) < 0){
			       continue;	
			}
			if (pack_send_retry_test(INF10,fd) < 0){
			       continue;	
			}
			if (pack_send_retry_test(INF18,fd) < 0){
			       continue;	
			}
			if (pack_send_retry_test(INF19,fd) < 0){
			       continue;	
			}

			break;
	}
	
	return 0;
}

/* 2018-05-07 
* load sd info to init more params
* need to support expanding
*/
#if 0
void can_up_config ()
{ 
        int i;
	FILE *fp;

        //unsigned char *buf = (unsigned char *) malloc(128 * sizeof (unsigned char));
	//if (buf == NULL) printf("malloc failed!\n");
	//memset(buf,0,128 * sizeof (unsigned char));
        
	unsigned char buf[50];
	bzero(buf,50);
	if((fp = fopen(MY_PARAMS,"r" )) == NULL){
		printf("gxgk params save error!\n" );
		return;
	}
        printf ("sd card data open ok!\n");
	
	fread (&buf,sizeof(buf),1,fp);
      
       /* for 16bit id*/	

	for (i = 0;i < 40;i ++){
		printf("buf[%d]= %c\n",i,buf[i]);
	}
        if(buf[37]!='\0'){
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!222222222\n");
		psh->gs_config_info.bwtversion = 0x91;

		//wj: change data in sd card.
		for(i=0;i<sizeof(buf);i++){
			if(buf[i]>='A' && buf[i]<='F'){
				buf[i] -= 55;
			}
			else if(buf[i]>='a' && buf[i]<='f'){
				buf[i] -= 87;
			}
			else if(buf[i]>='0' && buf[i]<='9'){
				buf[i] -= 48;
			}
			printf("%d",buf[i]);
		}
		printf("\n");

		usleep (100000);


		printf("*buf=%d\n*buf+1=%d\n",*buf,*(buf+1));

		for (i=0; i<8; i++){
			psh->gs_bwtaddr[i] = buf[2*i]*16+buf[2*i+1];
			printf("%d",psh->gs_bwtaddr[i]);
		}

		psh->gs_config_info.bmucount       = *(buf+16)*16 + *(buf+17);
		psh->totalcellcnt = *(buf+18)*16 + *(buf+19);
		printf("%d",*(buf+18));

		for (i=0; i<12; i++){
			psh->gs_config_info.cellcntperbmu[i] =*(buf+20+i*2)*16 + *(buf+21+i*2);
			printf("%d",psh->gs_config_info.cellcntperbmu[i]);
		}
	}
	/* for 8bit id*/	
	else {
		psh->gs_config_info.bwtversion = 0x90;
		printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11111111111\n");
		//wj: change data in sd card.
		for(i=0;i<sizeof(buf);i++){
			if(buf[i]>='A' && buf[i]<='F'){
				buf[i] -= 55;
			}
			else if(buf[i]>='a' && buf[i]<='f'){
				buf[i] -= 87;
			}
			else if(buf[i]>='0' && buf[i]<='9'){
				buf[i] -= 48;
			}
			printf("%d",buf[i]);
		}
		printf("\n");

		usleep (100000);


		printf("*buf=%d\n*buf+1=%d\n",*buf,*(buf+1));

		for (i=0; i<4; i++){
			psh->gs_bwtaddr[i] = buf[2*i]*16+buf[2*i+1];
			printf("%d",psh->gs_bwtaddr[i]);
		}

		psh->gs_config_info.bmucount       = *(buf+8)*16 + *(buf+9);
		psh->totalcellcnt = *(buf+10)*16 + *(buf+11);
		printf("%d",*(buf+18));

		for (i=0; i<12; i++){
			psh->gs_config_info.cellcntperbmu[i] =*(buf+12+i*2)*16 + *(buf+13+i*2);
			printf("%d",psh->gs_config_info.cellcntperbmu[i]);
		}
	}

        printf("\n######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
        printf("######################################################\n");
      	printf ("bmucnt = %d\n",psh->gs_config_info.bmucount);
	printf ("totalcellcnt = %d\n",psh->totalcellcnt);

	fclose (fp);
	
}
#endif

int can_read(int fd)
{
	int ret, i;
	fd_set readfs;
	struct timeval tv;
	struct can_frame frdup;
	int cnt = 0;

	printf("------------------------ \n");

    tv.tv_sec = 1;
	tv.tv_usec = 0;
	FD_ZERO(&readfs);
	FD_SET(fd,&readfs);
	ret =select(fd+1,&readfs,NULL,NULL,&tv);
	if(ret == 0){
		myerr("socketcan select timeout\n");
        /*  to do
         *       can sleep
         */
        psh->can_recv_flag = 0;
		return -1;
	}

	ret = read(fd, &frdup, sizeof(frdup));
	if (ret < sizeof (frdup) ) {
		myerr("read failed");
		return -1;
	}
    psh->can_recv_flag = 1;
	printf("can read cnt is %d\n", ret);

	can_data_deal(&frdup);
	
	if (frdup.can_id & CAN_ERR_FLAG) { /* Âá∫ÈîôËÆæÂ§áÈîôËØØ */
		handle_err_frame(&frdup);
		myerr("CAN device error");
		return -1;
	}
//	print_frame(&frdup);

	return 0;	
}

void can_data_deal(struct can_frame *fr)
{ 
	unsigned long  fcan_id;
	unsigned long  canid;
	unsigned short index;
	unsigned long  mileage,energy;
	unsigned char  currentcase = 1;

	unsigned int   crc;
	unsigned int   i;
	unsigned short SA = 0;
	unsigned char  volIndex;
	unsigned char  mesID = 0;

	unsigned int   nomalVtg;
	unsigned long  nomTemp;
      
    unsigned char  g_i=0;	
    unsigned char  g_bmu_cell_id=0;
//	unsigned char bmu_total_cnt=0;
//	unsigned char cell_total_cnt=104;

//	unsigned char idbuf[] = {2,2,2,2,2,2,2,2};
//	unsigned char bmubuf[] = {0,5,1,0,4};
//	unsigned char cellbuf[] = {2,4,2,4,2,3,2,2,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    static	unsigned char id_flag   = 0;
    static	unsigned char bmu_flag  = 0;
	static  unsigned char celll_flag = 0;
	static  unsigned char cellh_flag = 0;
	
	struct can_frame frdup;

	FILE *fp;

	fcan_id = fr->can_id & CAN_EFF_MASK;


    printf("here deal candata \n");
	if(fcan_id == 0x18f1f003 && psh->wait_flag == INF1 ){
	//	crc = 0;
	//	for (i=0;i<7;i++){
	//		
	//		crc+=fr->data[i];		
	//	}
	//	if(crc == fr->data[7])
	//	{
			psh->recv_flag = 1;
	//	}
	}

	if(fcan_id == 0x18f2f003){
		if(fr->data[0] == 0x41 && psh->wait_flag == INF3 ){
			psh->recv_flag = 1;
		}
		if(fr->data[0] == 0x42 && psh->wait_flag == INF4 ){
			psh->recv_flag = 1;
		}
		if(fr->data[0] == 0x43 && psh->wait_flag == INF5 ){
			psh->recv_flag = 1;
		}
		if(fr->data[0] == 0x44 && psh->wait_flag == INF6 ){
			psh->recv_flag = 1;
		}
		if(fr->data[0] == 0x45 && psh->wait_flag == INF7 ){
			psh->recv_flag = 1;
		}
		if(fr->data[0] == 0x46 && psh->wait_flag == INF8 ){
			psh->recv_flag = 1;
		}
		if(fr->data[0] == 0x47 && psh->wait_flag == INF9 ){
			psh->recv_flag = 1;
		}
		if(fr->data[0] == 0x48 && psh->wait_flag == INF10){
			psh->recv_flag = 1;
		}	
		if(fr->data[0] == 0x50 && psh->wait_flag == INF18){
			psh->recv_flag = 1;
		}
		if(fr->data[0] == 0x51 && psh->wait_flag == INF19){
			psh->recv_flag = 1;
		}

	        if( psh->wait_flag == INF13){

			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
			if (fr->data[0] == 0x41){
				psh->recv_flag_inf3 =1;
                psh->gs_remotecfg_send.bmucnt    = fr->data[1];
				psh->gs_remotecfg_send.hsoc[0]   = fr->data[3];
				psh->gs_remotecfg_send.hsoc[1]   = fr->data[2];
				psh->gs_remotecfg_send.lsoc[0]   = fr->data[5];
				psh->gs_remotecfg_send.lsoc[1]   = fr->data[4];
				printf("∑???????????????????????\n");
			}
			if (fr->data[0] == 0x42){
				psh->recv_flag_inf4 =1;
                psh->gs_remotecfg_send.ulsoc[0]  = fr->data[2];
                psh->gs_remotecfg_send.ulsoc[1]  = fr->data[1];
				psh->gs_remotecfg_send.lt[0]     = fr->data[4];
				psh->gs_remotecfg_send.lt[1]     = fr->data[3];
				psh->gs_remotecfg_send.ht[0]     = fr->data[6];
				psh->gs_remotecfg_send.ht[1]     = fr->data[5];
			}
			if (fr->data[0] == 0x43){ 

				psh->recv_flag_inf5 =1;
				printf("@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
                psh->gs_remotecfg_send.ocv[0]    = fr->data[2];
                psh->gs_remotecfg_send.ocv[1]    = fr->data[1];
				psh->gs_remotecfg_send.hv[0]     = fr->data[4];
				psh->gs_remotecfg_send.hv[1]     = fr->data[3];
				psh->gs_remotecfg_send.lv[0]     = fr->data[6];
				psh->gs_remotecfg_send.lv[1]     = fr->data[5];
			}
			if (fr->data[0] == 0x44){ 

				psh->recv_flag_inf6 =1;
                psh->gs_remotecfg_send.odv[0]           = fr->data[2];
                psh->gs_remotecfg_send.odv[1]           = fr->data[1];
				psh->gs_remotecfg_send.nonimalcap[0]    = fr->data[4];
				psh->gs_remotecfg_send.nonimalcap[1]    = fr->data[3];
				psh->gs_remotecfg_send.totalcap[0]      = fr->data[6];
				psh->gs_remotecfg_send.totalcap[1]      = fr->data[5];
			}
			if (fr->data[0] == 0x45){                                                    

				psh->recv_flag_inf7 =1;
                psh->gs_remotecfg_send.remaincap[0]     = fr->data[2];
                psh->gs_remotecfg_send.remaincap[1]     = fr->data[1];
				psh->gs_remotecfg_send.cstchgcur[0]     = fr->data[4];
				psh->gs_remotecfg_send.cstchgcur[1]     = fr->data[3];
				psh->gs_remotecfg_send.maxchgcur[0]     = fr->data[6];
				psh->gs_remotecfg_send.maxchgcur[1]     = fr->data[5];
			}
			if (fr->data[0] == 0x46){                                                    
  
				psh->recv_flag_inf8 =1;
                psh->gs_remotecfg_send.maxdchgcur[0]    = fr->data[2];
                psh->gs_remotecfg_send.maxdchgcur[1]    = fr->data[1];
				psh->gs_remotecfg_send.maxchgpwr[0]     = fr->data[4];
				psh->gs_remotecfg_send.maxchgpwr[1]     = fr->data[3];
				psh->gs_remotecfg_send.maxdchgpwr[0]    = fr->data[6];
				psh->gs_remotecfg_send.maxdchgpwr[1]    = fr->data[5];
			}
			if (fr->data[0] == 0x47){  

				psh->recv_flag_inf9 =1;
                psh->gs_remotecfg_send.maxchgvolt[0]    = fr->data[2];
                psh->gs_remotecfg_send.maxchgvolt[1]    = fr->data[1];
				psh->gs_remotecfg_send.mindchgvolt[0]   = fr->data[4];
				psh->gs_remotecfg_send.mindchgvolt[1]   = fr->data[3];
			}
			if (fr->data[0] == 0x48){   
				
				psh->recv_flag_inf10 =1;
                psh->gs_remotecfg_send.hofullcur[0]     = fr->data[2];
                psh->gs_remotecfg_send.hofullcur[1]     = fr->data[1];
				psh->gs_remotecfg_send.hofullvolt[0]    = fr->data[4];
				psh->gs_remotecfg_send.hofullvolt[1]    = fr->data[3];
				psh->gs_remotecfg_send.hogain[0]        = fr->data[6];
				psh->gs_remotecfg_send.hogain[1]        = fr->data[5];
			}
			if (fr->data[0] == 0x50){   

				psh->recv_flag_inf18 =1;
                psh->gs_remotecfg_send.chgwarmctl       =fr->data[1];
				psh->gs_remotecfg_send.chgctl           =fr->data[2];
				psh->gs_remotecfg_send.warmbrk          =fr->data[3];

			}

            if (fr->data[0] == 0x51){

				psh->recv_flag_inf19 =1;
				psh->gs_remotecfg_send.cellseries       =fr->data[1];
			}

                
            printf("psh->recv_flag_inf3 = %d\n", psh->recv_flag_inf3);  
            printf("psh->recv_flag_inf4 = %d\n", psh->recv_flag_inf4);  
            printf("psh->recv_flag_inf5 = %d\n", psh->recv_flag_inf5);  
            printf("psh->recv_flag_inf6 = %d\n", psh->recv_flag_inf6);  
            printf("psh->recv_flag_inf7 = %d\n", psh->recv_flag_inf7);  
            printf("psh->recv_flag_inf8 = %d\n", psh->recv_flag_inf8);  
            printf("psh->recv_flag_inf9 = %d\n", psh->recv_flag_inf9);  
            printf("psh->recv_flag_inf10 = %d\n", psh->recv_flag_inf10);  
            printf("psh->recv_flag_inf18 = %d\n", psh->recv_flag_inf18);  
            printf("psh->recv_flag_inf19 = %d\n", psh->recv_flag_inf19);  
		
			if (psh->recv_flag_inf3 ==1 && psh->recv_flag_inf4 ==1&&
			    psh->recv_flag_inf5 ==1 && psh->recv_flag_inf6 ==1&&
			    psh->recv_flag_inf7 ==1 && psh->recv_flag_inf8 ==1&&
			    psh->recv_flag_inf9 ==1 && psh->recv_flag_inf10 ==1&&
			    psh->recv_flag_inf18 ==1 && psh->recv_flag_inf19 ==1){
				psh->recv_flag_inf3 = 0;
				psh->recv_flag_inf4 = 0;
				psh->recv_flag_inf5 = 0;
				psh->recv_flag_inf6 = 0;
				psh->recv_flag_inf7 = 0;
				psh->recv_flag_inf8 = 0;
				psh->recv_flag_inf9 = 0;
				psh->recv_flag_inf10 = 0;
				psh->recv_flag_inf18 = 0;
				psh->recv_flag_inf19 = 0;

				psh->gs_remotecfg_recvflag =1;
				psh->recv_flag = 1;
			}

		}	
	}	

 	if(fcan_id == 0x104c1865){

		print_frame(fr);

		psh->gs_extrminfo_recv.batterytv[0] = fr->data[1];
		psh->gs_extrminfo_recv.batterytv[1] = fr->data[0];

printf("psh->gs_extrminfo_recv.batterytv[0]=%d\n",psh->gs_extrminfo_recv.batterytv[0]);
		// to gov.
		psh->gs_govinfo_recv.batterytv[0] = fr->data[1];
		psh->gs_govinfo_recv.batterytv[1] = fr->data[0];
		
		index = ( fr->data[3] * 256 + fr->data[2] ) + 3200;
		psh->gs_extrminfo_recv.current[0] = (unsigned char)( index >> 8);
		psh->gs_extrminfo_recv.current[1] = (unsigned char) index ;
		
		//to gov.
		psh->gs_govinfo_recv.current[0] = (unsigned char)( index >> 8);
		psh->gs_govinfo_recv.current[1] = (unsigned char) index ;

		psh->gs_extrminfo_recv.soc[0] = 0x00;
		psh->gs_extrminfo_recv.soc[1] = fr->data[7] * 4 /10;
		
		//to gov.
		psh->gs_govinfo_recv.soc[0] = 0x00;
		psh->gs_govinfo_recv.soc[1] = fr->data[7] * 4 /10;
	}


	if(fcan_id == 0x104c1866){
	
		psh->gs_extrminfo_recv.maxcellvolt[0]   = fr->data[1];
		psh->gs_extrminfo_recv.maxcellvolt[1]   = fr->data[0];
        psh->gs_extrminfo_recv.maxcellvoltno[0] = fr->data[2];
        psh->gs_extrminfo_recv.maxcellvoltno[1] = fr->data[3];
		
		psh->gs_extrminfo_recv.mincellvolt[0]   = fr->data[5];
		psh->gs_extrminfo_recv.mincellvolt[1]   = fr->data[4];
		psh->gs_extrminfo_recv.mincellvoltno[0] = fr->data[6];
        psh->gs_extrminfo_recv.mincellvoltno[1] = fr->data[7];
	}
	if(fcan_id == 0x104c1867){		

		psh->gs_extrminfo_recv.maxtempera    = fr->data[0];
		psh->gs_extrminfo_recv.maxtemperano  = 0;
		psh->gs_extrminfo_recv.maxtemperano |= fr->data[1] << 3;
		psh->gs_extrminfo_recv.maxtemperano |= (1 & 0x07) ;

		psh->gs_extrminfo_recv.mintempera    = fr->data[2];
		psh->gs_extrminfo_recv.mintemperano  = 0;
		psh->gs_extrminfo_recv.mintemperano |= fr->data[3] << 3;
		psh->gs_extrminfo_recv.mintemperano |= (1 & 0x07) ;
	}

	if(fcan_id == 0x18ffd017){

		mileage = (unsigned long)(fr->data[7] << 24) |
                  (unsigned long)(fr->data[6] << 16) |
                  (unsigned long)(fr->data[5] <<  8) |
                  (unsigned long) fr->data[4];
        mileage = mileage * 5 / 1000;

		psh->gs_carinfo_recv.totalmile[0] = (unsigned char)(mileage >> 24);
		psh->gs_carinfo_recv.totalmile[1] = (unsigned char)(mileage >> 16);
		psh->gs_carinfo_recv.totalmile[2] = (unsigned char)(mileage >> 8);
		psh->gs_carinfo_recv.totalmile[3] = (unsigned char) mileage;

		//to gov.
		psh->gs_govinfo_recv.totalmile[0] = (unsigned char)(mileage >> 24);
		psh->gs_govinfo_recv.totalmile[1] = (unsigned char)(mileage >> 16);
		psh->gs_govinfo_recv.totalmile[2] = (unsigned char)(mileage >> 8);
    printf("###0006");
		psh->gs_govinfo_recv.totalmile[3] = (unsigned char) mileage;
	}	

	if(fcan_id == 0x104c1864){

		mileage = (unsigned long)(fr->data[3] << 24) |
                  (unsigned long)(fr->data[2] << 16) |
                  (unsigned long)(fr->data[1] <<  8) |
                  (unsigned long) fr->data[0];
        mileage = mileage / 10;

		psh->gs_carinfo_recv.totalmile[0] = (unsigned char)(mileage >> 24);
		psh->gs_carinfo_recv.totalmile[1] = (unsigned char)(mileage >> 16);
		psh->gs_carinfo_recv.totalmile[2] = (unsigned char)(mileage >> 8);
		psh->gs_carinfo_recv.totalmile[3] = (unsigned char) mileage;

		//to gov.
		psh->gs_govinfo_recv.totalmile[0] = (unsigned char)(mileage >> 24);
		psh->gs_govinfo_recv.totalmile[1] = (unsigned char)(mileage >> 16);
		psh->gs_govinfo_recv.totalmile[2] = (unsigned char)(mileage >> 8);
		psh->gs_govinfo_recv.totalmile[3] = (unsigned char) mileage;

		energy =  (unsigned long)(fr->data[7] << 24) |
                  (unsigned long)(fr->data[6] << 16) |
                  (unsigned long)(fr->data[5] <<  8) |
                  (unsigned long) fr->data[4];
        energy = energy / 10;

		psh->gs_carinfo_recv.totalusedenergy[0] = (unsigned char)(energy >> 24);
		psh->gs_carinfo_recv.totalusedenergy[1] = (unsigned char)(energy >> 16);
		psh->gs_carinfo_recv.totalusedenergy[2] = (unsigned char)(energy >> 8);
		psh->gs_carinfo_recv.totalusedenergy[3] = (unsigned char) energy;
	}

	if(fcan_id == 0x18fef1a6){

		psh->gs_carinfo_recv.speed = fr->data[0];
		//to gov.
		psh->gs_govinfo_recv.speed = fr->data[0];
	
		psh->gs_carinfo_recv.remainmile[0] = 0;
		psh->gs_carinfo_recv.remainmile[1] = fr->data[1];
            
        /*	
		index = ( fr->data[3] * 256 + fr->data[2]) / 10;
		psh->gs_carinfo_recv.rings[0] = (unsigned char)(index >> 8);
		psh->gs_carinfo_recv.rings[1] = (unsigned char) index ;
		*/
		psh->gs_carinfo_recv.rings[0] = fr->data[2];
		psh->gs_carinfo_recv.rings[1] = fr->data[3];

		psh->gs_carinfo_recv.tmtcon = fr->data[4];
	}

	if(fcan_id == 0x18fef2a6){

		psh->gs_carinfo_recv.motorspeed[0] = fr->data[1];
		psh->gs_carinfo_recv.motorspeed[1] = fr->data[0];
		
		//to gov.
		psh->gs_govinfo_recv.motorspeed[0] = fr->data[1];
		psh->gs_govinfo_recv.motorspeed[1] = fr->data[0];

		psh->gs_carinfo_recv.mttorque[0] = fr->data[3];
		psh->gs_carinfo_recv.mttorque[1] = fr->data[2];

        psh->gs_carinfo_recv.imtcon[0] = fr->data[5];
		psh->gs_carinfo_recv.imtcon[1] = fr->data[4];
	}

	/*
	 * wj:add for make config as *id#,*bmu#,*cell#.
	 * 0926 accord to deal with huazai.
	*/
	if(fcan_id == 0x18200005){
	
		if(fr->data[0] == 0x40){
			//can_up_config ();
			load_config(CONFIG_FILE_PATH);	
            usleep(200*1000);
			
			frdup.can_id  = 0x98200006;
			frdup.can_dlc = 8;	
            frdup.data[0]= 0x41;
            frdup.data[7]= 0;
			for(i=1;i<7;i++){
				frdup.data[i] = psh->gs_config_info.bwtaddr[i-1];
			}
			write(psh->can_fd, &frdup, sizeof(frdup));

			frdup.data[0] = 0x42;
			frdup.data[1] = psh->gs_config_info.bwtaddr[6];
			frdup.data[2] = psh->gs_config_info.bwtaddr[7];
			frdup.data[3] = psh->gs_config_info.bmucount;
			frdup.data[4] = psh->gs_config_info.totalcellcnt;
			frdup.data[5] = 0;
			frdup.data[6] = 0;
			write(psh->can_fd, &frdup, sizeof(frdup));
            usleep(200*1000);

			frdup.data[0] = 0x43;
			for(i= 1; i<7; i++){
				frdup.data[i] = psh->gs_config_info.cellcntperbmu[i-1];
			}
			write(psh->can_fd, &frdup, sizeof(frdup));
            usleep(200*1000);

			frdup.data[0] = 0x44;
			for(i= 1; i<7; i++){
				frdup.data[i]= psh->gs_config_info.cellcntperbmu[i+5];
			}
			write(psh->can_fd, &frdup, sizeof(frdup));
            usleep(200*1000);
		}	

		if(fr->data[0] == 0x41){
			for(i=1;i<7;i++){
				psh->gs_config_info.bwtaddr[i-1] = fr->data[i];
			}
            id_flag = 1;
		}
		if(fr->data[0] == 0x42){
			psh->gs_config_info.bwtaddr[6] = fr->data[1];
			psh->gs_config_info.bwtaddr[7] = fr->data[2];
			
			psh->gs_config_info.bmucount      = fr->data[3];
			psh->gs_config_info.totalcellcnt  = fr->data[4];

			bmu_flag = 1;
		}
		if(fr->data[0] == 0x43){
			for(i= 1; i<7; i++){
				psh->gs_config_info.cellcntperbmu[i-1] = fr->data[i];
			}

			celll_flag = 1;
		}
		if(fr->data[0] == 0x44){
			for(i= 1; i<7; i++){
				psh->gs_config_info.cellcntperbmu[i+5] = fr->data[i];
			}
			
			cellh_flag = 1;
		}	
                
        if((id_flag == 1) && (bmu_flag == 1) && (celll_flag == 1) && (cellh_flag == 1)){

			if((fp = fopen(MY_PARAMS,"w+" )) == NULL){
				printf("gxgk params save error!\n" );
				return;
			}
			printf ("sd card open ok!\n");
			//fwrite (&save_params,sizeof(save_params) ,1,fp);
			for(i=0; i<8; i++){
				fprintf (fp,"%02x",psh->gs_config_info.bwtaddr[i]);
			}
			fprintf (fp,"%02x%02x",psh->gs_config_info.bmucount,psh->gs_config_info.totalcellcnt);
			for (i = 0; i<12; i++){
				fprintf (fp,"%02x",psh->gs_config_info.cellcntperbmu[i]);
			}
			usleep (100000);
			id_flag    = 0;
		        bmu_flag   = 0;
		       	celll_flag = 0;
			cellh_flag = 0;
			
			fclose (fp);
	       }
	}


       // nomalVtg = (psh->gs_extrminfo_recv.batterytv[0]*256 +  psh->gs_extrminfo_recv.batterytv[1]) *100/ psh->totalcellcnt;
        nomalVtg = (psh->gs_extrminfo_recv.batterytv[0]*256 +  psh->gs_extrminfo_recv.batterytv[1]) *100/ psh->gs_config_info.totalcellcnt;
       
        psh->gs_extrminfo_recv.averagecellvoltage[0] = (unsigned char)(nomalVtg >> 8);
        psh->gs_extrminfo_recv.averagecellvoltage[1] = (unsigned char)nomalVtg;
       	
        nomalVtg = 0;

        psh->gs_extrminfo_recv.averagetemp = (psh->gs_extrminfo_recv.maxtempera + psh->gs_extrminfo_recv.mintempera)/2;
	// to gov.
        psh->gs_govinfo_recv.outtemp = (psh->gs_extrminfo_recv.maxtempera + psh->gs_extrminfo_recv.mintempera)/2;
        psh->gs_govinfo_recv.avertemp[1] = psh->gs_govinfo_recv.outtemp;
        psh->gs_govinfo_recv.avertemp[0] = 0;
	

	psh->gs_extrminfo_recv.tempsub = psh->gs_extrminfo_recv.maxtempera - psh->gs_extrminfo_recv.mintempera;

	nomTemp = psh->gs_extrminfo_recv.maxcellvolt[0]*256 + psh->gs_extrminfo_recv.maxcellvolt[1] - psh->gs_extrminfo_recv.mincellvolt[0]*256 - psh->gs_extrminfo_recv.mincellvolt[1];
        
    psh->gs_extrminfo_recv.vtgsub[0] = (unsigned char)(nomTemp >> 8);
    psh->gs_extrminfo_recv.vtgsub[1] = (unsigned char)nomTemp;
	
	nomTemp = 0;


//	psh->gs_cellinfo_recv.cellchkpt[0] = (unsigned char)(psh->totalcellcnt>> 8);
//  psh->gs_cellinfo_recv.cellchkpt[1] = (unsigned char)psh->totalcellcnt;
//	psh->gs_cellinfo_recv.tempchkpt[0] = (unsigned char)(psh->totalcellcnt>> 8);
//  psh->gs_cellinfo_recv.tempchkpt[1] = (unsigned char)psh->totalcellcnt;
	psh->gs_cellinfo_recv.cellchkpt[0] = (unsigned char)(psh->gs_config_info.totalcellcnt>> 8);
    psh->gs_cellinfo_recv.cellchkpt[1] = (unsigned char)psh->gs_config_info.totalcellcnt;
	psh->gs_cellinfo_recv.tempchkpt[0] = (unsigned char)(psh->gs_config_info.totalcellcnt>> 8);
    psh->gs_cellinfo_recv.tempchkpt[1] = (unsigned char)psh->gs_config_info.totalcellcnt;

	/* biaozhunhua yuancheng jiankong chengxu*/ 
	bmu_cell_num_start[0]=0;

	for (g_i=1; g_i<psh->gs_config_info.bmucount; g_i++){
		bmu_cell_num_start[g_i]= bmu_cell_num_start[g_i-1]+ psh->gs_config_info.cellcntperbmu[g_i-1]*2; 
	}

        for (g_i=0; g_i<psh->gs_config_info.bmucount; g_i++){
		if(psh->gs_config_info.cellcntperbmu[g_i]>32){
			psh->gs_cellinfo_recv.modenable[g_i+g_bmu_cell_id]=0xffffffff;
			g_bmu_cell_id++;
			psh->gs_cellinfo_recv.modenable[g_i+g_bmu_cell_id]=(1<<(psh->gs_config_info.cellcntperbmu[g_i]-32))-1;
		}
		else if (psh->gs_config_info.cellcntperbmu[g_i]==32){
			psh->gs_cellinfo_recv.modenable[g_i+g_bmu_cell_id]=0xffffffff;
		}
		else if((psh->gs_config_info.cellcntperbmu[g_i]>0)&& (psh->gs_config_info.cellcntperbmu[g_i]<32)){
			psh->gs_cellinfo_recv.modenable[g_i+g_bmu_cell_id]=(1<<psh->gs_config_info.cellcntperbmu[g_i])-1;	
		}
	}
	
    if((psh->gs_config_info.bmucount+g_bmu_cell_id)<=8){
		psh->gs_cellinfo_recv.caseenable=((1<<(psh->gs_config_info.bmucount+g_bmu_cell_id))-1)<<24;
	}
	else{
		psh->gs_cellinfo_recv.caseenable=0xff000000|(((1<<(psh->gs_config_info.bmucount+g_bmu_cell_id-8))-1)<<16);
	}
	/*/

	/*  cell info modify
	//
	// µ•ÃÂµÁ—π
	//
	if((fcan_id & 0x0000ffff) == 0x000028f3 ){
		cell_vtg1 =  ((fr->data[1] << 8 | fr->data[0]) & 0x0fff) * 10;
		cell_vtg2 =  ((fr->data[3] << 8 | fr->data[2]) & 0x0fff) * 10;
		cell_vtg3 =  ((fr->data[5] << 8 | fr->data[4]) & 0x0fff) * 10;
		cell_vtg4 =  ((fr->data[7] << 8 | fr->data[6]) & 0x0fff) * 10;

		canid = (fcan_id >> 16) & 0x0f;

		currentcase = (fr->data[1] & 0xf0 ) >> 4;

		if(cell_vtg1 > 0){
			currentcase = (fr->data[1] & 0xf0 ) >> 4;

			psh->gs_cellinfo_recv.cellvtg[canid * 8]      = (unsigned char)(cell_vtg1 >> 8);
			psh->gs_cellinfo_recv.cellvtg[canid * 8 + 1]  = (unsigned char)(cell_vtg1);

			if(cell_vtg2 > 0){
				psh->gs_cellinfo_recv.cellvtg[ canid * 8 + 2] = (unsigned char)(cell_vtg2 >> 8);
				psh->gs_cellinfo_recv.cellvtg[ canid * 8 + 3] = (unsigned char)(cell_vtg2);
			}

			if(cell_vtg3 > 0){
				psh->gs_cellinfo_recv.cellvtg[ canid * 8 + 4] = (unsigned char)(cell_vtg3 >> 8);
				psh->gs_cellinfo_recv.cellvtg[ canid * 8 + 5] = (unsigned char)(cell_vtg3);
			}

			if(cell_vtg4 > 0){
				psh->gs_cellinfo_recv.cellvtg[ canid * 8 + 6] = (unsigned char)(cell_vtg4 >> 8);
				psh->gs_cellinfo_recv.cellvtg[ canid * 8 + 7] = (unsigned char)(cell_vtg4);
			}
		}
	}
*/
	
	if((fcan_id & 0x0000ffff) == 0x000028f3 ){
		cell_vtg1 =  ((fr->data[1] << 8 | fr->data[0]) & 0x0fff) * 10;
		cell_vtg2 =  ((fr->data[3] << 8 | fr->data[2]) & 0x0fff) * 10;
		cell_vtg3 =  ((fr->data[5] << 8 | fr->data[4]) & 0x0fff) * 10;
		cell_vtg4 =  ((fr->data[7] << 8 | fr->data[6]) & 0x0fff) * 10;

		canid = (fcan_id >> 16) & 0x0f;

		currentcase = (fr->data[1] & 0xf0 ) >> 4;

		if(cell_vtg1 > 0){
			currentcase = (fr->data[1] & 0xf0 ) >> 4;

			psh->gs_cellinfo_recv.cellvtg[caseenable[currentcase - 1] + canid * 8]      = (unsigned char)(cell_vtg1 >> 8);
			psh->gs_cellinfo_recv.cellvtg[caseenable[currentcase - 1] + canid * 8 + 1]  = (unsigned char)(cell_vtg1);

			if(cell_vtg2 > 0){
				psh->gs_cellinfo_recv.cellvtg[caseenable[currentcase - 1] + canid * 8 + 2] = (unsigned char)(cell_vtg2 >> 8);
				psh->gs_cellinfo_recv.cellvtg[caseenable[currentcase - 1] + canid * 8 + 3] = (unsigned char)(cell_vtg2);
			}

			if(cell_vtg3 > 0){
				psh->gs_cellinfo_recv.cellvtg[caseenable[currentcase - 1] + canid * 8 + 4] = (unsigned char)(cell_vtg3 >> 8);
				psh->gs_cellinfo_recv.cellvtg[caseenable[currentcase - 1] + canid * 8 + 5] = (unsigned char)(cell_vtg3);
			}

			if(cell_vtg4 > 0){
				psh->gs_cellinfo_recv.cellvtg[caseenable[currentcase - 1] + canid * 8 + 6] = (unsigned char)(cell_vtg4 >> 8);
				psh->gs_cellinfo_recv.cellvtg[caseenable[currentcase - 1] + canid * 8 + 7] = (unsigned char)(cell_vtg4);
			}
		}
	}

	//
	// µ•ÃÂŒ¬∂»
	//
	if((fcan_id & 0x0000ffff) == 0x000028f4 ){

		canid = (fcan_id >> 16) & 0x0f;

		if(canid < 8){
			psh->gs_cellinfo_recv.casetemp[caseenable[canid] / 2 + 0] =  fr->data[0];
			psh->gs_cellinfo_recv.casetemp[caseenable[canid] / 2 + 1] =  fr->data[1];
			psh->gs_cellinfo_recv.casetemp[caseenable[canid] / 2 + 2] =  fr->data[2];
			psh->gs_cellinfo_recv.casetemp[caseenable[canid] / 2 + 3] =  fr->data[3];
		}
	}


/************************************************************************************************************************************
 * xwj 2017 03-21
 * delet cellinfo
 * for taiwanTR
 *
	bmu_cell_num_start[0]=0;

	for (g_i=1; g_i<psh->gs_config_info.bmucount; g_i++){
		bmu_cell_num_start[g_i]= bmu_cell_num_start[g_i-1]+ psh->gs_config_info.cellcntperbmu[g_i-1]*2; 
	}

        for (g_i=0; g_i<psh->gs_config_info.bmucount; g_i++){
		if(psh->gs_config_info.cellcntperbmu[g_i]>32){
			psh->gs_cellinfo_recv.modenable[g_i+g_bmu_cell_id]=0xffffffff;
			g_bmu_cell_id++;
			psh->gs_cellinfo_recv.modenable[g_i+g_bmu_cell_id]=(1<<(psh->gs_config_info.cellcntperbmu[g_i]-32))-1;
		}
		else if (psh->gs_config_info.cellcntperbmu[g_i]==32){
			psh->gs_cellinfo_recv.modenable[g_i+g_bmu_cell_id]=0xffffffff;
		}
		else if((psh->gs_config_info.cellcntperbmu[g_i]>0)&& (psh->gs_config_info.cellcntperbmu[g_i]<32)){
			psh->gs_cellinfo_recv.modenable[g_i+g_bmu_cell_id]=(1<<psh->gs_config_info.cellcntperbmu[g_i])-1;	
		}
	}

	if((psh->gs_config_info.bmucount+g_bmu_cell_id)<=8){
		psh->gs_cellinfo_recv.caseenable=((1<<(psh->gs_config_info.bmucount+g_bmu_cell_id))-1)<<24;
	}
	else{
		psh->gs_cellinfo_recv.caseenable=0xff000000|(((1<<(psh->gs_config_info.bmucount+g_bmu_cell_id-8))-1)<<16);
	}


        if((fcan_id & 0xFFF0FF00) == 0x18004100){
		SA = (unsigned short) (fcan_id & 0x000000FF)-1;
		volIndex = (unsigned char )((fcan_id & 0x000F0000) >> 16) - 1;

		for (mesID = 0; mesID < 8; mesID ++){
			if (mesID % 2 == 0){
				if (fr->data[mesID + 1] > 0){
					psh->gs_cellinfo_recv.cellvtg[bmu_cell_num_start[SA] + volIndex * 8 + mesID]= fr->data[mesID + 1];
				}
			}
			else{
			      	if (fr->data[mesID - 1] > 0){
					psh->gs_cellinfo_recv.cellvtg[bmu_cell_num_start[SA] + volIndex * 8 + mesID]= fr->data[mesID - 1];
				}
			}
		}

	}

//        if((fcan_id & 0xFFFFFF00) == 0x18104100){
//		psh->gs_cellinfo_recv.casetemp[]
//	
//	}	

*************************************************************************************************************************************/

	printf ("data is here\n");

      //staticerrorchk();
	memcpy(&psh->gs_extrminfo_send, &psh->gs_extrminfo_recv, sizeof(extrm_info));
	memcpy(&psh->gs_cellinfo_send, &psh->gs_cellinfo_recv, sizeof(cell_info));
	memcpy(&psh->gs_carinfo_send, &psh->gs_carinfo_recv, sizeof(car_info));
	memcpy(&psh->gs_govinfo_send, &psh->gs_govinfo_recv, sizeof(gov_info));

	memcpy(&psh->gs_remotecfg_sendall, &psh->gs_remotecfg_send, sizeof(remotecfg_info));

//	transfercontrl((unsigned char *)(&psh->gs_remotecfg_sendall),(unsigned char *)(&psh->gs_transferbuf),0xb210,48);
//	udpsend(psh->gs_transferbuf,60);


}


void staticerrorchk(void)
{
	unsigned short totalvtg_a,totalamp_a,maxvtg_a,minvtg_a;

	totalvtg_a = psh->gs_extrminfo_recv.batterytv[0] * 256 + psh->gs_extrminfo_recv.batterytv[1];
	totalamp_a = psh->gs_extrminfo_recv.current[0] * 256 + psh->gs_extrminfo_recv.current[1];
	maxvtg_a   = psh->gs_extrminfo_recv.maxcellvolt[0] * 256 + psh->gs_extrminfo_recv.maxcellvolt[1];
	minvtg_a   = psh->gs_extrminfo_recv.mincellvolt[0] * 256 + psh->gs_extrminfo_recv.mincellvolt[1];

//	BYTE 0															BYTE 1
//	Bit47	Bit46	Bit45	Bit44	Bit43	Bit42	Bit41	Bit40	Bit39	Bit38	Bit37	Bit36	Bit35	Bit34	Bit33	Bit32
//	

//	BYTE 2															BYTE 3(‘§æØ±Í÷æŒª)
//	Bit47	Bit46	Bit45	Bit44	Bit43	Bit42	Bit41	Bit40	Bit39	Bit38	Bit37	Bit36	Bit35	Bit34	Bit33	Bit32
//	‘§æØ ±º‰2S/bit  ‘§æØ∑∂Œß0~510√Î								    VOF		TOF		TMAX	TMIN	VSX		VSN		VMAX	VMIN

//	BYTE 4£®π ’œ±Í÷æŒª£©											BYTE 5£®π ’œ±Í÷æŒª£©
//	Bit31	Bit30	Bit29	Bit28	Bit27	Bit26	Bit25	Bit24	Bit23	Bit22	Bit21	Bit20	Bit19	Bit18	Bit17	Bit16
//	VOF				TOF				TMAX			TMIN			VSX				VSN				VMAX			VMIN

//	BYTE 6£®π ’œ±Í÷æŒª£©											BYTE 7£®π ’œ±Í÷æŒª£©
//	Bit15	Bit14	Bit13	Bit12	Bit11	Bit10	Bit9	Bit8	Bit7	Bit6	Bit5	Bit4	Bit3	Bit2	Bit1	Bit0
//	RML				MTC		MOT		ECU		EBM		GPRS	GPS		COMM	BMS		SOCL			IRL	    ±£¡Ù


	//—π≤Óπ˝¥Û(æ≤Ã¨)
	if(totalvtg_a < 3300)  // µÁ¡˜–°”⁄ 10A
	{
		 if(maxvtg_a >= 3400)
		 {
		 	if(minvtg_a <= 3320 && minvtg_a > 3300)
			{
				// —π≤Óπ˝¥Û “ªº∂π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);
				psh->gs_extrminfo_recv.alarminfo[4] |= 1 << 6;
			}
			else if(minvtg_a <= 3300)
			{
				// —π≤Óπ˝¥Û ∂˛º∂π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);
				psh->gs_extrminfo_recv.alarminfo[4] |= 2 << 6;
			}
			else
			{
				// Ω‚≥˝π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);	
			}
		 }
		 else if(maxvtg_a >= 3250 && maxvtg_a < 3350)
		 {
		 	if(maxvtg_a - minvtg_a > 100 && maxvtg_a - minvtg_a <= 200)
			{
				// —π≤Óπ˝¥Û “ªº∂π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);
				psh->gs_extrminfo_recv.alarminfo[4] |= 1 << 6;
			}
			else if(maxvtg_a - minvtg_a > 200)
			{
				// —π≤Óπ˝¥Û ∂˛º∂π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);
				psh->gs_extrminfo_recv.alarminfo[4] |= 2 << 6;
			}
			else
			{
				// Ω‚≥˝π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);	
			}
		 }
		 else if(maxvtg_a >= 3150 && maxvtg_a < 3250)
		 {
		 	if(maxvtg_a - minvtg_a > 200 && maxvtg_a - minvtg_a <= 300)
			{
				// —π≤Óπ˝¥Û “ªº∂π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);
				psh->gs_extrminfo_recv.alarminfo[4] |= 1 << 6;

			}
			else if(maxvtg_a - minvtg_a > 300)
			{
				// —π≤Óπ˝¥Û ∂˛º∂π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);
				psh->gs_extrminfo_recv.alarminfo[4] |= 2 << 6;
			}
			else
			{
				// Ω‚≥˝π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);
			}
		 }
		 else if(maxvtg_a <= 3150)
		 {
		 	if(maxvtg_a - minvtg_a > 500 && maxvtg_a - minvtg_a <= 700)
			{
				// —π≤Óπ˝¥Û “ªº∂π ’œ
			       	psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);
				psh->gs_extrminfo_recv.alarminfo[4] |= 1 << 6;

			}
			else if(maxvtg_a - minvtg_a > 700)
			{
				// —π≤Óπ˝¥Û ∂˛º∂π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);
				psh->gs_extrminfo_recv.alarminfo[4] |= 2 << 6;
			}
			else
			{
				// Ω‚≥˝π ’œ
				psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);

			}
		 }
		 else
		 {
			// Ω‚≥˝π ’œ
			psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 6);		 
		 }
	}

	//µ•ÃÂµÁ—ππ˝∏ﬂ
	if(maxvtg_a >= 3850 && maxvtg_a < 4000)
	{
		//µ•ÃÂµÁ—ππ˝∏ﬂ “ªº∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 2);
		psh->gs_extrminfo_recv.alarminfo[5] |= 1 << 2;
	}
	else if(maxvtg_a >= 4000)
	{
		//µ•ÃÂµÁ—ππ˝∏ﬂ ∂˛º∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 2);
		psh->gs_extrminfo_recv.alarminfo[5] |= 2 << 2;
	}
	else
	{
		// Ω‚≥˝π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 2);
	}

	//µ•ÃÂµÁ—ππ˝µÕ
	if(minvtg_a <= 2500 && minvtg_a > 2000)
	{
		//µ•ÃÂµÁ—ππ˝∏ﬂ “ªº∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 0);
		psh->gs_extrminfo_recv.alarminfo[5] |= 1 << 0;
	}
	else if(minvtg_a <= 2000)
	{
		//µ•ÃÂµÁ—ππ˝∏ﬂ ∂˛º∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 0);
		psh->gs_extrminfo_recv.alarminfo[5] |= 2 << 0;
	}
	else
	{
		// Ω‚≥˝π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 0);
	}

	//Œ¬≤Óπ˝¥Û
	if((	psh->gs_extrminfo_recv.maxtempera - psh->gs_extrminfo_recv.mintempera > 10) && (psh->gs_extrminfo_recv.maxtempera - psh->gs_extrminfo_recv.mintempera  <= 15))
	{
		// Œ¬≤Óπ˝¥Û “ªº∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 4);
		psh->gs_extrminfo_recv.alarminfo[4] |= 1 << 4;
	}
	else if(psh->gs_extrminfo_recv.maxtempera - psh->gs_extrminfo_recv.mintempera > 15)
	{
		// Œ¬≤Óπ˝¥Û∂˛º∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 4);
		psh->gs_extrminfo_recv.alarminfo[4] |= 2 << 4;
	}
	else
	{
		// Ω‚≥˝π ’œ
		psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 4);
	}

	//Œ¬∂»π˝∏ﬂ
	if(psh->gs_extrminfo_recv.maxtempera > 90 && psh->gs_extrminfo_recv.maxtempera <= 95)
	{
		// Œ¬∂»π˝∏ﬂ “ªº∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 2);
		psh->gs_extrminfo_recv.alarminfo[4] |= 1 << 2;
	}
	else if(psh->gs_extrminfo_recv.maxtempera > 95)
	{
		// Œ¬∂»π˝∏ﬂ ∂˛º∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 2);
		psh->gs_extrminfo_recv.alarminfo[4] |= 1 << 2;
	}
	else
	{
		// Ω‚≥˝π ’œ
		psh->gs_extrminfo_recv.alarminfo[4] &= ~(3 << 2);
	}


	//
	// Œ¬∂»π˝µÕ
	//
	if(psh->gs_extrminfo_recv.mintempera < 30 && psh->gs_extrminfo_recv.mintempera >= 20)
	{
		// Œ¬∂»π˝µÕ “ªº∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[4] &= ~3 ;
		psh->gs_extrminfo_recv.alarminfo[4] |= 1 ;
	}
	else if(psh->gs_extrminfo_recv.mintempera < 20)
	{
		// Œ¬∂»π˝µÕ ∂˛º∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[4] &= ~3 ;
		psh->gs_extrminfo_recv.alarminfo[4] |= 2 ;
	}
	else
	{
		// Ω‚≥˝π ’œ
		psh->gs_extrminfo_recv.alarminfo[4] &= ~3 ;
	}

	//
	// ◊‹—ππ˝∏ﬂ
	//
	if( totalvtg_a > cell_total_cnt * 36 && totalvtg_a <= cell_total_cnt * 37)
	{
		//◊‹—ππ˝∏ﬂ “ªº∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 6);
		psh->gs_extrminfo_recv.alarminfo[5] |= 1 << 6;
	}
	else if( totalvtg_a > cell_total_cnt * 37)
	{
		//◊‹—ππ˝∏ﬂ ∂˛º∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 6);
		psh->gs_extrminfo_recv.alarminfo[5] |= 2 << 6;
	}
	else
	{
		//◊‹—ππ˝∏ﬂ “ªº∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 6);
	}

	//
	// ◊‹—ππ˝µÕ
	//
	if( totalvtg_a < cell_total_cnt * 25 && totalvtg_a >= cell_total_cnt * 23)
	{
		//◊‹—ππ˝∏ﬂ “ªº∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 4);
		psh->gs_extrminfo_recv.alarminfo[5] |= 1 << 4;
	}
	else if( totalvtg_a < cell_total_cnt * 23)
	{
		//◊‹—ππ˝∏ﬂ ∂˛º∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 4);
		psh->gs_extrminfo_recv.alarminfo[5] |= 2 << 4;
	}
	else
	{
		//Ω‚≥˝π ’œ
		psh->gs_extrminfo_recv.alarminfo[5] &= ~(3 << 4);
	}

	//
	// SOCπ˝µÕ
	//
	if( psh->gs_extrminfo_recv.soc[1] < 5 && psh->gs_extrminfo_recv.soc[1] >= 1)
	{
		//◊‹—ππ˝∏ﬂ “ªº∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[7] &= ~(3 << 4);
		psh->gs_extrminfo_recv.alarminfo[7] |= 1 << 4;
	}
	else if( psh->gs_extrminfo_recv.soc[1] < 1)
	{
		//◊‹—ππ˝∏ﬂ ∂˛º∂π ’œ
		psh->gs_extrminfo_recv.alarminfo[7] &= ~(3 << 4);
		psh->gs_extrminfo_recv.alarminfo[7] |= 2 << 4;
	}
	else
	{
		//Ω‚≥˝π ’œ
		psh->gs_extrminfo_recv.alarminfo[7] &= ~(3 << 4);
	}
	
	// £”‡¿Ô≥Ãπ˝µÕ
	 
}

int socketcan_init()
{
    	int ret;
    	struct sockaddr_can addr;
    	struct ifreq ifr;
	char can_id;

	srand(time(NULL));
        psh->can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	
    	if (psh->can_fd  < 0) {
		perror("socket PF_CAN failed");
		return 0;
    	}

    	strcpy(ifr.ifr_name, "can0");
    	ret = ioctl(psh->can_fd, SIOCGIFINDEX, &ifr);
    	if (ret < 0) {
		perror("ioctl failed");
		return 0;
    	}

    	addr.can_family = PF_CAN;
    	addr.can_ifindex = ifr.ifr_ifindex;

    	ret = bind(psh->can_fd , (struct sockaddr *)&addr, sizeof(addr));
    	if (ret < 0) {
		perror("bind failed");
		return 0;
	}
	/*
	if (1) {
		struct can_filter filter[6];
    		filter[0].can_id = 0x200001 | CAN_EFF_FLAG;
		filter[0].can_mask = 0xFFFFFF;
	 
		filter[1].can_id = 0x200002 | CAN_EFF_FLAG;
		filter[1].can_mask = 0xFFFFFF;

		filter[2].can_id = 0x200003 | CAN_EFF_FLAG;
		filter[2].can_mask = 0xFFFFFF;
         
		filter[3].can_id = 0x200004 | CAN_EFF_FLAG;
		filter[3].can_mask = 0xFFFFFF;

		filter[4].can_id = 0x200005 | CAN_EFF_FLAG;
		filter[4].can_mask = 0xFFFFFF;

		filter[5].can_id = 0x004100 | CAN_EFF_FLAG;
		filter[5].can_mask = 0xF0FF00;

    		ret = setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));
		if (ret < 0) {
			perror("setsockopt failed");
			return 0;

		}
	}
	*/
      // return s; 
      
}
