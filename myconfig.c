#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "gx4g.h"

//#include <configs.h>

#define SYSTEM_CONFIG_FILE         "/system/config.ini"	/* default configfile */
#define SD_CONFIG_FILE         	   "/system/sd_config.ini"	/* default sd configfile */
#define MAX_LINE 256
/*
g_config_t gconf_default = {
	.photo_conf = {
		.photo_size = CONF_PHOTO_SIZE_2M,
	},
	.movie_conf = {
		.movie_size     = CONF_MOVIE_SIZE_1520S,
		.cycle_record   = CONF_CYCLIC_REC_3MIN,
		.record_bitrate = 10000,
	},
	.live_video_conf = {
		.preview_size    = CONF_PREVIEW_SIZE_640S,
		.preview_bitrate = 2000,
	},
	.gsensor_conf = {
		.lock_gsensor_sens    = CONF_LOCK_GSENSOR_SENS_MID,
		.monitor_gsensor_sens = CONF_MONITOR_GSENSOR_OFF,
	},
	.wdr_state   = CONF_WDR_DISABLED,
	.ev_value    = CONF_EV_00,
	.audio_state = CONF_AUDIO_ENABLED,
	.wifi_ssid   = "cardvssid",
	.wifi_pwd    = "12345678",
	.auto_poweroff_time = CONF_POWEROFF_TIME_3MIN,
	.language    = CONF_LANG_EN,
};

g_directory_config_t gconf_dirdefault[4]= {
		{
			.directory_name = "NOR",
			.directory_path = "/mnt/NOR/",
			.directory_function = 0,
			.directory_capacity_limit_type = 1,
			.directory_capacity_percent = 100,
			.directory_capacity_absolution = 0,
			.directory_file_cicle_delete = 1,
			.directory_delete_priority = 0,
			.directory_left_capacity_warning = 1,
		},
		{
			.directory_name = "EVT",
			.directory_path = "/mnt/EVT/",
			.directory_function = 0,
			.directory_capacity_limit_type = 1,
			.directory_capacity_percent = 30,
			.directory_capacity_absolution = 0,
			.directory_file_cicle_delete = 1,
			.directory_delete_priority = 1,
			.directory_left_capacity_warning = 1,
		},
		{
			.directory_name = "PHO",
			.directory_path = "/mnt/PHO/",
			.directory_function = 0,
			.directory_capacity_limit_type = 1,
			.directory_capacity_percent = 100,
			.directory_capacity_absolution = 0,
			.directory_file_cicle_delete = 1,
			.directory_delete_priority = 1,
			.directory_left_capacity_warning = 1,
		},
		{
			.directory_name = ".thumbnail",
			.directory_path = "/mnt/.thumbnail/",
			.directory_function = 0,
			.directory_capacity_limit_type = 1,
			.directory_capacity_percent = 100,
			.directory_capacity_absolution = 0,
			.directory_file_cicle_delete = 1,
			.directory_delete_priority = 1,
			.directory_left_capacity_warning = 1,
		},
};

g_sdconfig_t gconf_sddefault = {
	.format_enable = 1,
	.folder_num = 4,
	.sd_reserved_warning_number = 2,
	.lockvideo_reserved_warning_number = 2,
	.photo_reseved_warning_number = 10,
	.park_reseved_warning_number = 2,
	.directory = gconf_dirdefault,
};
*/

static void writeLogLine(FILE *fp, const char *content)
{
	if (fp == NULL) {
		printf("%s:%d -> config file not open!\n", __func__, __LINE__);
		return;
	}
	fprintf(fp,"%s\n",content);
}

