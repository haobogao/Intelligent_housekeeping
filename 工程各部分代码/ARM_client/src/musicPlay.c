#include "musicPlay.h"
#include "system_config.h"
// #include "system_config.h"
/*
	BUG ��¼:
		���ظ�����
		�����������ж�
		Flash ��Ч��ʼ״̬����
		��С��ɾ "{"
		����β�ж�
		tsBlock_event_playMusic ������ .h �ļ�
		�������л� ����ͼ
		�ս������ֲ�����ʱֱ���������� kill all ����
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
	tsBlock_function_t	START		;		//��ʼ����
	tsBlock_function_t	PAUSE 	;		//��ͣ
	tsBlock_function_t	CONTINUE	;		//����
	tsBlock_function_t	NEXT 		;		//��һ��
	tsBlock_function_t	PREVIOUS	;		//��һ��
	tsBlock_function_t	BACK		;		//����
}tsBlock_musicPlayer_t;

//���ֲ��� �¼���������������
tsBlock_musicPlayer_t   tsBlock_event_playMusic = {
	// {(480 - 370), (480 - 480), 355, 445},		//��ʼ���� �¼�
	// {(480 - 370), (480 - 480), 355, 445},		//��ͣ���� �¼�
	// {(480 - 370), (480 - 480), 355, 445},		//�������� �¼�
	// {(480 - 390), (480 - 466), 470, 540},		//��һ��   �¼�
	// {(480 - 390), (480 - 466), 264, 330},		//��һ��   �¼�
	// {(480 -     0),  (480 -  93),       0,   91},		//����     �¼�
	// 
	{(480),   370, 355,  445},		//��ʼ���� �¼�
	{(480), (370), 355, 445},		//��ͣ���� �¼�
	{(480), (370), 355, 445},		//�������� �¼�
	{(466), (390), 470, 540},		//��һ��   �¼�
	{(466), (390), 264, 330},		//��һ��   �¼�
	{(93),   (   0),     0,   91},		//����     �¼�
};

/*
	����ȫ�ֱ��������ڸ���������ֲ���
*/
static my_list	 list_filename, list_falshName;
static node_pointer_type ndptr_previousplay,  ndptr_prevFlash;
static int tag_player = START_PLAYMUSIC;				//��ťλ
static int status_musicplayer = START_PLAYMUSIC;			//����������״̬



 void *	flash_BGP(void * arg) {
 	/*
 		�̺߳������

 		�����ļ�������
		��ʼ���ţ��ϡ���һ����
			������ͷ��ʼˢͼ
		��ͣ��
			ֹͣˢͼ
		������
			����ˢͼ		

 	 */
 	 mylist_pointer_type   list_falshName_ptr 	= &list_falshName;
 	 ndptr_prevFlash 			 	= list_falshName_ptr->header;

 	 // �����̣߳�ȷ���߳̽���ʱ�����߳���Դ
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
	���ֲ�������ʼ����ϵͳ��ʼ��ʱ����
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
	
	// �������ļ�Ŀ¼
	musicdir_ptr = opendir( PATH_MUSICDIR );
	if(!musicdir_ptr)
	{
		perror("Open music dirctory  fail or music dirctory do not exit!");
		return -1;
	}
	
	while (  (stream_dir = readdir(musicdir_ptr) ) != NULL ) {

	        //�洢�¸�Ŀ¼�� .mp3 ��ʽ���ļ� �ļ���
	        if( strstr(stream_dir->d_name, ".mp3") )
	        {
	        	bzero(fileName_buffer, sizeof(fileName_buffer) );
	        	sprintf(fileName_buffer, "%s/%s",  PATH_MUSICDIR, stream_dir->d_name);
	         	push_front(lst_fileName_ptr,  fileName_buffer);
	         }  
	}

	/* �ر������ļ�Ŀ¼*/	
	closedir(musicdir_ptr);


	 lst_fileName_ptr = &list_falshName;
	if(NULL ==  creat_list(lst_fileName_ptr) ) {
		perror("creat flash name list list error!");
		return -1;
	};
	
	// ��flash �ļ�Ŀ¼
	musicdir_ptr = opendir( PATH_FLASHDIR );
	if(!musicdir_ptr)
	{
		perror("Open music dirctory  fail or music dirctory do not exit!");
		return -1;
	}
	
	while (  (stream_dir = readdir(musicdir_ptr) ) != NULL ) {

	        //�洢�¸�Ŀ¼�� .mp3 ��ʽ���ļ� �ļ���
	        if( strstr(stream_dir->d_name, ".bmp") )
	        {
	        	bzero(fileName_buffer, sizeof(fileName_buffer) );
	        	// strcpy(fileName_buffer, PATH_FLASHDIR);
	        	sprintf(fileName_buffer, "%s/%s",  PATH_FLASHDIR, stream_dir->d_name);
	         	push_front(lst_fileName_ptr,  fileName_buffer);
	         }  
	}

	/* �ر�flash �ļ�Ŀ¼*/	
	closedir(musicdir_ptr);

	tag_player = START_PLAYMUSIC;

	return 0;
}

void play_musicfile(char * music_fileName) {

	printf("Play music file ��%s!\n", music_fileName);

	char madCommand_buffer[255] = {0};

	sprintf(madCommand_buffer, "madplay %s &", music_fileName);
	system(madCommand_buffer);
}

