#include "tsOperation_block.h"

/*
	触屏事件坐标->功能区坐标判断
	参数:
		coordinate_touchevent 触屏事件的坐标值
		tsBlock_function  	  功能 触屏区域	
*/
int  touchAction_check(coordinate_t coordinate_touchevent, tsBlock_function_t  tsBlock_function)	{

	return	(  ( (coordinate_touchevent.y > tsBlock_function.bottom) && (coordinate_touchevent.y < tsBlock_function.top)   )   &&
		 	   ( (coordinate_touchevent.x > tsBlock_function.left  ) && (coordinate_touchevent.x < tsBlock_function.right) )	
		 	);
}