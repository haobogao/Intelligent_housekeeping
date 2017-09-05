#include "sensor_zigbee.h"

// 控制灯
#ifndef 	LED_ON_CMD
#define LED_ON_CMD 			"11"
#define LED_OFF_CMD 		"10"
#endif

// 报警灯 
#ifndef LED_WARNING_ON
#define LED_WARNING_ON    		"21"
#define LED_WARNING_OFF		"20"   
#endif

#ifndef 	FREQ_LED_WAERNING_FESH_uSEC
#define FREQ_LED_WAERNING_FESH_uSEC  (700 * 1000)
#endif

int sensor_temperature_value = 0, sensor_humidity_value  = 0;
int led_status     			= OFF;
// int led_warning_status		= OFF;
int temperature_warnValue_high = 100, temperature_warnValue_low = 0;

static int flag_led_warning 	= OFF;

void * led_warn_thread(void * arg);

void sensor_zigbee_init() {

	UART_init();

	flag_led_warning 	= OFF;
	led_off_control();
	led_warning_off_control();
	// temperature_warn();
}

int get_temperature_humidity_from_UART(char * UART_data, int * temp, int * humidity) {
	if (UART_data == NULL) {
		puts("UART_data for temperature  is empty ! ");
		return -1;
	}

	char *  start_temperature = NULL;
	char *  start_humidiy = NULL;
	if(NULL ==  (start_temperature = strstr(UART_data, "T") ) ) {	
		// puts("UART_data do not have  temperature data !");

		return -1;
	}
	if (NULL ==  (start_humidiy = strstr(UART_data, "H") ) ) {

		// puts("UART_data do not have  humidiy data !");
		return -1;
	}

	start_temperature++;
	*start_humidiy = '\0';
	start_humidiy++;

	if (strlen(start_temperature) == 0 || strlen(start_humidiy) == 0) {
		// puts("Have no temperature or humidity value !");
		return -1;
	}

	*temp = atoi(start_temperature);
	*humidity = atoi(start_humidiy);

	return 0;
}

int send_temperature_to_WX() {

	protocol_ptr_t  prctND_p = create_protc_nd();

	bzero(prctND_p, sizeof(prctND_p));

	prctND_p->command.c_command.dst    =  WX_APP;
	prctND_p->command.c_command.cmd  = temperature;
	prctND_p->command.c_command.ui 	     = temperature_warnValue_high;
	prctND_p->command.c_command.reserve  = temperature_warnValue_low;

	prctND_p->value 				= sensor_temperature_value;

	send_prctdata_to_server(prctND_p);
	
	free_prctND_ptr(prctND_p);
	
	return 0;
}

int send_humidity_to_WX() {

	protocol_ptr_t  prctND_p = create_protc_nd();

	bzero(prctND_p, sizeof(prctND_p));

	prctND_p->command.c_command.dst    =  WX_APP;
	prctND_p->command.c_command.cmd  = humidity;
	prctND_p->value 				= sensor_humidity_value;

	send_prctdata_to_server(prctND_p);

	free_prctND_ptr(prctND_p);
	
	return 0;
}

int led_on_control() {
	led_status     = ON;
	return send_data_UART(LED_ON_CMD, strlen(LED_ON_CMD));
}

int led_off_control() {
	led_status     = OFF;
	return send_data_UART(LED_OFF_CMD, strlen(LED_OFF_CMD));
}

int led_operate(unsigned int cmd_value) {
	if (cmd_value == ON) {
		puts("Open led !");
		return led_on_control();
	} else if (cmd_value == OFF) {
		puts("Close led !");
		return led_off_control() ;
	} else {
		puts("Led cmd_value is wrong !");
		return -1;
	}
}

int led_warning_on_control() {

	return send_data_UART(LED_WARNING_ON, strlen(LED_WARNING_ON));
}

int led_warning_off_control() {

	return send_data_UART(LED_WARNING_OFF, strlen(LED_WARNING_OFF));
}

int get_led_status() {

	return led_status;
}

int temperature_warn() {

	if (flag_led_warning != ON ) {
		pthread_t led_warn_thread_id;//创建线程接收串口数据
		if(0 != pthread_create(&led_warn_thread_id,NULL, led_warn_thread, NULL) ) {
			perror("Can not  create thread to recieve serial data !\n");
			return -1;
		}
	}
	return 0;
}

void * led_warn_thread(void * arg) {

        flag_led_warning  = ON;

	// 分离线程，确保线程结束时回收线程资源
       pthread_detach(pthread_self() );

       puts("Temperature warning !");

       while (sensor_temperature_value > temperature_warnValue_high || sensor_temperature_value < temperature_warnValue_low) {
       	led_warning_on_control();
       	usleep(FREQ_LED_WAERNING_FESH_uSEC);
       	led_warning_off_control();
       	usleep(FREQ_LED_WAERNING_FESH_uSEC);
       }

       led_warning_off_control();
       flag_led_warning  = OFF;

       pthread_exit(NULL);
       return;
}