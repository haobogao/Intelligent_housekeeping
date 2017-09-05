#ifndef __PROTCOL_DATA_H__
#define __PROTCOL_DATA_H__ 

#include "socket_server.h"
#include "websocket.h"

#ifndef PROTCOL_T
#define PROTCOL_T 

// // 目的字段
// typedef enum Dst
// {
// 	WX_APP = 0, //微信小程序
// 	BOARD   = 1   //开发板
// }dst_t;

// //发给微信小程序的命令字段
// typedef enum _Cmd_WX 
// {
// 	temperature 	 = 0,//温度
// 	humidity	 = 1,//湿度
// 	light		 = 2,//光照
// 	MQ_2 		 = 3,//MQ_2
// 	status_board   = 4

// }cmd_wx_t;

// // 板子功能状态
// typedef enum _Vaule_Status
// {
// 	LCD_OK	 = 	0x01,//lcd，状态ok
// 	TEM_OK	 =	0x02,//温度状态ok
// 	H_OK		 = 	0x04,//湿度状态ok
// 	L_OK 		 = 	0x08//光照状态ok
// }board_status_t;

// //发给单板的命令字段
// typedef enum _Cmd_BOARD 
// {
// 	led                  = 0,  		 //LED
// 	window	 = 1,		//窗户
// 	ui_contr	 = 2		//界面控制
// }cmd_board_t;

// typedef struct  _protocol
// {
// 	union {
// 		unsigned int _command;
// 		struct {
// 			char reserve;// 本机为小端，所以这个存在低地址
// 			char ui;
// 			char cmd;
// 			char dst;
// 		} command;
// 	}command;
// 	unsigned int value;
// };
// 目的字段
enum Dst
{
	WX_APP 	 = 0, //微信小程序
	BOARD  	 =  1,   //开发板
	PC_SERVER  =  2   //PC 服务器
};

//发给微信小程序的命令字段
enum _Cmd_WX 
{
	temperature 	 = 0,//温度
	humidity	 = 1,//湿度
	light		 = 2,//光照
	MQ_2 		 = 3,//MQ_2
	status_board   = 4
};

// 板子功能状态
enum _Vaule_Status
{
	LCD_OK	 = 	0x01,//lcd，状态ok
	TEM_OK	 =	0x02,//温度状态ok
	H_OK		 = 	0x04,//湿度状态ok
	L_OK 		 = 	0x08//光照状态ok
};

//发给单板的命令字段
enum _Cmd_BOARD 
{
	led                		= 0,  		 //LED
	window	 	= 1,		//窗户
	reslt_voice_recog     = 2,		//语音识别
	ui_contr	 	= 3		//界面控制
};

//发给单板的命令字段
enum _Cmd_PC_SERVER
{
	speech_recognize          = 0   //语音识别
};


typedef struct  _protocol
{
	union {
		unsigned int u_command;
		struct {
			char reserve;// 本机为小端，所以这个存在低地址
			char ui;
			char cmd;
			char dst;
		} c_command;
	}command;
	unsigned int value;
}protocol_t, * protocol_ptr_t;

#endif

#ifndef ON
#define ON 	 1
#endif

#ifndef  OFF
#define OFF	 0
#endif

// 协议数据处理
int  prctData_process_server(protocol_ptr_t  pctND_p);

int fill_prctND(protocol_ptr_t  proctND, char dst, char cmd, char ui, char reserve, unsigned int value);

// 创建协议数据节点
protocol_ptr_t create_protc_nd();

// 释放数据节点
void free_prctND(protocol_ptr_t  pctND_p);

// 发送协议帧给板子
int  send_prctdata_toBoard(protocol_ptr_t  pctND_p);

// 发送协议帧给微信
int  send_prctdata_toWX(protocol_ptr_t  pctND_p);

#endif		