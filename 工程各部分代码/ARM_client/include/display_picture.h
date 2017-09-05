#ifndef  __DISPLAY_PICTURE__
#define __DISPLAY_PICTURE__


#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "tsOperation_block.h"
#include "touchsreen.h"
#include "my_list.h"
#include "my_showBMP.h"

#ifndef MODE_DISPLAYPICTURE
#define DISPLAY_BIGPICTURE		0
#define DISPLAY_SMALLPICTURE		1
#endif

#define 	PATH_PICTURE_DIRECTOR			"./picturedisplayer/picture"
// 功能选择背景图
#define	BGP_selectFunction_pictureDisplayer 		"./picturedisplayer/BGP_picturedisplayer/functionSelect.bmp"

// 播放器工作模式
#define MODE_AUTO_DISPLAY  		1
#define MODE_MANUAL_DISPLAY		0

#ifndef DISPLAY_START
#define DISPLAY_START_X   0
#define DISPLAY_START_Y   0
#endif

// 目录功能
#ifndef MENU_PIC_NUM_X
#define MENU_PIC_NUM_X      8
#endif
#ifndef MENU_PIC_NUM_Y
#define MENU_PIC_NUM_Y      4
#endif
#ifndef  MENU_PIC_SIZE
#define MENU_PIC_SIZE_X      (WIDTH_LCD  / MENU_PIC_NUM_X)
#define MENU_PIC_SIZE_Y      (HEIGHT_LCD / MENU_PIC_NUM_Y)
#endif

void picture_displayer();

int pictureplayer_init();

// 目录功能
void contents_displayer();
#endif