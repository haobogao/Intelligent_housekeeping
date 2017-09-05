#include "musicPlay.h"
#include "system_config.h"
// #include "system_config.h"
/*
	BUG 记录:
		宏重复定义
		功能区坐标判断
		Flash 特效初始状态不对
		不小心删 "{"
		链表尾判断
		tsBlock_event_playMusic 定义在 .h 文件
		上下曲切换 背景图
		刚进入音乐播放器时直接上下曲会 kill all 错误
 */
#define	START_PLAYMUSIC 	0
#define 	PAUSE_PLAYMUSIC	1
#define	CONTINUE_PLAYMUSIC	2
#define	NEXT_PLAYMUSIC 		3
#define	PREV_PALYMUSIC 		4
#define	EXIT_PALYMUSIC 		5

#define FLASHPIC_START_X		247
#define FLASHPIC_START_Y		0
#define FREQ_FLASH_FRESH_USEC	(150 * 1000)

typedef struct {
	tsBlock_function_t	START		;		//开始播放
	tsBlock_function_t	PAUSE 	;		//暂停
	tsBlock_function_t	CONTINUE	;		//继续
	tsBlock_function_t	NEXT 		;		//下一曲
	tsBlock_function_t	PREVIOUS	;		//上一曲
	tsBlock_function_t	BACK		;		//返回
}tsBlock_musicPlayer_t;

//音乐播放 事件各动作区块坐标
tsBlock_musicPlayer_t   tsBlock_event_playMusic = {
	// {(480 - 370), (480 - 480), 355, 445},		//开始播放 事件
	// {(480 - 370), (480 - 480), 355, 445},		//暂停播放 事件
	// {(480 - 370), (480 - 480), 355, 445},		//继续播放 事件
	// {(480 - 390), (480 - 466), 470, 540},		//下一曲   事件
	// {(480 - 390), (480 - 466), 264, 330},		//上一曲   事件
	// {(480 -     0),  (480 -  93),       0,   91},		//返回     事件
	// 
	{(480),   370, 355,  445},		//开始播放 事件
	{(480), (370), 355, 445},		//暂停播放 事件
	{(480), (370), 355, 445},		//继续播放 事件
	{(466), (390), 470, 540},		//上一曲   事件
	{(466), (390), 264, 330},		//下一曲   事件
	{(93),   (   0),     0,   91},		//返回     事件
};

/*
	两个全局变量，用于辅助完成音乐播放
*/
static my_list	 list_filename, list_falshName;
static node_pointer_type ndptr_previousplay,  ndptr_prevFlash;
static int tag_player = START_PLAYMUSIC;				//按钮位
static int status_musicplayer = START_PLAYMUSIC;			//播放器工作状态



 void *	flash_BGP(void * arg) {
 	/*
 		线程函数完成

 		建立文件名链表
		开始播放，上、下一曲：
			从链表头开始刷图
		暂停：
			停止刷图
		继续：
			继续刷图		

 	 */
 	 mylist_pointer_type   list_falshName_ptr 	= &list_falshName;
 	 ndptr_prevFlash 			 	= list_falshName_ptr->header;

 	 // 分离线程，确保线程结束时回收线程资源
      	 pthread_detach(pthread_self() );

 	 if (!size(list_falshName) ) {
 	 	perror("Flash picture file list is empty!");
 	 	return;
 	 }

 	 while (1) {

 	 	if (status_musicplayer == EXIT_PALYMUSIC)  {
 	 		// break;
 	 		return;
 	 	} else if (status_musicplayer == PAUSE_PLAYMUSIC) {
 	 		continue;
 	 	} else {
	 	 	show_shapeBmp(FLASHPIC_START_X, FLASHPIC_START_Y, ndptr_prevFlash->data);
	 	 	ndptr_prevFlash = (ndptr_prevFlash->next == list_falshName_ptr->trailer) ? list_falshName_ptr->header : ndptr_prevFlash->next;
	 	 	usleep( FREQ_FLASH_FRESH_USEC );	  	 		
 	 	}

 	}


 	pthread_exit(NULL);
 }

