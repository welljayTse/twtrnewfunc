/*
 * storage_card_format.c:This file used for check if the format of card_stored is
 * proper. If the format is not proper, then formating it and touch flag file ,
 * necessary directory.
 * Copyright (C) 2017 Ingenic Semiconductor Co.,Ltd
 */

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include "mystorage.h"
#include "gx4g.h"


#define FORMAT_FLAG_NAME        "/media/mmcblk0p1/.flag_file"
#define DIR_ONLINE_NAME         "/media/mmcblk0p1/ONLINE"
#define DIR_OFFLINE_NAME        "/media/mmcblk0p1/OFFLINE"


#define FSCHECK_COMMAND			"fsck /dev/mmcblk0p1"
#define MOUNT_COMMAND			"mount -t vfat /dev/mmcblk0p1 /media/mmcblk0p1"
//#define MOUNT_COMMAND			"mount -t vfat -w -o errors=continue /dev/mmcblk0p1 /media/mmcblk0p1"
#define UMOUNT_COMMAND			"umount -fl /media/mmcblk0p1"
#define FORMAT_COMMAND			"mkfs.vfat /dev/mmcblk0p1"
#define FDISK_COMMAND			"fdisk /dev/mmcblk0"
#define TOUCH_COMMAND           "touch /media/mmcblk0p1/.flag_file"


#define FLAG_FILE				"/media/mmcblk0p1/.flag_file"
char* directory[] = {"/media/mmcblk0p1/ONLINE/",
                      "/media/mmcblk0p1/OFFLINE/",
                      "/media/mmcblk0p1/ONLINE/EXTREM/",
                      "/media/mmcblk0p1/ONLINE/CAR/",
                      "/media/mmcblk0p1/OFFLINE/EXTREM/",
                      "/media/mmcblk0p1/OFFLINE/CAR/"

};

int sd_free_space = 0;

static int storage_card_command(char * command)
{
	int ret;
	FILE *fp;

	fp = popen(command, "w");
	if (NULL == fp) {
		printf("command:[%s], popen fail! %s\n", command, strerror(errno));
		return -1;
	}

	ret = pclose(fp);
	if (ret == -1){
		printf("command:[%s], pclose fail!\n", command);
		return -1;
	}

	if (WIFEXITED(ret) == 0) {
		printf("command:[%s], command exec fail!\n", command);
		return -1;
	}

	return 0;
}

