#!/bin/sh
if [ "$1" == "caninit" ];then

    ifconfig can0 down
    echo 250000 > /sys/devices/platform/FlexCAN.0/bitrate
    ifconfig can0 up

elif [ "$1" == "4ginit" ];then

    echo "Now Enable The LongSung Moudle"
    echo 76 > /sys/class/gpio/export 
    echo out > /sys/class/gpio/gpio76/direction 
    echo 1 > /sys/class/gpio/gpio76/value


elif [ "$1" == "4gdial" ];then
    echo "TIMEOUT 5 ''AT OK ATE0V1 OK AT OK ATS0=0 OK AT OK ATE0V1 OK AT OK ATDT*99*#CONNECT ''" > /tmp/chatscript;
    /usr/bin/pppd /dev/ttyUSB1 persist connect '/usr/bin/chat -v -f /tmp/chatscript'

elif [ "$1" == "4groute" ];then
    route add default gw 10.64.64.64
    nameserver 8.8.8.8

elif [ "$1" == "gpsinit" ];then
     echo -en "AT+GPSSTART\r\n" > /dev/ttyUSB2
     echo -en "AT+GPSCONFIG=20,180,9999999,1,2\r\n" > /dev/ttyUSB2

elif [ "$1" == "ntpdate" ];then
     ntpdate 0.pool.ntp.org
fi
 


