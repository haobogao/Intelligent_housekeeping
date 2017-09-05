//数字键盘 处理程序
//打开键盘 点击相应的数字键 回馈回相应的击键信息
#include "keyBoard.h"
#include <pthread.h>

#ifndef   TYPEDEF_NUMBERVALUE
#define   TYPEDEF_NUMBERVALUE
typedef enum numberValue{
	number_nop  = 100,      	//无数字值
	number_0    = 0  ,     	 //数字值0
	number_1    = 1  ,		//数字值1
	number_2    = 2  ,		//数字值2
	number_3    = 3  ,		//数字值3
	number_4    = 4  ,		//数字值4
	number_5    = 5  ,		//数字值5
	number_6    = 6  ,		//数字值6
	number_7    = 7  ,		//数字值7
	number_8    = 8  ,		//数字值8
	number_9    = 9 , 		//数字值9
	Cancel        = 11 , 		//取消键
	Enter         = 12  		//回车键
}numberValue_t;
#endif

#ifndef    TYPEDEF_NUMBERKEYBOARD
#define    TYPEDEF_NUMBERKEYBOARD
typedef struct numberKeyBoard{
	tsBlock_function_t   number_1   ;		//触摸屏上 数字1 位置范围
	tsBlock_function_t   number_2   ;
	tsBlock_function_t   number_3   ;
	tsBlock_function_t   number_4   ;
	tsBlock_function_t   number_5   ;
	tsBlock_function_t   number_6   ;
	tsBlock_function_t   number_7   ;
	tsBlock_function_t   number_8   ;
	tsBlock_function_t   number_9   ;
	tsBlock_function_t   key_cancel ;
	tsBlock_function_t   number_0   ;
	tsBlock_function_t   key_enter  ;
}tsBlock_numberKeyBoard_t;
#endif

#ifndef KEYBOARD_OPEN
#define KEYBOARD_OPEN   1
#define KEYBOARD_OFF      0
#endif
	
tsBlock_numberKeyBoard_t  tsBlock_event_numberKeyBoard = {
	{( 203), ( 128)   ,118, 283},			//数字 1 图片坐标范围
	{( 203), ( 128)   ,315, 484},			//数字 2 图片坐标范围
	{( 203), ( 128)   ,515, 683},			//数字 3 图片坐标范围
	{( 281), ( 214)   ,118, 283},			//数字 4 图片坐标范围  
	{( 281), ( 214)   ,315, 484},			//数字 5 图片坐标范围
	{( 281),( 214)   , 515, 683},			//数字 6 图片坐标范围
	{( 365), ( 298)   ,118, 283},			//数字 7 图片坐标范围
	{( 365), ( 298)   ,315, 484},			//数字 8 图片坐标范围
	{( 365), ( 298)   ,515, 683},			//数字 9 图片坐标范围
	{( 450),( 381)   , 118, 283},			//按键 删除 图片坐标范围
	{( 450), ( 381)   ,315, 484},			//数字 0 图片坐标范围
	{( 450), ( 381)   ,515, 683}			//按键 确认 图片坐标范围
};

static numberValue_t  numberValue_click =  number_nop;
// 键盘键入值， 确认键按下时有效
static int keyIN_value = 0;
static int keyBoard_is_open = KEYBOARD_OFF;

// 键盘输入值完成
int keyBoard_input_finished() {

	return numberValue_click ==  Enter;
}

