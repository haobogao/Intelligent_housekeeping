#ifndef __VOICERECORGNIZE_H__
#define __VOICERECORGNIZE_H__ 

#include "my_list.h"
#include "touchsreen.h"
#include "my_showBMP.h"
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include "socket_client.h"
#include "tsOperation_block.h"

#ifndef TIMELEN_RECORD
#define TIMELEN_RECORD  				3
#endif

#ifndef FREQ_FLASH_FRESH_VRCG_USEC
#define FREQ_FLASH_FRESH_VRCG_USEC  		(200 * 1000)
#endif

#ifndef PATH_OUT_FILE
#define DIR_OUT_FILE  					"./voice_recorgnize/record"
#define NAME_OUT_FILE  					"sample.wav"
#endif

#ifndef PATH_FLASH_RECORD_DIR
#define PATH_FLASH_RECORD_DIR 			"./voice_recorgnize/flash_BGP/record_flash"
#define PATH_FLASH_RECOGNIZE_DIR 			"./voice_recorgnize/flash_BGP/recognise_flash"
#endif

#ifndef PATH_RECORD_BGP
#define PATH_RECORD_BGP 				"./voice_recorgnize/BGP/voicerecorgnize_BGP.bmp"
#endif	

int  voice_recognize_init();
void voice_recognize();

#endif