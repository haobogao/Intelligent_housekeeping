#include "voice_recorgnize.h"
#include "system_config.h"
#include "socket_client.h"
#include "protcol_data.h"
#include "sensor_zigbee.h"

#ifndef STATUS_RECORD
#define STATUS_NOP					0
#define STATUS_RECORD_START 			1
#define STATUS_RECORD_FINSHED			2
#define STATUS_RECOGNIZE_START			3
#define STATUS_RECOGNIZE_FINSHED			4
#define STATUS_EXIT					5
#endif

static int status_recrd =  STATUS_NOP;
static my_list	 list_record_flash, list_recog_falsh;
static node_pointer_type   ndptr_prevFlash;

typedef struct voice_reg{
	tsBlock_function_t	START		;		//开始录音
	tsBlock_function_t	BACK		;		//返回
}tsBlock_voiceRecognizer_t;

//录音 事件各动作区块坐标
tsBlock_voiceRecognizer_t   tsBlock_event_voiceRecognizer = {
	{(435),    97, 291,  501},		//开始播放 事件
	{( 77),   (  0),    0,   79},		//返回     事件
};

void do_recognize_result_operate(char * cmd_operate) {

	if ( !strcmp(cmd_operate, "开灯") ) {
		led_on_control();
	} else if ( !strcmp(cmd_operate, "关灯") ) {
		led_off_control();
	}else {
		puts("命令无法执行！");
	}
}

// 将录音文件发送给服务器
int send_recordFile_to_server(char *file_path_dir, char * file_name) {

	protocol_ptr_t  prctND = create_protc_nd();

	bzero(prctND, sizeof(prctND));

	prctND->command.c_command.dst = PC_SERVER;
	prctND->command.c_command.cmd = speech_recognize;

	send_prctdata_to_server(prctND);

	return send_file_to_serv(file_path_dir, file_name);
}

// 接收语音识别结果接收程序在 socket_client

// 录音函数
int record(int time_len, char * dir_output_file, char * name_output_file) {

	char buff[255] = {0};
	sprintf(buff, "arecord -d%d -c1 -r16000 -twav -fS16_LE %s/%s", time_len, dir_output_file, name_output_file);
	
	//1. 录音 	3秒
	status_recrd =  STATUS_RECORD_START; 
	system(buff);
	status_recrd =  STATUS_RECORD_FINSHED; 

	puts("Record finished !");

	status_recrd =  STATUS_RECOGNIZE_START; 
	//3. 发送录音的文件
	if(0 > send_recordFile_to_server(dir_output_file, name_output_file) ) {
		puts("Send record file to service failed !");
		status_recrd = STATUS_EXIT;
		return -1;
	}

	puts("Sended record file to server finished !");

	return 0;
}

int  recv_result_recog() {

	char msg_buff[BUFF_SIZE_SEND] = {0};
	// 此处改为收数据帧
  	int len_recv = recv_speechRecgResult_from_serv(msg_buff, BUFF_SIZE_SEND);
  	if (len_recv < 0)  {
  		puts("Receive result error !");
  		status_recrd = STATUS_EXIT;
  		return -1;
  	}
  	
  	status_recrd =  STATUS_RECOGNIZE_FINSHED; 
  	
	//TODO:对结果进行处理
	printf("\nRecongnize result : \n");
	printf("==============================\n");
	printf("%s\n", msg_buff);
	printf("==============================\n\n");

	do_recognize_result_operate(msg_buff);

	return 0;
}

