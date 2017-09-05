/* 
	显示含指定数字的 bmp图片在LCD 触摸屏上
	先决条件： my_showBMP.c
*/
#include "show_number_BMP.h"

//数字图片路径 结构体类型
#ifndef  TYPEDEF_PATH_NUMBERBMP	
#define  TYPEDEF_PATH_NUMBERBMP	
typedef struct path_numberBMP{
	char    path_number_1BMP[50]  ;		//数字1 图片地址
	char    path_number_2BMP[50]  ;		//数字2 图片地址
	char    path_number_3BMP[50]  ;		//数字3 图片地址
	char    path_number_4BMP[50]  ;		//数字4 图片地址
	char    path_number_5BMP[50]  ;		//数字5 图片地址
	char    path_number_6BMP[50]  ;		//数字6 图片地址
	char    path_number_7BMP[50]  ;		//数字7 图片地址
	char    path_number_8BMP[50]  ;		//数字8 图片地址
	char    path_number_9BMP[50]  ;		//数字9 图片地址
	char    path_number_0BMP[50]  ;		//数字0 图片地址
	char    path_number_pointBMP[50]  ;	//小数点 图片地址
	char    path_number_blackBMP[50]  ;	//空白（遮挡之前显示） 图片地址
	char    path_number_coverBMP[50]  ;	//空白（遮挡之前显示） 图片地址
	char    path_unit_tempBMP[50]  ;	//温度的单位 图片地址
}path_numberBMP_TypeDef;
#endif

//数字图片的 图片路径
path_numberBMP_TypeDef  path_numberBMP ={
	"./keyBoard/number_BMP/number_1.bmp"  ,			//数字1 图片地址
	"./keyBoard/number_BMP/number_2.bmp"  ,			//数字2 图片地址
	"./keyBoard/number_BMP/number_3.bmp"  ,			//数字3 图片地址
	"./keyBoard/number_BMP/number_4.bmp"  ,			//数字4 图片地址
	"./keyBoard/number_BMP/number_5.bmp"  ,			//数字5 图片地址
	"./keyBoard/number_BMP/number_6.bmp"  ,			//数字6 图片地址
	"./keyBoard/number_BMP/number_7.bmp"  ,			//数字7 图片地址
	"./keyBoard/number_BMP/number_8.bmp"  ,			//数字8 图片地址
	"./keyBoard/number_BMP/number_9.bmp"  ,			//数字9 图片地址
	"./keyBoard/number_BMP/number_0.bmp"  ,			//数字0 图片地址
	"./keyBoard/number_BMP/number_point.bmp",		//小数点 图片地址
	"./keyBoard/number_BMP/black.bmp"		,		//空    图片地址
	"./keyBoard/number_BMP/cover.bmp"		,		//用于遮掩的图片地址
	"./keyBoard/number_BMP/temperatureUnit.bmp"		//温度的单位图片地址
};

//获取图片的宽和高的信息
/*
	参数：
		path_numberBMP 图片地址
	返回值：
		成功： 0
		失败： -1	
*/
int getWidth_numberBmp(char *path_numberBMP)
{
	int bmpfd = 0;
	//定义一个图片头信息结构变量
	bmp bmp_header;  
	
	//打开要显示的图片 
	if((bmpfd = open(path_numberBMP, O_RDONLY)) == -1)
	{
		perror("open number_bmp error!\n");
		return -1;
	}
	
	//读取图片头文件信息
	bmp_header = readbmpfile(bmpfd); //读取一个位图	
	
	//关闭文件
	close(bmpfd);
	
	return bmp_header.info.biWidth;
}

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
int show_numberBmp(int offset_x, int offset_y, int number)
{
	char path_BMP_buffer[50] = {0};
	int  width_numberBMP = 0;
	
	if((number > 9) || (number < 0) )
	{
		puts("The number is illegal for show number function! ");
		return -1;
	}
	
	//获取图片文件路径
	bzero(path_BMP_buffer, sizeof(path_BMP_buffer));
	switch(number)
	{
		case 0:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_0BMP);
			break;
		
		case 1:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_1BMP);
			break;
			
		case 2:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_2BMP);
			break;
			
		case 3:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_3BMP);
			break;
			
		case 4:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_4BMP);
			break;
			
		case 5:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_5BMP);
			break;
			
		case 6:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_6BMP);
			break;
			
		case 7:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_7BMP);
			break;
			
		case 8:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_8BMP);
			break;
			
		case 9:
			strcpy(path_BMP_buffer, path_numberBMP.path_number_9BMP);
			break;		
			
		default:
			puts("Display number is illegal !");
			return -1;
	}
	
	/* puts("Number BMP path:");
	puts(path_BMP_buffer); */
	
	//获取图片的宽的信息、用于后面图片的显示
	width_numberBMP = getWidth_numberBmp(path_BMP_buffer);
	
	if( 0 != show_shapeBmp(offset_x, offset_y, path_BMP_buffer) )
	{
		puts("Display number error !");
		
		return -1;
	}
	
	return width_numberBMP;
}

