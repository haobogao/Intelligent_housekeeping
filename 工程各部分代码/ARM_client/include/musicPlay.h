#ifndef __MUSICPLAY_H__
#define __MUSICPLAY_H__

#include "my_list.h"
#include "touchsreen.h"
#include "my_showBMP.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "tsOperation_block.h"


// 音乐保存的路径
#define PATH_MUSICDIR 			 "./musicplayer/music"
#define PATH_FLASHDIR			 "./musicplayer/flash_musicplayer"

//music暂停播放背景图片 文件路径
#define  PATH_PAUSEPLAY_BMP 	 "./musicplayer/BGP_musicplayer/musicplayerBGP_pause.bmp"
//music继续播放背景图片 文件路径
#define  PATH_CONTINUEPLAY_BMP  	 "./musicplayer/BGP_musicplayer/musicplayerBGP_continue.bmp"

//音乐播放
void musicplayer();

// 播放器初始化
int  musicplayer_init();


#endif
