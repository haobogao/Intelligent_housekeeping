#ifndef __SHOW_NUMBER_BMP_H__
#define __SHOW_NUMBER_BMP_H__

#include <string.h>
#include "my_showBMP.h"

//存储 数字图片路径 的结构体变量
// extern path_numberBMP_TypeDef  path_numberBMP;

//指定位置（offset_x, int offset_y）显示指定数字的图片 -- >  int number
/*
	参数：
		offset_x： 图片显示起始位置距显示屏原点 X方向的偏移距离
		offset_y： 图片显示起始位置距显示屏原点 Y方向的偏移距离
		number:    要显示的数字
	返回值：
		成功： 显示数字图片的宽度
		失败： -1	
*/
int show_numberBmp(int offset_x, int offset_y, int number);

//指定位置（offset_x, int offset_y）显示 小数点的图片
/*
	参数：
		offset_x： 图片显示起始位置距显示屏原点 X方向的偏移距离
		offset_y： 图片显示起始位置距显示屏原点 Y方向的偏移距离
	返回值：
		成功： 显示数字图片的宽度
		失败： -1	
*/
int show_pointBmp(int offset_x, int offset_y);

//指定位置（offset_x, int offset_y）显示 空白的图片
/*
	参数：
		offset_x： 图片显示起始位置距显示屏原点 X方向的偏移距离
		offset_y： 图片显示起始位置距显示屏原点 Y方向的偏移距离
	返回值：
		成功： 显示数字图片的宽度
		失败： -1	
*/
int show_blackBmp(int offset_x, int offset_y);

//指定位置（offset_x, int offset_y）显示 遮掩的图片
/*
	参数：
		offset_x： 图片显示起始位置距显示屏原点 X方向的偏移距离
		offset_y： 图片显示起始位置距显示屏原点 Y方向的偏移距离
	返回值：
		成功： 显示数字图片的宽度
		失败： -1	
*/
int show_coverBmp(int offset_x, int offset_y);

//指定位置（offset_x, int offset_y）显示 温度的单位的图片
/*
	参数：
		offset_x： 图片显示起始位置距显示屏原点 X方向的偏移距离
		offset_y： 图片显示起始位置距显示屏原点 Y方向的偏移距离
	返回值：
		成功： 显示数字图片的宽度
		失败： -1	
*/
int show_tempUnitBmp(int offset_x, int offset_y);

#endif

