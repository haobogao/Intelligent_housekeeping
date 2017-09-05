#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <strings.h>
#include "UART.h"
#include "debug.h"

#ifndef  UART_BUFF_SIZE
#define UART_BUFF_SIZE 	255
#endif

static int serial_fd  =  -1;
static struct termios termios_old;//为了在后面进行关闭串口的时候，恢复原来的设置

int send_data_UART(const void * msg,unsigned int msg_len);
void  *recv_msg_from_UART(void * arg);
int UART_data_process(char * recv_data);

int UART_init()	{	

	//打开串口设备
	serial_fd = open(PATH_UART, O_RDWR);
	if(serial_fd == -1)	{
		perror("Open serial error!\n");
		return -1;
	}

	//设置串口
	//获取串口属性
	struct termios termios_current;//为了在原来的基础上改变串口的设置（属性）

	tcgetattr(serial_fd, &termios_old);
	tcgetattr(serial_fd, &termios_current);

	//设置位raw模式
	cfmakeraw(&termios_current);
	//设置波特率	
	cfsetspeed(&termios_current,	BUAD_RATE);

	//设置bit位 8位
	termios_current.c_cflag &= ~CSIZE;//11 0000
	termios_current.c_cflag |= CS8; //11 0000

	//设置停止位
	termios_current.c_cflag &= ~CSTOPB;//

	//关闭奇偶校验
	termios_current.c_cflag &= ~PARENB;

	termios_current.c_cflag &= ~CRTSCTS;

	tcflush(serial_fd,TCIOFLUSH);

	termios_current.c_cc[VTIME] = 0;
	termios_current.c_cc[VMIN] = 1;

	tcsetattr(serial_fd,TCSANOW,&termios_current);

	pthread_t recv_msg_from_serial_id;//创建线程接收串口数据
	if(0 != pthread_create(&recv_msg_from_serial_id,NULL, recv_msg_from_UART, NULL) ) {
		perror("Can not  create thread to recieve UART data !\n");
		return -1;
	}

	return serial_fd;
}

int close_UART() {
	if (serial_fd == -1) return -1;

	// 关闭串口
	tcsetattr(serial_fd, TCSANOW,&termios_old);
	close(serial_fd);
	serial_fd = -1;

	return 0;
}

int send_data_UART(const void * msg,unsigned int msg_len) {
	
	if(serial_fd == -1) {
		UART_init();
	}

#ifdef  _DEBUG_M_
	printf("Send data to UART : %s\n", (char *)msg);
#endif	
	
    	return write(serial_fd, msg, msg_len);
}

void  *recv_msg_from_UART(void * arg)  {

	if(serial_fd == -1){
		UART_init();
	}

	// 分离线程，确保线程结束时回收线程资源
       pthread_detach(pthread_self() );

	while (1) {	

		// 接收来自串口的数据
		int  msg_len = 0;
		char recv_buff[UART_BUFF_SIZE] = {0};

		bzero(recv_buff, sizeof(recv_buff));
		msg_len = read(serial_fd, recv_buff, sizeof(recv_buff));
		if (msg_len < 0) {
			perror("Read UART data error ! \n");
			continue;
		} else if (msg_len == 0) {
			continue;
		} else {
			UART_data_process(recv_buff);
		}
	}
}

int UART_data_process(char * recv_data) {

#ifdef  _DEBUG_M_
	printf("UART recieved data : %s\n", recv_data);
#endif
	int temp, humid;
	if (0 > get_temperature_humidity_from_UART(recv_data, &temp, &humid) ) {
		// puts("Get temperature value error !");
		return -1;
	}

	sensor_temperature_value = temp;
	sensor_humidity_value = humid;
#ifdef  _DEBUG_M_
	printf("Get temperature: %d\t humidity: %d\n", sensor_temperature_value, sensor_humidity_value);
#endif
	if (sensor_temperature_value > temperature_warnValue_high || sensor_temperature_value < temperature_warnValue_low) {
		temperature_warn();
	}

	send_temperature_to_WX();
	send_humidity_to_WX();

	return 0;
}


