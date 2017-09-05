#ifndef __MY_SHOWBMP_H__
#define __MY_SHOWBMP_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <stdio.h>

//液晶屏信息
#ifndef  INFORMATION_TOUCHSCREEN
#define  INFORMATION_TOUCHSCREEN
#define SIZE_LCD   		 800*480
#define WIDTH_LCD   	 800
#define HEIGHT_LCD  	 480
#define DISPLAY_START_X  0
#define DISPLAY_START_Y  0
#endif

/*各设备文件的路径*/
#ifndef LCD_FILEPATH
#define LCD_FILEPATH 	 "/dev/fb0"
#endif

#define FREQ_FRESH_DISP  	1
#define DISPLAY_DIRCTOR_OPT	0
#define DISPLAY_DIRCTOR_NEG	1


/*bmp文件的头信息*/
#ifndef  TYPEDEF_TAsGBITMAPFILEHEADER
#define  TYPEDEF_TAGBITMAPFILEHEADER
typedef  struct  tagBITMAPFILEHEADER
{
	unsigned char       bfType[4];     //位图文件的类型，必须为BM
	unsigned long       bfSize;       //文件大小，以字节为单位
	unsigned short int  bfReserverd1; //位图文件保留字，必须为0
	unsigned short int  bfReserverd2; //位图文件保留字，必须为0
	unsigned long       bfbfOffBits;  //位图文件头到数据的偏移量，以字节为单位
}BITMAPFILEHEADER;

typedef  struct  tagBITMAPINFOHEADER
{
	long biSize;                        //该结构大小，字节为单位
	long  biWidth;                      //图形宽度以象素为单位
	long  biHeight;                     //图形高度以象素为单位
	short int  biPlanes;                //目标设备的级别，必须为1
	short int  biBitcount;              //颜色深度，每个象素所需要的位数
	short int  biCompression;        //位图的压缩类型
	long  biSizeImage;             	    //位图的大小，以字节为单位
	long  biXPelsPermeter;       	    //位图水平分辨率，每米像素数
	long  biYPelsPermeter;       	    //位图垂直分辨率，每米像素数
	long  biClrUsed;                      //位图实际使用的颜色表中的颜色数
	long  biClrImportant;       		//位图显示过程中重要的颜色数
}BITMAPINFOHEADER;

typedef  struct
{
	BITMAPFILEHEADER  file; //文件信息区
	BITMAPINFOHEADER  info; //图象信息区
}bmp;

typedef  struct
{
	long  biWidth;                      //图形宽度以象素为单位
	long  biHeight;                     //图形高度以象素为单位
}bmp_bit_WidHight;
#endif

//读出图片头信息函数  
//参数   int bmpfd ： bmp图片文件描述符
//返回值： 图片头信息类型的结构体
bmp  readbmpfile(int bmpfd);

// 自定义实现显示bmp格式图片 在LCD显示屏上
//参数：  char *bmp_pathname  图片路径字符串
//返回值： 0 -- 操作成功   非0-- 操作失败
int show_bmp(char *bmp_pathname);

//指定位置（offset_x, int offset_y）显示指定图片 -- >  char *bmp_pathname
int show_shapeBmp(int offset_x, int offset_y, char *bmp_pathname);

//获取图片的宽和高的信息
/*
    参数：
        path_numberBMP 图片地址
    返回值：
        成功： 宽度和高度的结构体
        失败： -1  
*/
bmp_bit_WidHight getWidthHight(char *path_BMP);

/*
	缩放并显示图片
 */
int show_bmp_zoom(char * bmp_pathname, int start_x, int start_y, int pic_size_x_zoomed, int pic_size_y_zoomed);

int show_zoomBMP_init();
void close_lcd_show_zoomBMP();
#endif
