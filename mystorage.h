/**
 * storage_card_tools.h
 **/

#ifndef __STORAGE_CARD_TOOLS_H__
#define __STORAGE_CARD_TOOLS_H__

//#include <configs.h>

/****************************************************************
 *Function:    storage_card_init
 *Description: detect the exsitence of SDcard,format logo,
 *             and neccessary directory.
 *             If SDcard need to be formated,will format SDcard.
 *Return:      0 means SDcard available, other value means SDcard
 *             unavailabl for einexsitence or format fail
 ****************************************************************/
int storage_card_init();


/****************************************************************
 *Function:    storage_card_format
 *Description: used for format SDcard and make directory and
 *             format logo needed
 *Return:      0 means success, other means fail
 ***************************************************************/
int storage_card_format();


/****************************************************************
 *Function:    storage_card_detect()
 *Description: check SD card exisit or not
 *Return:      0 means detect SDcard, other means fail
 ****************************************************************/
int storage_card_detect();


/****************************************************************
 *Function:    storage_card_format_status()
 *Description: check SD card whether to format or not
 *Return:      0 means do not need format SDcard , -1 means need
 ****************************************************************/
int storage_card_format_status();


/**************************added headfile***************************/
int get_directory_index(int dir_function);
int calc_directory_size(char *path);
int calc_sdcard_totalsize(char *partition_path);
int calc_sdcard_freesize(char *partition_path);
int find_oldest_file_and_delete(char *directory);
int find_oldest_file_and_move(char *directory,int len, int type);
int prepare_enough_sdcard_freesize();
int directory_status_detect();
int sdcard_rw_check();
int sdcard_dir_check();
int sdcard_info_check();
int init_sdinfo();

int dump_info( char *path,int len, int type);
int saveinfo(unsigned char *buf,int len,int type);

#endif /* __STORAGE_CARD_TOOLS_H__ */
