#ifndef MYGPS_H 
#define MYGPS_H


//void gps_init();

//unsigned char get_complet_prame(int fd);
void ubxconfig(int fd);

void uartgetstr(int fd);

unsigned char gpsinfoget(int fd);



#endif 
