#include "TH_value_display.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifndef  VALUE_DISPLAY_START
#define VALUE_DISPLAY_START_X			282
#define VALUE_DISPLAY_H_WARNING_START_Y	53
#define VALUE_DISPLAY_L_WARNING_START_Y	254
#define VALUE_DISPLAY_T_START_Y			155
#define VALUE_DISPLAY_H_START_Y			357
#endif

#ifndef  FREQ_TH_VALUE_FRESH
#define FREQ_TH_VALUE_FRESH_uSEC 		(200 * 1000)
#endif

#ifndef  SENSOR_DISP_STATUS
#define SENSOR_DISP_START   				0
#define SENSOR_DISP_EXIT   		 			1
#define SENSOR_DISP_SETTING 		 		2
#endif

#ifndef  T_WARNING_TAG_DISP_START
#define T_WARNING_TAG_DISP_START_X    (NUMBER_KEYIN_DISP_START_X - 200)
#define T_WARNING_TAG_DISP_START_Y    (NUMBER_KEYIN_DISP_START_Y -5)
#endif

//温湿度显示 事件具有的属性 变量类型
typedef struct {
	tsBlock_function_t	HIGH_T_WARNING_SETTING	;		//高温报警值设置
	tsBlock_function_t	LOW_T_WARNING_SETTING 	;		//低温报警值设置
	tsBlock_function_t	BACK					;		//返回
}tsBlock_TH_sensor_t;	

//温湿度显示 事件各动作区块坐标
tsBlock_TH_sensor_t   tsBlock_event_TH_sensor = {
	{162, 103, 650, 800},		//高温报警值设置
	{265, 197, 650, 800},		//低温报警值设置
	{446, 354, 670, 770},		//返回
};

static int do_sensor_disp_status = SENSOR_DISP_EXIT;

//显示报警温度值
int display_waringValue(int sart_X, int start_Y, int temperatureValue) {

	//报警高度值显示
	int buffer_displayValue[10] = {0};
	int index_buffer = 0;
	int index        = 0;
	
	int width_numberBMP = 0;
	//温度值显示位置
	int x_offset = 0, y_offset = 0;	
	
	if (temperatureValue == 0) {
		index_buffer = 1;
	}
	while(temperatureValue != 0)
	{
		buffer_displayValue[index_buffer++] =  (temperatureValue % 10);
		temperatureValue =  (temperatureValue / 10);	
	}
	
	//将温度值显示到触摸屏上
	width_numberBMP = 0;
	x_offset = sart_X;
	y_offset = start_Y;
	for(index = (index_buffer-1); index >= 0; index-- )
	{
		width_numberBMP = show_numberBmp(x_offset, y_offset, buffer_displayValue[--index_buffer]);
		x_offset = (x_offset + (width_numberBMP));
		y_offset = start_Y;
	}
	
	width_numberBMP = show_tempUnitBmp(x_offset, y_offset);
	
	return 0;	
}

void show_temperature_value(int x_offset, int y_offset, int TH_value_temp) {
	int TH_value_separated[15] = {0};
	int TH_bits = 0; //位数

	// TH_value_temp = 556;
	while (TH_value_temp != 0) {
	 	TH_value_separated[++TH_bits] = (TH_value_temp % 10);
	 	TH_value_temp = TH_value_temp / 10;
	 }

	//  // printf("sensor_temperature_value : %d\n",  sensor_temperature_value);
	// x_offset = VALUE_DISPLAY_START_X;
	// y_offset = VALUE_DISPLAY_T_START_Y;
	while ( TH_bits ) {
		x_offset = x_offset + show_numberBmp(x_offset, y_offset, TH_value_separated[TH_bits--]);
	}

	show_tempUnitBmp(x_offset, y_offset);
}

void show_humidity_value(int x_offset, int y_offset, int TH_value_temp) {
	int TH_value_separated[15] = {0};
	int TH_bits = 0; //位数

	// TH_value_temp = 556;
	while (TH_value_temp != 0) {
	 	TH_value_separated[++TH_bits] = (TH_value_temp % 10);
	 	TH_value_temp = TH_value_temp / 10;
	 }

	 // printf("sensor_temperature_value : %d\n",  sensor_temperature_value);
	while ( TH_bits ) {
		x_offset = x_offset + show_numberBmp(x_offset, y_offset, TH_value_separated[TH_bits--]);
	}
}