int update_config(const char *path)
{
	FILE *fp;
    int j ,temp=0;
    char buf[17];
    
    bzero(buf,17);

	if ((fp = fopen(path, "w+")) == NULL) {
		printf("ERROR: %s:%d -> open config file [%s] error! use default [%s]\n", __func__, __LINE__, path, SYSTEM_CONFIG_FILE);
		if ((fp = fopen(SYSTEM_CONFIG_FILE, "w+")) == NULL)
		{
			printf("ERROR: %s:%d -> open default config file [%s] error!\n", __func__, __LINE__, SYSTEM_CONFIG_FILE);
			return -1;
		}
	}

	char writeLine[MAX_LINE] = {0};
	/* write head */
	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s", "<-- Start save config file -->\n");
	writeLogLine(fp, writeLine);

	/* write config info */

    //for 16bit id
    if(  psh->gs_config_info.bwtversion = 0x91){

            for(j=0;j<8;j++){
                    temp = psh->gs_config_info.bwtaddr[j]/16;
                    sprintf(&buf[j*2],"%d",temp);
                    
                    temp = psh->gs_config_info.bwtaddr[j]%16;
                    sprintf(&buf[j*2+1],"%d",temp);
          
            }
    }
    else if( psh->gs_config_info.bwtversion = 0x90){

            for(j=0;j<4;j++){
                    temp = psh->gs_config_info.bwtaddr[j]/16;
                    sprintf(buf[j*2],"%d",temp);
                    
                    temp = psh->gs_config_info.bwtaddr[j]%16;
                    sprintf(buf[j*2+1],"%d",temp);
            }
    }

    //printf("buf=%s\n",buf);
	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%s", "id", buf);
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "bmucount", psh->gs_config_info.bmucount) ;
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "celltotalcount", psh->gs_config_info.totalcellcnt);
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_01", psh->gs_config_info.cellcntperbmu[0]);
	writeLogLine(fp, writeLine);
    
    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_02", psh->gs_config_info.cellcntperbmu[1]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);
    
    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_03", psh->gs_config_info.cellcntperbmu[2]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);

    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_04", psh->gs_config_info.cellcntperbmu[3]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);

    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_05", psh->gs_config_info.cellcntperbmu[4]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);
    
    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_06", psh->gs_config_info.cellcntperbmu[5]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);

    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_07", psh->gs_config_info.cellcntperbmu[6]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);
    
    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_08", psh->gs_config_info.cellcntperbmu[7]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);

    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_09", psh->gs_config_info.cellcntperbmu[8]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);

    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_10", psh->gs_config_info.cellcntperbmu[9]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);

    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_11", psh->gs_config_info.cellcntperbmu[10]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);

    memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "cell_12", psh->gs_config_info.cellcntperbmu[11]);
	writeLogLine(fp, writeLine);memset(writeLine, 0x0, MAX_LINE);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "extreminfo_send_frequence", psh->gs_config_info.extrem_send_freq);
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "extreminfo_save_frequence", psh->gs_config_info.extrem_save_freq);
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "carinfo_send_frequence", psh->gs_config_info.car_send_freq);
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "carinfo_save_frequence", psh->gs_config_info.car_save_freq);
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "gpsinfo_send_frequence", psh->gs_config_info.gps_send_freq);
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%d", "gpsinfo_save_frequence", psh->gs_config_info.gps_save_freq);
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%ld", "cellinfo_send_frequence", psh->gs_config_info.cell_send_freq);
	writeLogLine(fp, writeLine);

	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s=%ld", "cellinfo_save_frequence", psh->gs_config_info.cell_save_freq);
	writeLogLine(fp, writeLine);

	/* write tail */
	memset(writeLine, 0x0, MAX_LINE);
	sprintf(writeLine, "%s", "<-- End save config file -->");
	writeLogLine(fp, writeLine);
	writeLogLine(fp, writeLine);
	writeLogLine(fp, writeLine);
	writeLogLine(fp, writeLine);
	writeLogLine(fp, writeLine);
	writeLogLine(fp, writeLine);
	writeLogLine(fp, writeLine);
	writeLogLine(fp, writeLine);

	fclose(fp);

	return 0;
}