int  voice_recognize_init() {

	DIR * flash_file_ptr             	=  NULL;
	struct dirent * stream_dir 	=  NULL;
	char fileName_buffer[255]   	=  {0};	

	// 创建背景图片的存储链表
	mylist_pointer_type	 list_record_flash_ptr = &list_record_flash;
	mylist_pointer_type	 list_recog_falsh_ptr  = &list_recog_falsh;
	if(NULL ==  creat_list(list_record_flash_ptr) ) {
		perror("Creat record_flash list error!");
		return -1;
	};

	if(NULL ==  creat_list(list_recog_falsh_ptr) ) {
		perror("Creat record_flash list error!");
		return -1;
	};

	
	// 打开文件目录
	flash_file_ptr = opendir( PATH_FLASH_RECORD_DIR );
	if(!flash_file_ptr)
	{
		perror("Open flash dirctory  fail!");
		return -1;
	}
	
	while (  (stream_dir = readdir(flash_file_ptr) ) != NULL ) {

	        //存储下该目录下 .bmp 格式的文件 文件名
	        if( strstr(stream_dir->d_name, ".bmp") )
	        {
		        	bzero(fileName_buffer, sizeof(fileName_buffer) );
		        	sprintf(fileName_buffer, "%s/%s",  PATH_FLASH_RECORD_DIR, stream_dir->d_name);
		         	push_front(list_record_flash_ptr,  fileName_buffer);
	         }  
	}

	/* 关闭文件目录*/	
	closedir(flash_file_ptr);

	// 打开文件目录
	flash_file_ptr = opendir( PATH_FLASH_RECOGNIZE_DIR );
	if(!flash_file_ptr)
	{
		perror("Open flash dirctory  fail!");
		return -1;
	}
	
	while (  (stream_dir = readdir(flash_file_ptr) ) != NULL ) {

	        //存储下该目录下 .bmp 格式的文件 文件名
	        if( strstr(stream_dir->d_name, ".bmp") )
	        {
		        	bzero(fileName_buffer, sizeof(fileName_buffer) );
		        	sprintf(fileName_buffer, "%s/%s",  PATH_FLASH_RECOGNIZE_DIR, stream_dir->d_name);
		         	push_front(list_recog_falsh_ptr,  fileName_buffer);
	         }  
	}

	/* 关闭文件目录*/	
	closedir(flash_file_ptr);

}

void * flash_BGP_recognize(void * arg) {

	// 分离线程，确保线程结束时回收线程资源
      	pthread_detach(pthread_self() );

 	 if (!size(list_record_flash)  || !size(list_recog_falsh) ) {
 	 	puts("Flash picture file list is empty!");
 	 	voice_recognize_init();
 	 }

 	 ndptr_prevFlash 			 	= list_record_flash.header;

 	 while (1) {

 	 	if (status_recrd == STATUS_EXIT)  {
 	 		break;
 	 	} else if(status_recrd == STATUS_RECOGNIZE_FINSHED) {

 	 		// 显示 背景图片
			show_bmp( PATH_RECORD_BGP );
			continue;

 	 	} else if (status_recrd == STATUS_RECORD_START)  {

 	 		// 开始刷录音动画
 	 		show_bmp(ndptr_prevFlash->data);

 	 		if (ndptr_prevFlash->next == list_record_flash.trailer) {
 	 			ndptr_prevFlash = list_record_flash.header;
 	 		} else {
 	 			ndptr_prevFlash = ndptr_prevFlash->next;
 	 		}

 	 		usleep( FREQ_FLASH_FRESH_VRCG_USEC );	 

 	 	} else if (status_recrd == STATUS_RECOGNIZE_START) {

 	 		// 开始刷识别语音识别动画
 	 		show_bmp(ndptr_prevFlash->data);
 	 		
 	 		if (ndptr_prevFlash->next == list_recog_falsh.trailer) {
 	 			ndptr_prevFlash = list_recog_falsh.header;
 	 		} else {
 	 			ndptr_prevFlash = ndptr_prevFlash->next;
 	 		}

 	 		usleep( FREQ_FLASH_FRESH_VRCG_USEC );

 	 	}
 	}

 	//回到主功能 界面
	status_system = G_MAIN ;
 	pthread_exit(NULL);
 	return;
}

void voice_recognize() {
	//触摸屏 x 、 y 坐标值
	tsEvent_value_t 	tsEvent_value;
	
	// 显示 背景图片
	show_bmp( PATH_RECORD_BGP );

	status_recrd =  STATUS_NOP;

	 // 创建线程执行自动刷图
	pthread_t threadID_flashBGP = 0;
	if( 0 != pthread_create(&threadID_flashBGP, NULL, flash_BGP_recognize, NULL) )
	{
		perror("Create recorgnize flash_BGP display thread fail !");
	}
	
	while(1)
	{	
		//获取触屏事件的坐标值
		getCoordinate_ts( &tsEvent_value);

		if ( tsEvent_value.event_type == MODE_EVENT_TOUCH) {

			if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_voiceRecognizer.START) ) {

				status_recrd =  STATUS_RECORD_START;
				ndptr_prevFlash = list_record_flash.header;

				if ( !record(TIMELEN_RECORD , DIR_OUT_FILE, NAME_OUT_FILE) ) {
					status_recrd = STATUS_RECOGNIZE_START;
					ndptr_prevFlash = list_recog_falsh.header;

					// recv_result_recog();
					while (status_recrd !=  STATUS_RECOGNIZE_FINSHED) {
						continue;
					}
				}

			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_voiceRecognizer.BACK) )  {

				status_recrd =  STATUS_EXIT;
				
				//回到主功能 界面
				status_system = G_MAIN ;
				
				break;

			}

		} 
	}
}