// 获取键盘输入的值，有阻塞， 知道键盘输入确认键
int get_value_keyBoard_input() {

	if (keyBoard_is_open != KEYBOARD_OPEN) {
		open_keyBoard();
	}

	while ( !keyBoard_input_finished() )  {
		continue;
	}

	printf("keyBoard input value : %d\n", keyIN_value);
	return keyIN_value;
}

	
//处理点击键盘事件 线程函数
void *click_keyBoard(void *argument) {
	
	// 分离线程，确保线程结束时回收线程资源
      	pthread_detach( pthread_self() );

	//触摸屏 x 、 y 坐标值
	tsEvent_value_t 	tsEvent_value;
	//按键值 记录变量
	numberValue_click =  number_nop;
	int keyIN_buff[MAXNUMBER_KEYIN] = {0};
	int index_keyIN_buff = 0;

	int x_offset = NUMBER_KEYIN_DISP_START_X;
	int y_offset = NUMBER_KEYIN_DISP_START_Y;
	//每次显示的图片数字的宽度
	int width_numberBMP = 0;

	//显示键盘背景图片
	show_bmp( PATH_KEYBOARD_BGP );

	//判断按键，执行相应动作
	index_keyIN_buff = 0;
	while(1) {	

		//获取触屏事件的坐标值
		getCoordinate_ts( &tsEvent_value);

		if ( tsEvent_value.event_type == MODE_EVENT_TOUCH) {

			numberValue_click =  number_nop;

			 if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_1) ) {
				
				puts("Click number 1 !");			
				numberValue_click    =  number_1;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_2) ) {
				
				puts("Click number 2 !");			
				numberValue_click    =  number_2;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_3) ) {
				
				puts("Click number 3 !");			
				numberValue_click    =  number_3;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_4) ) {
				
				puts("Click number 4 !");			
				numberValue_click    =  number_4;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_5) ) {
				
				puts("Click number 5 !");			
				numberValue_click    =  number_5;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_6) ) {
				
				puts("Click number 6 !");			
				numberValue_click    =  number_6;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_7) ) {
				
				puts("Click number 7 !");			
				numberValue_click    =  number_7;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_8) ) {
				
				puts("Click number 8 !");			
				numberValue_click    =  number_8;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_9) ) {
				
				puts("Click number 9 !");			
				numberValue_click    =  number_9;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate,tsBlock_event_numberKeyBoard.number_0) ) {
				
				puts("Click number 0 !");			
				numberValue_click    =  number_0;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_numberKeyBoard.key_cancel) ) {
				
				puts("Click key CANCEL !");			
				numberValue_click    =  Cancel;

				if (width_numberBMP > 0 && x_offset > NUMBER_KEYIN_DISP_START_X) {
					
					//取消之前显示的一位数字
					x_offset = (x_offset - (width_numberBMP));
					y_offset = NUMBER_KEYIN_DISP_START_Y;	
					width_numberBMP = show_blackBmp(x_offset, y_offset);

					keyIN_buff[index_keyIN_buff--] = number_nop;
				}

			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_numberKeyBoard.key_enter) ) {
				
				puts("Click key ENTER !");			
				
				// 将键盘键入的值转换出来
				int i = 0;
				keyIN_value = 0;		//全局变量，注意清零
				for (i = 0; i < index_keyIN_buff; i++) {
					keyIN_value = keyIN_value * 10;
					keyIN_value += keyIN_buff[i];
				}

				numberValue_click    =  Enter;
				break;

			}

			if (numberValue_click    >=  number_0 && numberValue_click <= number_9 ) {

				width_numberBMP = show_numberBmp(x_offset, y_offset, numberValue_click);
				x_offset += width_numberBMP;
				y_offset = NUMBER_KEYIN_DISP_START_Y;

				keyIN_buff[index_keyIN_buff++] = numberValue_click;
			}

		}	
		
	}

	keyBoard_is_open = KEYBOARD_OFF;
	// 关闭键盘线程
	pthread_exit(NULL);
}

//打开键盘 
//创建检测点击键盘事件线程函数 显示键盘背景图片
/*
	参数： 无
	返回值：
		成功： 0
		失败： -1
*/
int open_keyBoard(void) {
	pthread_t threadID_keyBoard = 0;
	
	//创建线程
	if( 0 != pthread_create(&threadID_keyBoard, NULL, click_keyBoard, NULL ) ) {
		perror("Create keyBoard thread fail!");
		puts("Open keyBoard fail!");
		
		return -1;
	}

	keyBoard_is_open = KEYBOARD_OPEN;
	//显示键盘背景图片
	show_bmp( PATH_KEYBOARD_BGP );
	
	return 0;
}