int load_config(const char *path)
{
	FILE *fp = NULL;
	char strLine[MAX_LINE];
	char *file_name = path;

	char *delim = "=";
	char *tok = NULL, *p;
	int  prase_value = -1, i = 0,j= 0;
	char *prase_string = NULL;

	char buf[17];

	bzero(buf,sizeof(buf));
    
	if (!file_name || access(file_name, F_OK)) {
		printf("WARNING: %s:%d -> %s not exist, use default config file!\n", \
		    	__func__, __LINE__, path);
	      //  memcpy(gconf, &gconf_default, sizeof(g_config_t));
		return 1;
	}

	if((fp = fopen(file_name,"r")) == NULL) {
		printf("%s:%d -> open %s error!\n", __func__, __LINE__, file_name);
		return -1;
	}

	while (!feof(fp)) {
		fgets(strLine, MAX_LINE, fp);
		/*printf("%s:%d -> RL: %s!\n", __func__, __LINE__, strLine);*/
		i = 0;
		do {
			p = (i++ == 0) ? strLine : NULL;
			tok = strtok(p, delim);
			if (tok) {
				if (!strncmp("id", tok, strlen("id"))) {
	           	    prase_string = tok + strlen("id") + 1;

				    /* leave the last byte "\n" from fgets() */
				    printf("id lenth = %d\n",strlen(prase_string) -1);
				    memcpy(buf, prase_string, strlen(prase_string)-1);
				    
				    for(j=0;j<strlen(buf);j++){
					   if(buf[j]>='A' && buf[j]<='F'){
					       buf[j] -= 55;
					   }
					   else if(buf[j]>='a' && buf[j]<='f'){
					       buf[j] -= 87;
					   }
					   else if(buf[j]>='0' && buf[j]<='9'){
					       buf[j] -= 48;
					   }
				     //     printf("buf[%d]=%d\n",j,buf[j]);
				    }
				    if(strlen(buf) == 16){
					    /* for 16bit id */
					    psh->gs_config_info.bwtversion = 0x91;
					    for (j=0; j<8; j++){
						    psh->gs_config_info.bwtaddr[j] = buf[2*j]*16+buf[2*j+1];
				     //     printf("%d",psh->gs_bwtaddr[j]);
					    }
				    }
				    else if(strlen(buf) == 8){
					    /* for 8bit id */
					    psh->gs_config_info.bwtversion = 0x90;
					    for (j=0; j<4; j++){
						    psh->gs_config_info.bwtaddr[j] = buf[2*j]*16+buf[2*j+1];
				     //     printf("%d",psh->gs_bwtaddr[j]);
					    }
				    }
				    else{
					    printf("config file has error!\n");
				    }
                    //note: here donot need \n,cause prase_string has
                    printf("\tconfig file: parameter = %s, value = %s", tok, prase_string);
				} else if (!strncmp("bmucount", tok, strlen("bmucount"))) {
					prase_string = tok + strlen("bmucount") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.bmucount = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("celltotalcount", tok, strlen("celltotalcount"))) {
					prase_string = tok + strlen("celltotalcount") + 1;
				    prase_value = atoi(prase_string);
				    psh->gs_config_info.totalcellcnt = prase_value;
				    printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cell_01", tok, strlen("cell_01"))) {
					prase_string = tok + strlen("cell_01") + 1;
					prase_value = atoi(prase_string);
            		psh->gs_config_info.cellcntperbmu[0] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cell_02", tok, strlen("cell_02"))) {
					prase_string = tok + strlen("cell_02") + 1;
					prase_value = atoi(prase_string);
            		psh->gs_config_info.cellcntperbmu[1] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
                } else if (!strncmp("cell_03", tok, strlen("cell_03"))) {
                   	prase_string = tok + strlen("cell_03") + 1;
                   	prase_value = atoi(prase_string);
            		psh->gs_config_info.cellcntperbmu[2] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cell_04", tok, strlen("cell_04"))) {
					prase_string = tok + strlen("cell_04") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cellcntperbmu[3] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cell_05", tok, strlen("cell_05"))) {
					prase_string = tok + strlen("cell_05") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cellcntperbmu[4] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				}else if (!strncmp("cell_06", tok, strlen("cell_06"))) {
					prase_string = tok + strlen("cell_06") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cellcntperbmu[5] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cell_07", tok, strlen("cell_07"))) {
					prase_string = tok + strlen("cell_07") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cellcntperbmu[6] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cell_08", tok, strlen("cell_08"))) {
					prase_string = tok + strlen("cell_08") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cellcntperbmu[7] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				}else if (!strncmp("cell_09", tok, strlen("cell_09"))) {
					prase_string = tok + strlen("cell_09") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cellcntperbmu[8] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cell_10", tok, strlen("cell_10"))) {
					prase_string = tok + strlen("cell_10") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cellcntperbmu[9] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cell_11", tok, strlen("cell_11"))) {
					prase_string = tok + strlen("cell_11") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cellcntperbmu[10] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cell_12", tok, strlen("cell_12"))) {
					prase_string = tok + strlen("cell_12") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cellcntperbmu[11] = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("extreminfo_send_frequence", tok, strlen("extreminfo_send_frequence"))) {
					prase_string = tok + strlen("extreminfo_send_frequence") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.extrem_send_freq = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("extreminfo_save_frequence", tok, strlen("extreminfo_save_frequence"))) {
					prase_string = tok + strlen("extreminfo_save_frequence") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.extrem_save_freq = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				}  else if (!strncmp("carinfo_send_frequence", tok, strlen("carinfo_send_frequence"))) {
					prase_string = tok + strlen("carinfo_send_frequence") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.car_send_freq = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("carinfo_save_frequence", tok, strlen("carinfo_save_frequence"))) {
					prase_string = tok + strlen("carinfo_save_frequence") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.car_save_freq = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("gpsinfo_send_frequence", tok, strlen("gpsinfo_send_frequence"))) {
					prase_string = tok + strlen("gpsinfo_send_frequence") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.gps_send_freq = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("gpsinfo_save_frequence", tok, strlen("gpsinfo_save_frequence"))) {
					prase_string = tok + strlen("gpsinfo_save_frequence") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.gps_save_freq = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cellinfo_send_frequence", tok, strlen("cellinfo_send_frequence"))) {
					prase_string = tok + strlen("cellinfo_send_frequence") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cell_send_freq = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("cellinfo_save_frequence", tok, strlen("cellinfo_save_frequence"))) {
					prase_string = tok + strlen("cellinfo_save_frequence") + 1;
					prase_value = atoi(prase_string);
					psh->gs_config_info.cell_save_freq = prase_value;
					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
				}    
                 
//                else if (!strncmp("wdr_state", tok, strlen("wdr_state"))) {
//					prase_string = tok + strlen("wdr_state") + 1;
//					prase_value = atoi(prase_string);
//					gconf->wdr_state = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} else if (!strncmp("ev_value", tok, strlen("ev_value"))) {
//					prase_string = tok + strlen("ev_value") + 1;
//					prase_value = atoi(prase_string);
//					gconf->ev_value = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} else if (!strncmp("audio_state", tok, strlen("audio_state"))) {
//					prase_string = tok + strlen("audio_state") + 1;
//					prase_value = atoi(prase_string);
//					gconf->audio_state = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} else if (!strncmp("preview_size", tok, strlen("preview_size"))) {
//					prase_string = tok + strlen("preview_size") + 1;
//					prase_value = atoi(prase_string);
//					gconf->live_video_conf.preview_size = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} else if (!strncmp("record_bitrate", tok, strlen("record_bitrate"))) {
//					prase_string = tok + strlen("record_bitrate") + 1;
//					prase_value = atoi(prase_string);
//					gconf->movie_conf.record_bitrate = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} else if (!strncmp("preview_bitrate", tok, strlen("preview_bitrate"))) {
//					prase_string = tok + strlen("preview_bitrate") + 1;
//					prase_value = atoi(prase_string);
//					gconf->live_video_conf.preview_bitrate = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} else if (!strncmp("lock_gsensor_sens", tok, strlen("lock_gsensor_sens"))) {
//					prase_string = tok + strlen("lock_gsensor_sens") + 1;
//					prase_value = atoi(prase_string);
//					gconf->gsensor_conf.lock_gsensor_sens = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} else if (!strncmp("monitor_gsensor_sens", tok, strlen("monitor_gsensor_sens"))) {
//					prase_string = tok + strlen("monitor_gsensor_sens") + 1;
//					prase_value = atoi(prase_string);
//					gconf->gsensor_conf.monitor_gsensor_sens = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} else if (!strncmp("wifi_ssid", tok, strlen("wifi_ssid"))) {
//					prase_string = tok + strlen("wifi_ssid") + 1;
//					/*prase_value = atoi(prase_string);*/
//					/*gconf->wifi_ssid = prase_value;*/
//					memcpy(gconf->wifi_ssid, prase_string, strlen(prase_string));
//					printf("\tconfig file: parameter = %s, value = %s\n", tok, prase_string);
//				} else if (!strncmp("wifi_pwd", tok, strlen("wifi_pwd"))) {
//					prase_string = tok + strlen("wifi_pwd") + 1;
//					/*prase_value = atoi(prase_string);*/
//					/*gconf->wifi_pwd = prase_value;*/
//					memcpy(gconf->wifi_pwd, prase_string, strlen(prase_string));
//					printf("\tconfig file: parameter = %s, value = %s\n", tok, prase_string);
//				} else if (!strncmp("auto_poweroff_time", tok, strlen("auto_poweroff_time"))) {
//					prase_string = tok + strlen("auto_poweroff_time") + 1;
//					prase_value = atoi(prase_string);
//					gconf->auto_poweroff_time = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} else if (!strncmp("language", tok, strlen("language"))) {
//					prase_string = tok + strlen("language") + 1;
//					prase_value = atoi(prase_string);
//					gconf->language = prase_value;
//					printf("\tconfig file: parameter = %s, value = %d\n", tok, prase_value);
//				} 
				else {
					/*printf("\tconfig file: parameter = %s\n", tok);*/
				}
			}
		} while (tok);
	}

	fclose(fp);

	return 0;
}

