#ifndef __PROTCOL_DATA_H__
#define __PROTCOL_DATA_H__ 

#include "socket_client.h"
#include "voice_recorgnize.h"
#include "system_UI_control.h"
#include "UART.h"

#ifndef PROTCOL_T
#define PROTCOL_T 

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
	led                 	 = 0,  		 //LED
	// window	 	 = 1,		//窗户
	reslt_voice_recog       = 2,		//语音识别
	ui_contr		 = 3		//界面控制
};

enum _cmd_UI_board
{
	main_UI_cmd			= 1,
	musicplayer_UI_cmd		= 2,
	picturedisplayer_UI_cmd	= 3,
	TH_value_UI_cmd		= 4
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

// 发送协议帧给服务器
int  send_prctdata_to_server(protocol_ptr_t  prctND);
// 按协议格式发送收据给服务器
int send_data_to_server(char dst, char cmd, char ui, char reserve, unsigned int value);
// 协议数据处理 对发给板子的数据进行处理
int  prctData_process_client(protocol_ptr_t  prctND);
// 创建协议数据节点
protocol_ptr_t  create_protc_nd();

// 释放协议数据节点内存
void free_prctND_ptr(protocol_ptr_t  prctND);

int net2host_prctND(protocol_ptr_t  prctND_ptr);
int host2net_prctND(protocol_ptr_t  prctND_ptr);

#endif		