/*
	音乐播放器初始化，系统初始化时调用
*/
int  musicplayer_init() {

	DIR * musicdir_ptr             	=  NULL;
	struct dirent * stream_dir 		=  NULL;
	char fileName_buffer[255]   	=  {0};	

	mylist_pointer_type	 lst_fileName_ptr = &list_filename;
	if(NULL ==  creat_list(lst_fileName_ptr) ) {
		perror("Creat list error!");
		return -1;
	};

	// puts("musicplayer init !");
	
	// 打开音乐文件目录
	musicdir_ptr = opendir( PATH_MUSICDIR );
	if(!musicdir_ptr)
	{
		perror("Open music dirctory  fail or music dirctory do not exit!");
		return -1;
	}
	
	while (  (stream_dir = readdir(musicdir_ptr) ) != NULL ) {

	        //存储下该目录下 .mp3 格式的文件 文件名
	        if( strstr(stream_dir->d_name, ".mp3") )
	        {
	        	bzero(fileName_buffer, sizeof(fileName_buffer) );
	        	sprintf(fileName_buffer, "%s/%s",  PATH_MUSICDIR, stream_dir->d_name);
	         	push_front(lst_fileName_ptr,  fileName_buffer);
	         }  
	}

	/* 关闭音乐文件目录*/	
	closedir(musicdir_ptr);


	 lst_fileName_ptr = &list_falshName;
	if(NULL ==  creat_list(lst_fileName_ptr) ) {
		perror("creat flash name list list error!");
		return -1;
	};
	
	// 打开flash 文件目录
	musicdir_ptr = opendir( PATH_FLASHDIR );
	if(!musicdir_ptr)
	{
		perror("Open music dirctory  fail or music dirctory do not exit!");
		return -1;
	}
	
	while (  (stream_dir = readdir(musicdir_ptr) ) != NULL ) {

	        //存储下该目录下 .mp3 格式的文件 文件名
	        if( strstr(stream_dir->d_name, ".bmp") )
	        {
	        	bzero(fileName_buffer, sizeof(fileName_buffer) );
	        	// strcpy(fileName_buffer, PATH_FLASHDIR);
	        	sprintf(fileName_buffer, "%s/%s",  PATH_FLASHDIR, stream_dir->d_name);
	         	push_front(lst_fileName_ptr,  fileName_buffer);
	         }  
	}

	/* 关闭flash 文件目录*/	
	closedir(musicdir_ptr);

	tag_player = START_PLAYMUSIC;

	return 0;
}

void play_musicfile(char * music_fileName) {

	printf("Play music file ：%s!\n", music_fileName);

	char madCommand_buffer[255] = {0};

	sprintf(madCommand_buffer, "madplay %s &", music_fileName);
	system(madCommand_buffer);
}

void start_playmusic() {
	
	puts("Start play music!");

	//切换到停止播放选项背景图，等待停止播放事件触发
	show_bmp( PATH_PAUSEPLAY_BMP );

	//播放文件名链表中的首音乐文件
	play_musicfile(list_filename.header->data);

	ndptr_previousplay = list_filename.header;
	
}

void continue_playmusic() {
	puts("Continue play music!");
			
	system("killall -CONT madplay");
	
	//切换到暂停播放选项背景图
	show_bmp( PATH_PAUSEPLAY_BMP );
	
}

void pause_playmusic() {

	puts("Pause play music!");
			
	system("killall -STOP madplay");
	
	//切换到继续播放选项背景图
	show_bmp( PATH_CONTINUEPLAY_BMP );

}

void next_playmusic() {

	puts("Next music!");
			
	// 关闭正在播放的                   
	system("killall -KILL madplay");  
	
	//播放链表中下一个节点中文件名的音乐
	ndptr_previousplay = ( (ndptr_previousplay->next == list_filename.trailer) ? (list_filename.header) : (ndptr_previousplay->next) );
	
	//执行播放音乐的命令
	play_musicfile(ndptr_previousplay->data);

	//切换到暂停播放选项背景图
	show_bmp( PATH_PAUSEPLAY_BMP );

}

