#ifndef  __SENSOR_ZIGBEE_H__
#define __SENSOR_ZIGBEE_H__    

#include "UART.h"
#include "protcol_data.h"

// 协议中定义的有开关值
// #ifndef ON
// #define ON 	 1
// #endif

// #ifndef  OFF
// #define OFF	 0
// #endif

extern int sensor_temperature_value, sensor_humidity_value;
extern int temperature_warnValue_high, temperature_warnValue_low;
extern int led_status;

void sensor_zigbee_init();
int send_temperature_to_WX();
int send_humidity_to_WX();
int temperature_warn();
int get_led_status();
int led_on_control();
int led_off_control();
int led_operate(unsigned int cmd_value);
int get_temperature_humidity_from_UART(char * UART_data, int * temp, int * humidity);

#endif