#ifndef __SYSTEM_CONFIG_H__
#define __SYSTEM_CONFIG_H__

//系统各模块均可能需要的一些共有 配置参数

//系统运行过程信息记录文档目录
// #define RECORD_SYSTEM "./Document/systemRecord.txt"


//系统的几种工作状态 类型
#ifndef   TYPE_STATUS_SYSTEM 
#define   TYPE_STATUS_SYSTEM 
typedef enum
{
	nop       		= 0 ,
	G_WELCOME	= 1 ,
	G_MAIN   		= 4 ,
	G_MUSIC  		= 5 ,
	G_VOICE_RECOG	= 6 ,
	G_T_SENSOR	= 8 ,
	G_PICTURE_DISP	= 9 
} status_system_t;
#endif

//系统状态 变量
extern status_system_t  status_system;


/*#ifndef  TYPEDEF_PATH_SYSBMP	
#define  TYPEDEF_PATH_SYSBMP	
//系统需要用到的图片路径结构体
typedef struct path_SysBmp{
	char    path_welcomeBmp[50]  ;		//欢迎界面
	char    path_waittingBmp[50] ;		//等待 wifi 连接界面
	char    path_loginBmp[50]    ;		//密码登录界面
	char    path_mainMenuBmp[50] ;		//主界面
	char    path_musicBmp[50]    ;		//音乐播放界面
	char    path_videoBmp[50]    ;		//视频播放界面
	char    path_cameraBmp[50]   ;		//摄像头功能界面
	char    path_zigbeeBmp[50]   ;		//zigbee 功能界面
	char    path_adcBmp[50]      ;		//ADC 模块功能界面
	char    path_telnetBmp[50]   ;		//远程登录 功能
	char    path_keyBoardBmp[50] ;		//数字键盘 
	
}path_bmp_t;
#endif

// 系统功能 用到的背景图片路径
extern path_bmp_t  path_Bmp;*/
/*
#ifndef  TYPEDEF_STATE_ACTION
#define  TYPEDEF_STATE_ACTION
//wifi 动作状态枚举类型
typedef enum state_action{
	action_nop     =  0,    //无任何操作
	legal_login    =  1,
	illegal_login  =  2,
	legal_exit     =  3,
	illegal_exit   =  4
}wifiAction_TypeDef;
#endif
*/
//wifi 动作状态变量
/*extern volatile wifiAction_TypeDef  wifi_action;*/

//记录系统运行状态
/*int note_systemRecord(char *buffer);*/

#endif