//获取键盘输入的值
/*
	参数： 键入的数值 buffer 字符指针 (最多记录键入 MAXNUMBER_KEYIN 位数)
	返回值：
		成功： 0
		失败： -1
*/
// int getValue_keyBoard(char * buffer_keyIn) {
// 	//键入的数值
// 	// char buffer_keyIn[10] = {0};
// 	int  index_buffer     =  0;	
// 	//数字值 图片显示位置
// 	int x_offset = 0, y_offset = 0;		

// 	//每次显示的图片数字的宽度
// 	int width_numberBMP = 0;
// 	//第一个数字显示的位置
// 	int Tag_topDis = 0;
	
// 	//变量 状态初始化
// 	//bzero(buffer_keyIn, sizeof(buffer_keyIn));
// 	index_buffer  = 0;	
// 	x_offset = NUMBER_KEYIN_DISP_START_X;
// 	y_offset = NUMBER_KEYIN_DISP_START_Y;	
// 	width_numberBMP = 0;
	
// 	//实时对击键事件进行处理
// 	while(1) {		
// 		if( ( (index_buffer < MAXNUMBER_KEYIN) || (numberValue_click == Cancel) || (numberValue_click == Enter)) && (tag_numberProcesser == 1) ) {
// 			//puts(" key event processing!");
// 			switch(numberValue_click) {
				
// 				case number_0:
// 					buffer_keyIn[index_buffer++] = (0 + '0');
// 					break;
				
// 				case number_1:
// 					buffer_keyIn[index_buffer++] = (1 + '0');
// 					break;
				
// 				case number_2:
// 					buffer_keyIn[index_buffer++] = (2 + '0');
// 					break;
				
// 				case number_3:
// 					buffer_keyIn[index_buffer++] = (3 + '0');
// 					break;
				
// 				case number_4:
// 					buffer_keyIn[index_buffer++] = (4 + '0');
// 					break;
				
// 				case number_5:
// 					buffer_keyIn[index_buffer++] = (5 + '0');
// 					break;
				
// 				case number_6:
// 					buffer_keyIn[index_buffer++] = (6 + '0');
// 					break;
				
// 				case number_7:
// 					buffer_keyIn[index_buffer++] = (7 + '0');
// 					break;
				
// 				case number_8:
// 					buffer_keyIn[index_buffer++] = (8 + '0');
// 					break;
				
// 				case number_9:
// 					buffer_keyIn[index_buffer++] = (9 + '0');
// 					break;
				
// 				case Cancel:
// 					(index_buffer == 0)?(buffer_keyIn[ (index_buffer = 0) ] = '\0'):(buffer_keyIn[index_buffer--] = '\0');
// 					break;
					
// 				default:
// 					break;				
// 			}
			
// 			if((numberValue_click >= number_0) && (numberValue_click <= number_9) ){
// 				width_numberBMP = show_numberBmp(x_offset, y_offset, numberValue_click);
				
// 				// if(status_system == G_LOGIN)
// 				// {
// 				// 	//延时一定时间后遮掩该数字图片
// 				// 	usleep(100*1000);
// 				// 	//遮盖之前显示的数字图片
// 				// 	show_coverBmp(x_offset, y_offset);				
// 				// }
				
// 				x_offset = (x_offset + (width_numberBMP));
// 				y_offset = NUMBER_KEYIN_DISP_START_Y;						
				
// 				Tag_topDis = 0;
// 			}
			
// 			else if( ((numberValue_click == Cancel) && (index_buffer >= 0) && (Tag_topDis != 1)) )
// 			{
// 				//取消之前显示的一位数字
// 				x_offset = (x_offset - (width_numberBMP));
// 				y_offset = NUMBER_KEYIN_DISP_START_Y;	
// 				width_numberBMP = show_blackBmp(x_offset, y_offset);
				
// 				//最开始的位置
// 				if(index_buffer == 0)	{
// 					Tag_topDis = 1;
// 				}
// 			}
			
// 			else if( numberValue_click == Enter) {
// 				//确认事件
// 				return 0;
// 			}
			
// 			//击键事件处理完毕 状态标志变量清零
// 			tag_numberProcesser = 0;
// 		}		
		
// 	}
// }
