#ifndef __UART_H__
#define __UART_H__

#include "sensor_zigbee.h"

// 串口文件路径 
#ifndef 	PATH_UART
#define PATH_UART 	"/dev/s3c2410_serial3"
#endif

// 波特率	115200
#ifndef BUAD_RATE
#define BUAD_RATE 		B115200
#endif

int UART_init();
int close_UART();
int send_data_UART(const void * msg,unsigned int msg_len);

#endif