void previous_playmusic() {
	
	puts("Play previous played !");
			
	// 关闭正在播放的 
	// 此处存在 刚初始化后直接 killall 不成功的 BUG                       
	system("killall -KILL madplay"); 
	
	//播放链表中上一个节点中文件名的音乐
	ndptr_previousplay = ( (ndptr_previousplay == list_filename.header) ? (list_filename.trailer->front) : (ndptr_previousplay->front) );
	
	//执行播放音乐的命令
	play_musicfile(ndptr_previousplay->data); 

	//切换到暂停播放选项背景图
	show_bmp( PATH_PAUSEPLAY_BMP );
}

/*
	附加功能:	滑屏切换
*/
//音乐播放
void musicplayer() {

	//触摸屏 x 、 y 坐标值
	tsEvent_value_t 	tsEvent_value;
		
	puts("Into picture_displayer !");
	
	// 显示 音乐播放 背景图片
	show_bmp( PATH_CONTINUEPLAY_BMP );
	tag_player = START_PLAYMUSIC;
	status_musicplayer = PAUSE_PLAYMUSIC;

	 // 创建线程执行自动刷图
	pthread_t threadID_flashBGP = 0;
	if( 0 != pthread_create(&threadID_flashBGP, NULL, flash_BGP, NULL) )
	{
		perror("Create music flash_BGP display thread fail !");
	}

	
	while(1)
	{	
		//获取触屏事件的坐标值
		getCoordinate_ts( &tsEvent_value);

		if ( tsEvent_value.event_type == MODE_EVENT_TOUCH) {

			if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.START) && (tag_player == START_PLAYMUSIC)	) {

				// 音乐播放 事件
				start_playmusic();

				// 更新播放器状态
				tag_player = PAUSE_PLAYMUSIC;
				status_musicplayer = START_PLAYMUSIC;
				ndptr_prevFlash  = list_falshName.header;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.PAUSE) && (tag_player == PAUSE_PLAYMUSIC)  ) {

				//暂停播放  事件
				pause_playmusic();

				tag_player = CONTINUE_PLAYMUSIC;
				status_musicplayer = PAUSE_PLAYMUSIC;
				
			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.CONTINUE) &&  (tag_player == CONTINUE_PLAYMUSIC)	) {

				//继续播放	事件
				continue_playmusic();

				tag_player =  PAUSE_PLAYMUSIC;	
				status_musicplayer =  CONTINUE_PLAYMUSIC;
				
			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.NEXT) ) {

				//下一曲  事件
				next_playmusic();
				status_musicplayer = NEXT_PLAYMUSIC;
				ndptr_prevFlash  = list_falshName.header;
				
			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.PREVIOUS) ) {

				//上一曲  事件
				previous_playmusic();
				status_musicplayer = PREV_PALYMUSIC;
				ndptr_prevFlash  = list_falshName.header;
				
			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.BACK) )  {

				status_musicplayer = EXIT_PALYMUSIC;
				//返回  事件
				puts("Close music player !");

				
				// 关闭正在播放的                       
				system("killall -KILL madplay");
				
				//回到主功能 界面
				status_system = G_MAIN ;
				
				break;

			}

		} else if ( tsEvent_value.event_type == MODE_EVENT_SLIP) {

			if (check_slipDirct_H(tsEvent_value) > 0) {
				//下一曲  事件
				next_playmusic();
				status_musicplayer = NEXT_PLAYMUSIC;
				ndptr_prevFlash  = list_falshName.header;
			} else if (check_slipDirct_H(tsEvent_value) < 0) { 
				//上一曲  事件
				previous_playmusic();
				status_musicplayer = PREV_PALYMUSIC;
				ndptr_prevFlash  = list_falshName.header;
			}
		}

	}
	
}

/* int main(int argc,char **argv)
{
	musicplayer_init();
	do_musicplayer();
	
	return 0;
}
 */