static int make_specific_directory()
{
	int i, ret;

	for (i = 0; i < sizeof(directory)/sizeof(directory[0]); i++) {
		if(opendir(directory[i]))
			continue;

		ret = mkdir(directory[i], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (ret) {
			printf("mkdir %s fail \n", directory[i]);
			return -1;
		}
	}

	return 0;
}

int storage_card_format()
{
	int ret;
	FILE *fp;

	/* new partition */
	fp = popen(FDISK_COMMAND, "w");
	if (NULL == fp) {
		printf("%s fail \n", FDISK_COMMAND);
		return -1;
	}

	/* create a new empty DOS partition table command */
	ret = fputs("o\n", fp);
	if (ret < 0) {
		printf("ret = %d,create a new empty DOS partition fail \n", ret);
	}

	/* set new partition command */
	ret = fputs("n\n", fp);
	if (ret < 0) {
		printf("ret = %d,new partition fail fail \n", ret);
	}

	/* set primary partition command */
	ret = fputs("p\n", fp);
	if (ret < 0) {
		printf("ret = %d,primary partition fail \n", ret);
	}

	/* set partition number command */
	ret = fputs("1\n", fp);
	if (ret < 0) {
		printf("ret = %d,set partition number fail \n", ret);
	}

	/* set First cylinder by using default value */
	ret = fputs("\n", fp);
	if (ret < 0) {
		printf("ret = %d,First cylinder fail \n", ret);
	}

	/* set last cylinder by using default value */
	ret = fputs("\n", fp);
	if (ret < 0) {
		printf("ret = %d,last cylinder fail \n", ret);
	}

	/* save setting */
	ret = fputs("w\n", fp);
	if (ret < 0) {
		printf("ret = %d,quit and save fail fail \n", ret);
	}

	ret = pclose(fp);
	if (-1 == ret) {
		printf("line = %d:close pointer fail  \n", __LINE__);
		return -1;
	}

	if (WIFEXITED(ret) == 0) {
		printf("line = %d: %s command fail!\n", __LINE__, FDISK_COMMAND);
		return -1;
	}

	/* format mmcblk0p1  */
	ret = storage_card_command(FORMAT_COMMAND);
	if (ret) {
		printf("command:%s fail \n",FORMAT_COMMAND);
		return -1;
	}

	/* fsck mmcblk0p1 */
	ret = storage_card_command(FSCHECK_COMMAND);
	if (ret) {
		printf("command:%s fail \n",FSCHECK_COMMAND);
		return -1;
	}

	/* mount sd card */
	ret = storage_card_command(MOUNT_COMMAND);
	if (ret) {
		printf("command:%s fail \n",MOUNT_COMMAND);
		return -1;
	}

	/* mkdir neccesary directory */
	ret = make_specific_directory();
	if (ret) {
		printf("make specific directory fail \n");
		return -1;
	}

	/* touch format flag */
	if (fopen(FLAG_FILE, "a+") == NULL) {
		printf("touch %s fail \n", FLAG_FILE);
		return -1;
	}

	return 0;
}

int sdcard_format_check()
{
    int ret;

    /* check the format status of sdcard*/
    ret = access("/media/mmcblk0p1/.flag_file",F_OK);
    if(!ret){
        return 0;
    }

	/* umount /media/mmcblk0p1 */
	ret = storage_card_command(UMOUNT_COMMAND);
	if (ret) {
		printf("command:%s fail \n", UMOUNT_COMMAND);
		return -1;
	}
    
    /* format mmcblk0p1 */
	ret = storage_card_command(FORMAT_COMMAND);
    if (ret) {
        printf("command:%s fail \n",FORMAT_COMMAND);
        return -1;
    }

	/* mount /media/mmcblk0p1 */
	ret = storage_card_command(MOUNT_COMMAND);
	if (ret) {
        printf("command:%s fail \n", MOUNT_COMMAND);
        return -1;
	}

    /* creat .flag_file*/
	ret = storage_card_command(TOUCH_COMMAND);
    if (ret){
        printf("command:%s fail \n",TOUCH_COMMAND);
        return -1;
    }

    printf("debug: sdcard was formated\n");
    return 0;

}

int storage_card_detect()
{
	int ret;

	/* Check for existence of storage card */
	ret = access( "/dev/mmcblk0", F_OK );
	if (ret) {
		return -1;
	}

    printf("debug: sdcard is here \n");
	return 0;
}

int storage_card_format_status()
{
	int ret;

	/* Check for existence of mmcblk0p1 */
	ret = access( "/dev/mmcblk0p1", F_OK );
	if (ret) {
		printf( "%s:there is not storage card partition \n ",__func__ );
		return -1;
	}

	/* fsck mmcblk0p1 */
	 ret = storage_card_command(FSCHECK_COMMAND); 
	 if (ret) { 
		printf("command:%s fail \n", FSCHECK_COMMAND); 
	 	return -1; 
	 }

	/* mount /dev/mmcblk0p1 */
	ret = storage_card_command(MOUNT_COMMAND);
	if (ret) {
		printf("command:%s fail \n", MOUNT_COMMAND);
		return -1;
	}

	/* Check for existence of flag_file */
	ret = access(FORMAT_FLAG_NAME, F_OK);
	if (ret) {
		printf( "There is not flag file, the storage need to be formated \n");
		goto umount_mmcblk0p1;
	}

	/* check neccesary directory, if no exisit, make it. */
	ret = sdcard_info_check();
	if (ret) {
		printf("check specific directory fail \n");
		goto umount_mmcblk0p1;
	}

	return 0;

umount_mmcblk0p1:
	/* umount /mnt */
	ret = storage_card_command(UMOUNT_COMMAND);
	if (ret) {
		printf("command:%s fail \n", UMOUNT_COMMAND);
		return -1;
	}

	return -1;
}

int calc_directory_size(char *path)
{
	DIR *directory = NULL;
	struct dirent *de;
	struct stat buf;
	int exists, ret;
	long long total_size = 0;
	char name[128] = {0};
	int calc_value = 0;			/* based on kBytes */

	directory = opendir(path);
	if (directory == NULL) {
		printf("%s:%d -> opendir %s error!\n,", __func__, __LINE__, path);
		return 0;
	}

	for (de = readdir(directory); de != NULL; de = readdir(directory)) {
		sprintf(name, "%s/%s", path, de->d_name);
		exists = stat(name, &buf);
		if (exists < 0) {
			fprintf(stderr, "Couldn't stat %s\n", de->d_name);
		} else {
			total_size += buf.st_size;
		}
	}

	ret = closedir(directory);
	if (ret) {
		printf("%s:%d -> closedir %s error!\n", __func__, __LINE__, path);
		return 0;
	}

	/* return kByte */
	/* printf("%lld\n", total_size); */
	calc_value = (int)(total_size / 1024);

	return calc_value;
}

int calc_sdcard_totalsize(char *partition_path)
{
	struct statvfs sfs;
	long long totalsize = 0;
	int value = 0;

	if (!partition_path) {
		printf("partition_path is NULL\n");
		return -1;
	}

	int i = statvfs(partition_path, &sfs);
	if (i == -1) {
		printf("statfs error! -> %s\n", strerror(errno));
		return -1;
	}

	totalsize =(long long) sfs.f_bsize * sfs.f_blocks;
	value = (int)(totalsize / 1024);

	return value;
}

/* return  KBytes */
int calc_sdcard_freesize(char *partition_path)
{
	struct statvfs sfs;
	long long freesize = 0;
	int value = 0;

	if (!partition_path) {
		printf("partition_path is NULL\n");
		return -1;
	}

	int i = statvfs(partition_path, &sfs);
	if (i == -1) {
		printf("statfs error! -> %s\n", strerror(errno));
		return -1;
	}

	freesize = (long long)sfs.f_bsize * sfs.f_bfree;

	value = (int)(freesize / 1024);
	printf("%s->%d:calc sd card freesize is %d kb\n", __func__, __LINE__, value);
	return value;
}

/* to enlager freespace */
int find_oldest_file_and_delete (char *directory)
{
	struct dirent **namelist;
	char name_buf[128] = {0}, thumbnails_buf[128] = {0}, tmp[128] = {0};
	int n, i;

	n = scandir(directory, &namelist, 0, alphasort);
	if (n < 0) {
		printf("scandir error! -> %s\n", strerror(errno));
		return -1;
	} else {
		while (n--) {
			/*printf("%s\n", namelist[n]->d_name);*/
			if (n == 2) {
				sprintf(name_buf, "%s%s", directory, namelist[n]->d_name);
				snprintf(tmp, strlen(namelist[n]->d_name)-2, "%s", namelist[n]->d_name);
			}
			free(namelist[n]);
		}
		free(namelist);
	}

	if (!access(name_buf, F_OK)) {
		if (remove(name_buf)) {
			printf("remove error! %s -> %s\n", name_buf, strerror(errno));
			return -1;
		}
	}

	return 0;
}

/*  move offline data into online folder,
    but it would reissue the data to server befor move */
int find_oldest_file_and_move (char *directory,int len,int type)
{
	struct dirent **namelist;
	char name_buf[128] = {0}, thumbnails_buf[128] = {0}, move_buf[128] = {0};
	int n, i;

	n = scandir(directory, &namelist, 0, alphasort);
	if (n < 0) {
		printf("scandir error! -> %s\n", strerror(errno));
		return -1;
	} else {
		while (n--) {
			/*printf("%s\n", namelist[n]->d_name);*/
			if (n == 2) {
				sprintf(name_buf, "%s%s", directory, namelist[n]->d_name);
			}
			free(namelist[n]);
		}
		free(namelist);
	}
printf("#####002\n");
	if (!access(name_buf, F_OK)) {

		/* to do 
		 * transfor accicdent data
		 */
        dump_info(name_buf,len,type);	
printf("#####003\n");
		sprintf(move_buf,"mv %s %s",name_buf,"/media/mmcblk0p1/ONLINE/EXTREM/");
		printf("---------------->%s\n",move_buf);
printf("#####004\n");
		system(move_buf);
printf("#####005\n");

	}

	return 0;
}


#if 0
int get_directory_index(int dir_function)
{
	int i, index;

	for (i = 0; i < sdinfo->folder_num; i++ ) {
		if (((sdinfo->directory)[i]).directory_function == dir_function) {
			index = i;
			break;
		}
	}
	return index;
}


int prepare_enough_sdcard_freesize()
{
	int free_size = 0;
	int i, lowest_priority, index, ret;
	int occupy_size = 0;

	free_size = calc_sdcard_freesize(sdinfo->rootpath);
	if (free_size < 0) {
		printf("%s:%d -> calc_sdcard_freesize error!\n,", __func__, __LINE__);
		return -1;
	}
	/* printf("%s:%d -> sd card free size is %d!\n,", __func__, __LINE__,free_size); */
	/* printf("%s:%d -> sd_reserved_warning_size is %d!\n,", __func__, __LINE__, sdinfo->sd_reserved_warning_size); */

	if (free_size < sdinfo->sd_reserved_warning_size) {
		/* get lowest priority */
		lowest_priority = ((sdinfo->directory)[0]).directory_delete_priority;
		index = 0;
		for (i = 1; i < sdinfo->folder_num; i++ ) {
			if (((sdinfo->directory)[i]).directory_delete_priority < lowest_priority) {
				lowest_priority = ((sdinfo->directory)[i]).directory_delete_priority;
				index = i;
			}
		}

		/* calc the lowest priority directory occupy space size */
		occupy_size = calc_directory_size(((sdinfo->directory)[index]).directory_path);
		if (occupy_size < 0) {
			printf("%s:%d -> calc_directory_size error!\n,", __func__, __LINE__);
			return -1;
		}

		/* compare  */
		if (free_size + occupy_size < sdinfo->sd_reserved_warning_size) {
			/* TODO */
			printf("%s:%d -> fail to prepare enough freesize!\n,", __func__, __LINE__);
			return 1;
		}

		/* delete the lowest priority directory file*/
		do
		{
			ret = find_oldest_file_and_delete(sdinfo, index);
			if (ret) {
				printf("%s:%d -> find_oldest_file_and_delete error!\n,", __func__, __LINE__);
				return -1;
			}
		} while((free_size = calc_sdcard_freesize(sdinfo->rootpath)) < sdinfo->sd_reserved_warning_size);
	}

	return 0;

}

int directory_status_detect(int dir_function)
{
	int i, index, ret;
	int occupy_size = 0;
	int directory_left_size = 0;
	int warning_flag = 0;
	int need_delete_warning_flag = 0;
	g_directory_config_t *directory;

	for (i = 0; i < sdinfo->folder_num; i++ ) {
		if (((sdinfo->directory)[i]).directory_function == dir_function) {
			index = i;
			printf("%s:%d ->i = %d\n,", __func__, __LINE__,i);
			break;
		}
	}

	directory = sdinfo->directory + index;

circle:
	/* calc the directory occupy space size */
	occupy_size = calc_directory_size(directory->directory_path);
	if (occupy_size < 0) {
		printf("%s:%d -> calc_directory_size error!\n,", __func__, __LINE__);
		return -1;
	}
	/* printf("%s:%d -> occupy_size of %s is %d!\n,",__func__, __LINE__,directory->directory_path,occupy_size); */

	/*calc the directory max left space size, percent eg 80, absolute 200M*/
	if (directory->directory_capacity_limit_type == DIRECTORY_CAPACITY_LIMIT_TYPE_PERCENT) {
		directory_left_size = ((long long)(sdinfo-> sd_free_space)) * (directory->directory_capacity_percent ) / 100 - occupy_size;
	} else if(directory->directory_capacity_limit_type == DIRECTORY_CAPACITY_LIMIT_TYPE_ABSOLUTE)  {
		directory_left_size = directory->directory_capacity_absolution - occupy_size;
	}

	if (directory->directory_left_capacity_warning == DIRECTORY_LEFT_CAPACITY_WARNING_ON) {
		switch(dir_function) {
		case DIRECTORY_FUNCITON_LOCK_VIDEO: {
			if (directory_left_size < sdinfo->lockvideo_reserved_warning_size) {
				warning_flag = 1;
			}
			else
				warning_flag = 0;
			break;
		}
		case DIRECTORY_FUNCITON_PHOTO: {
			if (directory_left_size < sdinfo->photo_reserved_warning_size) {
				warning_flag = 1;
			}
			else
				warning_flag = 0;
			break;
		}
		case DIRECTORY_FUNCITON_PARK_VIDEO: {
			if (directory_left_size < sdinfo->park_reserved_warning_size) {
				warning_flag = 1;
			}
			else
				warning_flag = 0;
			break;
		}
		default: break;
		}

		if (warning_flag == 0) {
			return 0;
		}
	}

	if (directory_left_size <= 0) {
		if (directory->directory_file_cicle_delete ==  DIRECTORY_FILE_CICLE_DELETE_ON) {
			ret = find_oldest_file_and_delete(sdinfo, index);
			if (ret) {
				printf("%s:%d -> find_oldest_file_and_delete error!\n,", __func__, __LINE__);
				return -1;
			}
			if (directory->directory_left_capacity_warning == DIRECTORY_LEFT_CAPACITY_WARNING_ON) {
				need_delete_warning_flag = 1;
			}
			goto circle;
		}else {
			/* arrive at max limit */
			return 2;
		}
	} else if(directory->directory_left_capacity_warning == DIRECTORY_LEFT_CAPACITY_WARNING_ON) {
		if ((warning_flag == 1) && (need_delete_warning_flag == 0)) {
			/*arrive at warning limit, not max limit yet*/
			return 1;
		}

		if ((warning_flag == 1) && (need_delete_warning_flag == 1)) {
			/* delete file until below at warning limit*/
			ret = find_oldest_file_and_delete(sdinfo, index);
			if (ret) {
				printf("%s:%d -> find_oldest_file_and_delete error!\n,", __func__, __LINE__);
				return -1;
			}
			goto circle;
		}

	}

	return 0;

}

#endif 

/**********************************SDCARD INFO INIT***********************************/
#if 0
int init_sdinfo()
{
	int temp = 0;
	/*wj: match function num to floder num*/
	sdinfo->format_enable = sdconf->format_enable;
	sdinfo->folder_num = sdconf->folder_num;
	memcpy(sdinfo->rootpath,"/mnt",sizeof("/mnt"));

	switch(conf->movie_conf.cycle_record){
		case CONF_CYCLIC_REC_3MIN:
			temp = 3 * 60;
			break;
		case CONF_CYCLIC_REC_5MIN:
			temp = 5 * 60;
			break;
		case CONF_CYCLIC_REC_10MIN:
			temp = 10 * 60;
			break;
		default :
			temp = 3 * 60;
	}
	sdinfo->norvideo_maxsize = temp * conf->movie_conf.record_bitrate /8;
	sdinfo->lockvideo_maxsize = 10 * conf->movie_conf.record_bitrate /8;
	sdinfo->photo_maxsize = 4000 *3000 * 1.5/ 20 /1024;

	sdinfo->park_maxsize =  sdinfo->norvideo_maxsize ;
	sdinfo->sd_reserved_warning_size = sdinfo->norvideo_maxsize * sdconf->sd_reserved_warning_number;
	sdinfo->lockvideo_reserved_warning_size = sdinfo->lockvideo_maxsize * sdconf->lockvideo_reserved_warning_number;
	sdinfo->photo_reserved_warning_size = sdinfo->photo_maxsize * sdconf->photo_reseved_warning_number ;
	sdinfo->park_reserved_warning_size = sdinfo->park_maxsize * sdconf->park_reseved_warning_number;

	sdinfo->directory = sdconf->directory;

	//dump_sdginfo();
}
#endif


int sdcard_rw_check()
{
	int ret = 0;
	if (fopen(FLAG_FILE, "a+") == NULL) {
            printf("touch %s fail \n", FLAG_FILE);
	    	/*unmount sd card */
	    	ret = storage_card_command(UMOUNT_COMMAND);
	    	if (ret) {
	    		printf("command:%s fail \n",UMOUNT_COMMAND);
	    		return -1;
	    	}
	    	/* format mmcblk0p1  */
	    	ret = storage_card_command(FORMAT_COMMAND);
	    	if (ret) {
	    		printf("command:%s fail \n",FORMAT_COMMAND);
	    		return -1;
	    	}

	    	/* fsck mmcblk0p1 */
	    	ret = storage_card_command(FSCHECK_COMMAND);
	    	if (ret) {
	    		printf("command:%s fail \n",FSCHECK_COMMAND);
	    		return -1;
	    	}

	    	/* mount sd card */
	    	ret = storage_card_command(MOUNT_COMMAND);
	    	if (ret) {
	    		printf("command:%s fail \n",MOUNT_COMMAND);
	    		return -1;
	    	}
	}
	return 0;
}

int sdcard_dir_check()
{

	int i ,ret= 0;
	for (i = 0; i < sizeof(directory)/sizeof(directory[0]); i++){
        if(opendir(directory[i]))
                continue;
        ret = mkdir(directory[i], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (ret) {
                printf("mkdir %s fail \n", directory[i]);
                return -1;
        }
    }
	return 0;

}

int sdcard_info_check()
{
	int ret = 0;

    ret = sdcard_format_check();
    if (ret){
        printf("format check error!\n");
        return -1;
    }
    
	ret = sdcard_rw_check();
	if (ret){
		printf ("rw check error!\n");
		return -1;
	}

	ret = sdcard_dir_check();
	if (ret){
		printf ("dir check error!\n");
		return -1;
	}
	return 0;
}

#if 0
int storage_card_init()
{
	int ret;

	/* Check for existence of storage card */
	ret = storage_card_detect();
	if (ret) {
		printf( "%s:there is not storage card detected \n", __func__);
		return -1;
	}
	ret = storage_card_format_status();
	if (ret) {
		/*format the sd card */
		ret = storage_card_format();
		if (ret) {
			printf("format the sd card fail \n");
			return -1;
		}
	}
	return 0;
}

#endif

/* 2018-05-08
* initial sd card to save info
* accord to ingnic code
*/
int storage_card_init()
{
    int ret = -1;
    int i = 0;
    int sdcard_valid = 0;

	printf("ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
	printf("oooooooooooooooooo  sd card initial start  oooooooooooooooooooooooo\n");
	printf("ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");

//    /*1.check sdcard device existence*/
//    ret = storage_card_detect();
//    if(ret < 0){
//        printf("warning:no sdcard available\n");
//        return -1;
//    }
//    /* check the format status of sdcard*/
//    ret = access("/media/mmcblk0p1/.flag_file",F_OK);
//    if(!ret){
//        printf("mesg:sd card has been using\n");
//        return 0;
//    }
//    /* umount /media/mmcblk0p1 */
//	ret = storage_card_command(UMOUNT_COMMAND);
//	if (ret) {
//	        printf("command:%s fail \n", UMOUNT_COMMAND);
//		    return -1;
//	}
  //      ret = storage_card_format();

    /*2.check sdcard status*/
    ret = sdcard_info_check();
    if(ret < 0){
        printf("error:sdcard check error\n");
        return -1;
    }
    printf(" i = %d\n",sizeof(directory)/sizeof(directory[0]));

    for(i=0; i<sizeof(directory)/sizeof(directory[0]); i++){
        sd_free_space += calc_directory_size(directory[i]); 
    }
    sd_free_space += calc_sdcard_freesize("/media/mmcblk0p1/"); 
    
    printf("sd_free_space = %ld\n",sd_free_space);

	printf("ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
	printf("oooooooooooooooooooo  sd card initial end  oooooooooooooooooooooooo\n");
	printf("ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");

    return 1;

}

/** reissue info send to server
 *  path : the package's absolute path
 *  len  : the package's size
 *  type : 0:extrminfo, 1:carinfo
 **/
int dump_info( char *path,int len, int type)
{
    FILE *fp;
    int i;
    extrm_info dump_extrem_info ={0};
    car_info dump_car_info ={0};

    char buf[1024];
    bzero (buf,1024);

    if(!(fp = fopen(path,"rb"))){
        printf("file %s open fail\n",path);
        return -1;
    }

    if (type == 0){
        if (fread(&dump_extrem_info,len,1,fp) != 1){
            printf("file %s read error\n",path);
            fclose(fp);
            return -1;
        }
        printf ("\t\n dump_extrem_info:batterytv = 0x%02x 0x%02x  \t\n",\
            dump_extrem_info.batterytv[0],dump_extrem_info.batterytv[1]);

        transfercontrl((unsigned char *)(&dump_extrem_info),(unsigned char *)(buf),0xb110,38);
        udpsend(psh->gs_transferbuf,54);
    }
       
    fclose(fp); 
    return 0;
}

/** 
 *  buf  :  source info 
 *  len  :  buf size
 *  type :  0:online extrminfo,1:online carinfo
            2:offline extrminfo,3:offline carinfo
 */
int saveinfo(unsigned char *buf,int len,int type)
{
    char save_path[128];
    char save_directory[128];
    char delete_directory[128];
    FILE *fp;
    int ret = 0,i = 0;
    int freesize =0; /* based on KBytes */
    bzero(save_path,128);
    bzero(save_directory,128);
    bzero(delete_directory,128);
          
    switch (type){
    case 0: 
            sprintf(save_directory,"%s",directory[2]);
            sprintf(delete_directory,"%s",directory[2]);
            break;
    case 1: 
            sprintf(save_directory,"%s",directory[3]);
            sprintf(delete_directory,"%s",directory[3]);
            break;
    case 2: 
            sprintf(save_directory,"%s",directory[4]);
            sprintf(delete_directory,"%s",directory[2]);
            break;
    case 3: 
            sprintf(save_directory,"%s",directory[5]);
            sprintf(delete_directory,"%s",directory[3]);
            break;
    default:
            break;
    }

    freesize = calc_sdcard_freesize("/media/mmcblk0p1/");
    printf("sd card freesize is %d\n",freesize);
    /* test set sd card limit is 3GB */
    if(freesize < 3871500){
    //if(freesize < 1024 * 1024 * 3.6){
          find_oldest_file_and_delete(delete_directory);
    }

 //   sprintf(save_path,"%s%04d-%02d-%02d-%02dh-%02dm-%02ds.log",save_directory,\
 //           psh->localtime[0], psh->localtime[1],psh->localtime[2],\
 //           psh->localtime[3], psh->localtime[4],psh->localtime[5]);

    sprintf(save_path,"%s%04d-%02d-%02d-%02dh-%02dm-%02ds.log",save_directory,\
        psh->gs_gpsinfo_recv.time[0]+2000,psh->gs_gpsinfo_recv.time[1], psh->gs_gpsinfo_recv.time[2],\
        psh->gs_gpsinfo_recv.time[3], psh->gs_gpsinfo_recv.time[4],psh->gs_gpsinfo_recv.time[5]);
   	printf("*********************************************************\n");
	printf("*****************              **************************\n");
	printf("*****************   SAVE INFO  **************************\n");
	printf("*****************              **************************\n");
	printf("*********************************************************\n");

    if(!(fp = fopen(save_path,"wb+"))){
        printf("file %s creat fail\n",save_path);
        return -1;
    }

    if (fwrite(buf,len,1,fp) != 1){
        printf("file %s save error\n",save_path);
        fclose(fp);
        return -1;
    }
    /* debug : printf the store package detail info */
    printf("Store OK : ");
    for(i = 0;i <len;i++){
        printf("0x%02x ",*(buf+i));
    }
    printf("-- total %d bytes\n",len);
    printf("%s -- save ok\n",save_path);
    fclose(fp);

    sleep(1);

    //  dump_info(save_path,len,type);
    //  find_oldest_file_and_delete (save_directory);
       
    return 0;
}

int dir_check_file_counts(int type)
{
    DIR *dirp = NULL;
    int num = 0;
    char save_directory[128];

    bzero(save_directory,128);

    switch (type){
    case 0: 
            sprintf(save_directory,"%s",directory[2]);
            break;
    case 1: 
            sprintf(save_directory,"%s",directory[3]);
            break;
    case 2: 
            sprintf(save_directory,"%s",directory[4]);
            break;
    case 3: 
            sprintf(save_directory,"%s",directory[5]);
            break;
    default:
            break;
    }

    /* here can also use scandir to get the package's counts */
    dirp = opendir(save_directory);
    if(dirp == NULL){
        dirp = opendir(save_directory);
        if(dirp == NULL){
            return -1;
        }
    }
    
    while (dirp){
        if(readdir(dirp)!= NULL){
              ++num;
        }
        else break;
    }
    closedir(dirp);
    return num;
}

/** 
 *  len  :  buf size
 *  type :  0:extrminfo,1:carinfo
 */
int reissue_info_send(int len, int type)
{
    char save_directory[128];
    bzero(save_directory,128);

    switch (type){
    case 0: 
            sprintf(save_directory,"%s",directory[2]);
            break;
    case 1: 
            sprintf(save_directory,"%s",directory[3]);
            break;
    case 2: 
            sprintf(save_directory,"%s",directory[4]);
            break;
    case 3: 
            sprintf(save_directory,"%s",directory[5]);
            break;
    default:
            break;
    }

    if( dir_check_file_counts(type) <= 0 ){
        /* no data need to be reissue */
        printf("type %d is empty\n",type);
        return 0;
    } 
    /* find the olderest file to reissue */
    find_oldest_file_and_move (save_directory,len,type);
}