//刷新温度值线程函数
void *refresh_TH_value(void *argument) {
	
	//温度值显示位置
	int x_offset = VALUE_DISPLAY_START_X, y_offset = VALUE_DISPLAY_T_START_Y;
	// 分离线程，确保线程结束时回收线程资源
      	pthread_detach( pthread_self() );	
	
	int width_numberBMP = 0;
	
	while(1)  {

		if (do_sensor_disp_status == SENSOR_DISP_SETTING) {
			continue;
		} else if (do_sensor_disp_status == SENSOR_DISP_EXIT) {
			break;
		}

		show_temperature_value(VALUE_DISPLAY_START_X, VALUE_DISPLAY_T_START_Y, sensor_temperature_value);
		if (do_sensor_disp_status != SENSOR_DISP_START) {
			continue;
		}

		show_humidity_value(VALUE_DISPLAY_START_X, VALUE_DISPLAY_H_START_Y, sensor_humidity_value);
		if (do_sensor_disp_status != SENSOR_DISP_START) {
			continue;
		}

		display_waringValue(VALUE_DISPLAY_START_X, VALUE_DISPLAY_H_WARNING_START_Y, temperature_warnValue_high);
		if (do_sensor_disp_status != SENSOR_DISP_START) {
			continue;
		}

		display_waringValue(VALUE_DISPLAY_START_X, VALUE_DISPLAY_L_WARNING_START_Y, temperature_warnValue_low);

		static int index_warnTimes = 0;
		//温度过高： 蜂鸣器 报警
		if( ((sensor_temperature_value - temperature_warnValue_high) < 0 || (sensor_temperature_value -  temperature_warnValue_low) > 0 ) && ((++index_warnTimes) % 600 == 0))  {
			index_warnTimes = 0;
			buzzer_warning();
		}
		if (do_sensor_disp_status != SENSOR_DISP_START) {
			continue;
		}			
		
		//延时 FREQ_TH_VALUE_FRESH_uSEC 秒
		usleep(FREQ_TH_VALUE_FRESH_uSEC);
	}

	pthread_exit(NULL);
}

//修改报警温度上限值
int modify_warningUpper(void) {

	puts("Modify upper warning temperature value !");
	
	//打开键盘  检测按键输入
	if(0 != open_keyBoard() )	{
		puts("Open keyBoard fail in modify upper warning temperature function !");
	}
	
	// 显示温度报警值提示字符
	show_shapeBmp(T_WARNING_TAG_DISP_START_X,  T_WARNING_TAG_DISP_START_Y, PATH_WARNUPPER_BMP);
	
	temperature_warnValue_high = get_value_keyBoard_input();
	
	return 0;
}

//修改报警温度下限值
int modify_warningLower(void) {
	
	puts("Modify lower warning temperature value !");
	
	//打开键盘  检测按键输入
	if(0 != open_keyBoard() )	{
		puts("Open keyBoard fail in modify upper lower temperature function !");
	}
	
	// 显示温度报警值提示字符
	show_shapeBmp(T_WARNING_TAG_DISP_START_X, T_WARNING_TAG_DISP_START_Y, PATH_WARNLOWER_BMP);
	
	temperature_warnValue_low = get_value_keyBoard_input();

	return 0;
}

//温湿度显示 功能函数
void do_TH_sensor_disp(void) {	

	puts("Into temperature and humidity display function!");
	
	//显示 温湿度显示 功能背景图片
	show_bmp( PATH_TH_VALUE_DISP_BGP );

	//触摸屏 x 、 y 坐标值
	tsEvent_value_t 	tsEvent_value;
	
	// 创建线程 实时刷新温湿度值
	do_sensor_disp_status = SENSOR_DISP_START;
	pthread_t threadID_freshTH = 0;
	if( 0 != pthread_create(&threadID_freshTH, NULL, refresh_TH_value, NULL) )	{
		perror("Create fresh TH value display thread fail !");
	}

	while(1) {	

		//获取触屏事件的坐标值
		getCoordinate_ts( &tsEvent_value);

		if ( tsEvent_value.event_type == MODE_EVENT_TOUCH) {

			 if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_TH_sensor.HIGH_T_WARNING_SETTING) ) {
				
				do_sensor_disp_status = SENSOR_DISP_SETTING;
				usleep(50 * 1000);
				modify_warningUpper();

				//显示 温湿度显示 功能背景图片  重新刷图
				show_bmp( PATH_TH_VALUE_DISP_BGP );

				do_sensor_disp_status = SENSOR_DISP_START;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_TH_sensor.LOW_T_WARNING_SETTING) ) {
				
				do_sensor_disp_status = SENSOR_DISP_SETTING;
				usleep(50 * 1000);
				modify_warningLower();

				//显示 温湿度显示 功能背景图片  重新刷图
				show_bmp( PATH_TH_VALUE_DISP_BGP );

				do_sensor_disp_status = SENSOR_DISP_START;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_TH_sensor.BACK) )  {

				//返回  事件
				puts("Out TH sensor display!");		
				do_sensor_disp_status = SENSOR_DISP_EXIT;		
				break;

			}

			// puts("Fresh BGP in TH_sensor_disp");
			
		}

	}	
}

 
