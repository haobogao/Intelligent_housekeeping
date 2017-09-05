#include "system_config.h"
#include <string.h>
#include "musicPlay.h"
#include "display_picture.h"
#include "my_showBMP.h"
#include "voice_recorgnize.h"
#include "socket_client.h"
#include "UART.h"
#include "TH_value_display.h"

#define 	PATH_WLECOME_BGP		"./BGP/wlecome_UI.bmp" 
#define	PATH_MIAN_BGP			"./BGP/main_UI.bmp"

//系统状态 变量
status_system_t  status_system;

void system_init() {
	// 系统工作状态 初始化
	// status_system = G_WELCOME;
	
	//显示欢迎背景图片
	// show_bmp( PATH_WLECOME_BGP );

	// 各模块初始化
	socket_client_init();
	sensor_zigbee_init();
	voice_recognize_init();
	musicplayer_init();
	pictureplayer_init();
}

// 欢迎界面
void do_welcome() {
	puts("Welcome !");
	
	//显示欢迎背景图片3秒
	show_bmp( PATH_WLECOME_BGP );
	
	system_init();
	
	//延时1秒
	sleep(2);
	
    	status_system = G_MAIN;
}

// 欢迎界面
void do_musicplayer() {
	static int run_times = 0;
	if (run_times == 0)
	{
		puts(" Music player !!!");
		run_times++;
	}
	
	musicplayer();
}

// 欢迎界面
void do_pictureDisplayer() {
	static int run_times = 0;
	if (run_times == 0)	{
		puts("Picture display !!!");
		run_times++;
	}
	
	picture_displayer();
}

//主界面
void do_main(void) {
	//主界面 拥有的功能 
	typedef struct {
		tsBlock_function_t   MUSICPLAYER	  		 ;
		tsBlock_function_t   PICTUREDISPLAYER     		 ;
		tsBlock_function_t   VOICE_RECOG	    		 ;
		tsBlock_function_t   T_SENSOR 				 ;
	}action_main_t;
	
	//主界面 拥有的功能 在图片上的坐标点
	action_main_t  tsBlock_event_main = {
		{445,  326, 220, 338},  	//music     功能 图片坐标范围
		{445,  326,  15, 143},  	//picture displayer     功能 图片坐标范围
		{445,  326, 433, 561},  	 //voice recognize    功能 图片坐标范围
		{445,  326, 662, 779},  	//温湿度    功能 图片坐标范围
	};
	
	//触摸屏坐标值
	tsEvent_value_t 	tsEvent_value;
	
	// 显示 主界面 背景图片
	show_bmp( PATH_MIAN_BGP );

	puts("Mianmanue fuction !");
	
	
	while(1)
	{
		//获取触屏事件的坐标值
		getCoordinate_ts( &tsEvent_value);
		
		if ( tsEvent_value.event_type == MODE_EVENT_TOUCH) {

			// 音乐播放 事件
			if(  touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_main.MUSICPLAYER) )	{
				
				status_system  =  G_MUSIC;
				break;
			} else if(  touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_main.PICTUREDISPLAYER) )	{

				status_system  =  G_PICTURE_DISP;
				break;
			} else if(  touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_main.VOICE_RECOG) )	{

				status_system  =  G_VOICE_RECOG;
				break;
			} else if(  touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_main.T_SENSOR) )	{

				status_system  =  G_T_SENSOR;
				break;
			} 
			
		}
	}
	
}

void do_t_sensor() {
	static int run_times = 0;
	if (run_times == 0)	{
		puts("Temperature and humidity sensor !!!");
		run_times++;
	}

	do_TH_sensor_disp();

	status_system = G_MAIN;

}

void do_voice_recognize() {

	static int run_times = 0;
	if (run_times == 0)	{
		puts("Voice recognize !!!");
		run_times++;
	}

	voice_recognize();
}

//系统其他不可预知状态
void do_default(void)
{
	static int run_times = 0;
	if (run_times == 0)
	{
		puts("System fault !!!");
		run_times++;
	}
	
}

int main(int argc, char const *argv[]) {
	system_init();
	// status_system = G_WELCOME;
	status_system = G_MAIN;

	while(1)
	{
		switch( status_system )
		{
			case G_WELCOME:
				//欢迎界面
				do_welcome();
				break;

			case G_MAIN:
				//主界面 -- 功能选择
				do_main();
				break;				
				
			case G_MUSIC:
				//音乐播放
				do_musicplayer();
				break;
				
			case G_PICTURE_DISP:
				//视频播放界面
				do_pictureDisplayer();
				break;
	
			case G_T_SENSOR:
				//温湿度传感器 使用
				do_t_sensor();
				break;	

			case G_VOICE_RECOG:
				//语音识别 使用
				do_voice_recognize();
				break;

			default:
			    //系统出错
			    do_default();
			    break;
		}		
	}	

	return 0;
}