#if 0
void delspace(char* p)
{
	int i,j = 0;
	for(i = 0;p[i] != '\0';i++){
		if(p[i]!='\n')
			p[j++] = p[i];
	}
	p[j] = '\0';
}
/**
 * wj: load sd card info
 */
int load_sd_config(const char *path)
{
	FILE *fp = NULL;
	char strLine[MAX_LINE];
	char *file_name = path;

	char *delim = "=";
	char *tok = NULL, *p;
	int prase_value = -1, i = 0;
	static int prase_index;
	char *prase_string = NULL;


	if (!file_name || access(file_name, F_OK)) {
	printf("WARNING: %s:%d -> %s not exist, use default sd config file!\n", \
		__func__, __LINE__, path);
	memcpy(gsdconf, &gconf_sddefault, sizeof(g_sdconfig_t));
	return 1;
	}

	if((fp = fopen(file_name,"r")) == NULL) {
		printf("%s:%d -> open %s error!\n", __func__, __LINE__, file_name);
		return -1;
	}

	while (!feof(fp)) {
		fgets(strLine, MAX_LINE, fp);
		/*printf("%s:%d -> RL: %s!\n", __func__, __LINE__, strLine);*/
		i = 0;

		do {
			p = (i++ == 0) ? strLine : NULL;
			printf("sdtok0 = %s,i = %d,\n",tok,i);
			tok = strtok(p, delim);
			if (tok) {
				if(!strncmp("folder_num", tok, strlen("folder_num"))) {
					prase_string = tok + strlen("folder_num") + 1;
					prase_value = atoi(prase_string);
					gsdconf->folder_num = prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
					gsdconf->directory = (g_directory_config_t*) malloc(prase_value * sizeof(g_directory_config_t));
					memset(gsdconf->directory, 0x0, prase_value * sizeof(g_directory_config_t));
				} else if (!strncmp("format_enable", tok, strlen("format_enable"))) {
					prase_string = tok + strlen("format_enable") + 1;
					prase_value = atoi(prase_string);
					gsdconf->format_enable = prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("sd_reserved_warning_number", tok, strlen("sd_reserved_warning_number"))) {
					prase_string = tok + strlen("sd_reserved_warning_number") + 1;
					prase_value = atoi(prase_string);
					gsdconf->sd_reserved_warning_number = prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("lockvideo_reserved_warning_number", tok, strlen("lockvideo_reserved_warning_number"))) {
					prase_string = tok + strlen("lockvideo_reserved_warning_number") + 1;
					prase_value = atoi(prase_string);
					gsdconf->lockvideo_reserved_warning_number = prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("photo_reseved_warning_number", tok, strlen("photo_reseved_warning_number"))) {
					prase_string = tok + strlen("photo_reseved_warning_number") + 1;
					prase_value = atoi(prase_string);
					gsdconf->photo_reseved_warning_number = prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("park_reseved_warning_number", tok, strlen("park_reseved_warning_number"))) {
					prase_string = tok + strlen("park_reseved_warning_number") + 1;
					prase_value = atoi(prase_string);
					gsdconf->park_reseved_warning_number = prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				/**
				 * wj: load ecah folder info. eg: directory_name1 = EVT
				 */
				} else if (!strncmp("directory_name", tok, strlen("directory_name"))) {
					prase_string = tok + strlen("directory_name");
					prase_index = atoi(prase_string);
					prase_string = tok + strlen("directory_name") + 2;
					delspace(prase_string);
					printf("after delspace %s\n",prase_string);
					memcpy(gsdconf->directory[prase_index].directory_name, prase_string, strlen(prase_string));
					//gsdconf->directory[prase_index].directory_path = strcat("/mnt",prase_string);
					//gsdconf->directory[prase_index].directory_path = strcat(gsdconf->directory[prase_index].directory_path,"/");
					sprintf(gsdconf->directory[prase_index].directory_path,"/mnt/%s/",prase_string);
					printf("\tsdconfig file: parameter = %s, value = %s\n", tok, prase_string);
				} else if (!strncmp("directory_function", tok, strlen("directory_function"))) {
					prase_string = tok + strlen("directory_function") + 1;
					prase_value = atoi(prase_string);
					gsdconf->directory[prase_index].directory_function= prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("directory_capacity_limit_type", tok, strlen("directory_capacity_limit_type"))) {
					prase_string = tok + strlen("directory_capacity_limit_type") + 1;
					prase_value = atoi(prase_string);
					gsdconf->directory[prase_index].directory_capacity_limit_type= prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("directory_capacity_percent", tok, strlen("directory_capacity_percent"))) {
					prase_string = tok + strlen("directory_capacity_percent") + 1;
					prase_value = atoi(prase_string);
					gsdconf->directory[prase_index].directory_capacity_percent= prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("directory_capacity_absolution", tok, strlen("directory_capacity_absolution"))) {
					prase_string = tok + strlen("directory_capacity_absolution") + 1;
					prase_value = atoi(prase_string);
					gsdconf->directory[prase_index].directory_capacity_absolution= prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("directory_file_cicle_delete", tok, strlen("directory_file_cicle_delete"))) {
					prase_string = tok + strlen("directory_file_cicle_delete") + 1;
					prase_value = atoi(prase_string);
					gsdconf->directory[prase_index].directory_file_cicle_delete= prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("directory_delete_priority", tok, strlen("directory_delete_priority"))) {
					prase_string = tok + strlen("directory_delete_priority") + 1;
					prase_value = atoi(prase_string);
					gsdconf->directory[prase_index].directory_delete_priority= prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else if (!strncmp("directory_left_capacity_warning", tok, strlen("directory_left_capacity_warning"))) {
					prase_string = tok + strlen("directory_left_capacity_warning") + 1;
					prase_value = atoi(prase_string);
					gsdconf->directory[prase_index].directory_left_capacity_warning= prase_value;
					printf("\tsdconfig file: parameter = %s, value = %d\n", tok, prase_value);
				} else {
					/*printf("\tconfig file: parameter = %s\n", tok);*/
				}
			} 	
		}while (tok);
	} 
	fclose(fp);

	return 0;
}

#endif
