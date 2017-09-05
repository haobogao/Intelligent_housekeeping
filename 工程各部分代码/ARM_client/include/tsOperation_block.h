#ifndef __TSOPERATION_BLOCK_H__
#define __TSOPERATION_BLOCK_H__

#ifndef	 COORDINAT_T
#define COORDINAT_T	
typedef struct  coordinate
{
	int x, y;
}coordinate_t;
#endif

//触屏功能选择区块属性 变量类型
#ifndef TYPEDEF_FUNCTION_POINT
#define TYPEDEF_FUNCTION_POINT	
typedef struct function_point{
	int  top    ;
	int  bottom ;
	int  left   ;
	int  right  ;
}tsBlock_function_t;
#endif

int  touchAction_check(coordinate_t coordinate_touchevent, tsBlock_function_t  tsBlock_function);

#endif