//指定位置（offset_x, int offset_y）显示 小数点的图片
/*
	参数：
		offset_x： 图片显示起始位置距显示屏原点 X方向的偏移距离
		offset_y： 图片显示起始位置距显示屏原点 Y方向的偏移距离
	返回值：
		成功： 显示数字图片的宽度
		失败： -1	
*/
int show_pointBmp(int offset_x, int offset_y)
{
	int  width_numberBMP = 0;
	
	if( 0 != show_shapeBmp(offset_x, offset_y, path_numberBMP.path_number_pointBMP) )
	{
		puts("Display point error !");
		
		return -1;
	}
	
	//获取图片的宽的信息、用于后面图片的显示
	width_numberBMP = getWidth_numberBmp(path_numberBMP.path_number_pointBMP);
	
	return width_numberBMP;
}

//指定位置（offset_x, int offset_y）显示 温度的单位的图片
/*
	参数：
		offset_x： 图片显示起始位置距显示屏原点 X方向的偏移距离
		offset_y： 图片显示起始位置距显示屏原点 Y方向的偏移距离
	返回值：
		成功： 显示数字图片的宽度
		失败： -1	
*/
int show_tempUnitBmp(int offset_x, int offset_y)
{
	int  width_numberBMP = 0;
	
	if( 0 != show_shapeBmp(offset_x, offset_y, path_numberBMP.path_unit_tempBMP) )
	{
		puts("Display point error !");
		
		return -1;
	}
	
	//获取图片的宽的信息、用于后面图片的显示
	width_numberBMP = getWidth_numberBmp(path_numberBMP.path_unit_tempBMP);
	
	return width_numberBMP;
}

//指定位置（offset_x, int offset_y）显示 空白的图片
/*
	参数：
		offset_x： 图片显示起始位置距显示屏原点 X方向的偏移距离
		offset_y： 图片显示起始位置距显示屏原点 Y方向的偏移距离
	返回值：
		成功： 显示数字图片的宽度
		失败： -1	
*/
int show_blackBmp(int offset_x, int offset_y)
{
	int  width_numberBMP = 0;
	
	if( 0 != show_shapeBmp(offset_x, offset_y, path_numberBMP.path_number_blackBMP) )
	{
		puts("Display black error !");
		
		return -1;
	}
	
	//获取图片的宽的信息、用于后面图片的显示
	width_numberBMP = getWidth_numberBmp(path_numberBMP.path_number_blackBMP);
	
	return width_numberBMP;
}

//指定位置（offset_x, int offset_y）显示 遮掩的图片
/*
	参数：
		offset_x： 图片显示起始位置距显示屏原点 X方向的偏移距离
		offset_y： 图片显示起始位置距显示屏原点 Y方向的偏移距离
	返回值：
		成功： 显示数字图片的宽度
		失败： -1	
*/
int show_coverBmp(int offset_x, int offset_y)
{
	int  width_numberBMP = 0;
	
	if( 0 != show_shapeBmp(offset_x, offset_y, path_numberBMP.path_number_coverBMP) )
	{
		puts("Display cover error !");
		
		return -1;
	}
	
	//获取图片的宽的信息、用于后面图片的显示
	width_numberBMP = getWidth_numberBmp(path_numberBMP.path_number_coverBMP);
	
	return width_numberBMP;
}

/* int main(int argc,char **argv)
{
	if(argc < 2)
	{
		printf("argc = %d\n", argc);
		puts("please input argv");
		return -1;
	}
	
	show_bmp(argv[1]);
	
	return 0;
} */