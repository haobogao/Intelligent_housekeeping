#ifndef  __KEYBOARD_H__
#define  __KEYBOARD_H__

#include "tsOperation_block.h"
#include "touchsreen.h"

#ifndef PATH_KEYBOARD_BGP
#define PATH_KEYBOARD_BGP   "./keyBoard/keyBoard_BGP/keyBoard_BGP.bmp"
#endif

//键盘键入值的有效值最多位数
#define MAXNUMBER_KEYIN   6

//键盘输入的数字值显示的起始坐标值
#ifndef NUMBER_KEYIN_DISP_START
#define NUMBER_KEYIN_DISP_START_X    280
#define NUMBER_KEYIN_DISP_START_Y    35
#endif

//打开键盘 
//创建检测点击键盘事件线程函数 显示键盘背景图片
/*
	参数： 无
	返回值：
		成功： 0
		失败： -1
*/
int open_keyBoard(void);

// 获取键盘输入的值，有阻塞， 知道键盘输入确认键
int get_value_keyBoard_input();

#endif