void start_playmusic() {
	
	puts("Start play music!");

	//�л���ֹͣ����ѡ���ͼ���ȴ�ֹͣ�����¼�����
	show_bmp( PATH_PAUSEPLAY_BMP );

	//�����ļ��������е��������ļ�
	play_musicfile(list_filename.header->data);

	ndptr_previousplay = list_filename.header;
	
}

void continue_playmusic() {
	puts("Continue play music!");
			
	system("killall -CONT madplay");
	
	//�л�����ͣ����ѡ���ͼ
	show_bmp( PATH_PAUSEPLAY_BMP );
	
}

void pause_playmusic() {

	puts("Pause play music!");
			
	system("killall -STOP madplay");
	
	//�л�����������ѡ���ͼ
	show_bmp( PATH_CONTINUEPLAY_BMP );

}

void next_playmusic() {

	puts("Next music!");
			
	// �ر����ڲ��ŵ�                   
	system("killall -KILL madplay");  
	
	//������������һ���ڵ����ļ���������
	ndptr_previousplay = ( (ndptr_previousplay->next == list_filename.trailer) ? (list_filename.header) : (ndptr_previousplay->next) );
	
	//ִ�в������ֵ�����
	play_musicfile(ndptr_previousplay->data);

	//�л�����ͣ����ѡ���ͼ
	show_bmp( PATH_PAUSEPLAY_BMP );

}

void previous_playmusic() {
	
	puts("Play previous played !");
			
	// �ر����ڲ��ŵ� 
	// �˴����� �ճ�ʼ����ֱ�� killall ���ɹ��� BUG                       
	system("killall -KILL madplay"); 
	
	//������������һ���ڵ����ļ���������
	ndptr_previousplay = ( (ndptr_previousplay == list_filename.header) ? (list_filename.trailer->front) : (ndptr_previousplay->front) );
	
	//ִ�в������ֵ�����
	play_musicfile(ndptr_previousplay->data); 

	//�л�����ͣ����ѡ���ͼ
	show_bmp( PATH_PAUSEPLAY_BMP );
}

/*
	���ӹ���:	�����л�
*/
//���ֲ���
void musicplayer() {

	//������ x �� y ����ֵ
	tsEvent_value_t 	tsEvent_value;
		
	puts("Into picture_displayer !");
	
	// ��ʾ ���ֲ��� ����ͼƬ
	show_bmp( PATH_CONTINUEPLAY_BMP );
	tag_player = START_PLAYMUSIC;
	status_musicplayer = PAUSE_PLAYMUSIC;

	 // �����߳�ִ���Զ�ˢͼ
	pthread_t threadID_flashBGP = 0;
	if( 0 != pthread_create(&threadID_flashBGP, NULL, flash_BGP, NULL) )
	{
		perror("Create music flash_BGP display thread fail !");
	}

	
	while(1)
	{	
		//��ȡ�����¼�������ֵ
		getCoordinate_ts( &tsEvent_value);

		if ( tsEvent_value.event_type == MODE_EVENT_TOUCH) {

			if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.START) && (tag_player == START_PLAYMUSIC)	) {

				// ���ֲ��� �¼�
				start_playmusic();

				// ���²�����״̬
				tag_player = PAUSE_PLAYMUSIC;
				status_musicplayer = START_PLAYMUSIC;
				ndptr_prevFlash  = list_falshName.header;

			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.PAUSE) && (tag_player == PAUSE_PLAYMUSIC)  ) {

				//��ͣ����  �¼�
				pause_playmusic();

				tag_player = CONTINUE_PLAYMUSIC;
				status_musicplayer = PAUSE_PLAYMUSIC;
				
			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.CONTINUE) &&  (tag_player == CONTINUE_PLAYMUSIC)	) {

				//��������	�¼�
				continue_playmusic();

				tag_player =  PAUSE_PLAYMUSIC;	
				status_musicplayer =  CONTINUE_PLAYMUSIC;
				
			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.NEXT) ) {

				//��һ��  �¼�
				next_playmusic();
				status_musicplayer = NEXT_PLAYMUSIC;
				ndptr_prevFlash  = list_falshName.header;
				
			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.PREVIOUS) ) {

				//��һ��  �¼�
				previous_playmusic();
				status_musicplayer = PREV_PALYMUSIC;
				ndptr_prevFlash  = list_falshName.header;
				
			} else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_playMusic.BACK) )  {

				status_musicplayer = EXIT_PALYMUSIC;
				//����  �¼�
				puts("Close music player !");

				
				// �ر����ڲ��ŵ�                       
				system("killall -KILL madplay");
				
				//�ص������� ����
				status_system = G_MAIN ;
				
				break;

			}

		} else if ( tsEvent_value.event_type == MODE_EVENT_SLIP) {

			if (check_slipDirct_H(tsEvent_value) > 0) {
				//��һ��  �¼�
				next_playmusic();
				status_musicplayer = NEXT_PLAYMUSIC;
				ndptr_prevFlash  = list_falshName.header;
			} else if (check_slipDirct_H(tsEvent_value) < 0) { 
				//��һ��  �¼�
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
