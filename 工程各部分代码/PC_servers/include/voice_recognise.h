#ifndef __VOICE_RECOGNISE_H__
#define __VOICE_RECOGNISE_H__ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "socket_server.h"
#include "voice_recognise.h"

#include "../include/qisr.h"
#include "../include/msp_cmn.h"
#include "../include/msp_errors.h"

#define SAMPLE_RATE_16K    	(16000)
#define SAMPLE_RATE_8K     	 (8000)
#define MAX_GRAMMARID_LEN  	 (32)
#define MAX_PARAMS_LEN    	  (1024)

const char * ASR_RES_PATH        = "fo|res/asr/common.jet"; 	//离线语法识别资源路径
const char * GRM_BUILD_PATH      = "res/asr/GrmBuilld"; 		//构建离线语法识别网络生成数据保存路径
const char * GRM_FILE            = "call.bnf"; 				//构建离线识别语法网络所用的语法文件
const char * LEX_NAME            = "contact"; 				//更新离线识别语法的contact槽（语法文件为此示例中使用的call.bnf）

int voice_recognise_init();
int start_speech_recognize();

#endif