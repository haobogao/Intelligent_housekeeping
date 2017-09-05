#include "display_picture.h"
#include "system_config.h"
/*
      bug ��¼:
          list push_back() ��������
          show_bmp   �ļ�·������
          .bmp ��  small.bmp ���� ˳�򲻶�
          ͼƬ��ʾ��ʼλ��

          pthread_join  ɱ�߳�ɱ����
          �����л�    ����β����
          �����¼�����

          �Զ�ˢͼ: ��Сͼͬʱˢ 16��

          26�ţ�
          ���߳�ˢĿ¼ͼʱ ���� ---> ��Ϊ������ָ�룬ֵ�ڱ䣬���˹���Ҳ�ڱ�
                ���̹߳���ȫ�ֱ���  �ر�lcdʱ�ı���ȫ�ֱ�����
          27�ţ�
              Ŀ¼���ܣ�
                  ͬʱ��δ�LCD�������ϵͳ����  64���߳�ͬʱִ�д���ʾ��������������ϵͳ����
                  �̴߳���ʧ�� ---> ÿ���߳̽�����û����ʽ�����߳���Դ  ---> ��Դ��� ---> �̷߳���

      �޸ļ�¼:
          ���������������������ϵͳ��ʼ��ʱ���
          
          ����ͼƬ����������
          ��ӻ�������
          ����ѡ�񱳾�ͼ
        
        ����Ŀ¼����
          ȥСͼ��ͳһ��ͼ   ----->   8/26
           ����Ч���޸�
           ����Ŀ¼ˢͼ����   ---> ���߳�

      �������:
           .jpgͼƬ��ʾ   ---> jpg��ͼ��ʾ
          �ֶ�ģʽ�£�����ͼƬĿ¼����
              ��ͼ --- ͼƬ�����ֻ�
              ������ͼ
              ˫����ͼ
              ��Ŀ¼�е�ͼ����ֱ�Ӵ򿪸�ͼ

              Ŀ¼����  ---> 8��25

 */

#define MAX_PIC_DISP_NUM     20

#ifndef  FREQ_FRESH_AUTO_uSEC
#define FREQ_FRESH_AUTO_big_uSEC        (100 * 1000)
#define FREQ_FRESH_AUTO_small_uSEC     (400 * 1000)
#endif

#ifndef  CONTENT_THREAD_NUM
#define CONTENT_THREAD_NUM                ((MENU_PIC_NUM_X*MENU_PIC_NUM_Y > 64) ? (MENU_PIC_NUM_X*(MENU_PIC_NUM_Y / 2)) :(MENU_PIC_NUM_X*MENU_PIC_NUM_Y) )
#define CONTENT_FRESH_INTERVAL_Y   (CONTENT_THREAD_NUM / MENU_PIC_NUM_X ) 
#endif

// ȫ�ֱ��������ڸ������ͼƬ����
static  my_list  list_picture_name;
static node_pointer_type  ndptr_previousDisplay ;
// static int tag_displayPicture_mode =  DISPLAY_BIGPICTURE;
// Ŀ¼��ʾ�ĵ�һ��ͼ
static node_pointer_type  first_disp_contents_ndptr = NULL;

// �Զ���ͼ�߳�
static pthread_t threadID_autoDispPicture = 0;
static int  tag_dispMode = MODE_AUTO_DISPLAY;

//ͼƬ�����¼����е����� ��������
typedef struct {
      tsBlock_function_t  NEXT    ;                   //��һ��
      tsBlock_function_t  PREVIOUS  ;           //��һ��
      tsBlock_function_t  AUTO_DISPLAY;   //�Զ���ͼ
      tsBlock_function_t  CONTENTS;           //Ŀ¼
      tsBlock_function_t  BACK    ;                 //����
}tsBlock_pictureDisplayer_t;

//ͼƬ���� �¼���������������
tsBlock_pictureDisplayer_t  tsBlock_event_pictureDisplayer = {

    {480, (480 - 60),  637, 691},   //��һ��
    {480, (480 - 60),  367, 420},   //��һ��
    {480, (480 - 60),  502, 556},   //�Զ���ͼ
    {480, (480 - 60),   99, 154},   //Ŀ¼
    {480, (480 - 60),  233, 287},   //���� 
};


void picture_displayer() ;
// �Զ�ˢͼ�̺߳���
void *autoDisplay_picture_thrd(void *arg) ;
void next_displayPicture() ;
void previous_displayPicture();
void auto_displayPicture() ;
void back_displayPicture();
void display_pictureFile(char * picture_fileName);
int do_contents_displayer();

void display_pictureFile(char * picture_fileName) {

    show_bmp(picture_fileName);

}

void display_BGP(char * BGP_path) {
    bmp_bit_WidHight   widthHight;
    widthHight = getWidthHight(BGP_path);

    show_shapeBmp(DISPLAY_START_X,   HEIGHT_LCD - widthHight.biHeight, BGP_path);
}

// ͼƬ��������ʼ��: ��ͼƬ�ļ��У������洢ͼƬ���ֵ�����
int pictureplayer_init() {  

    DIR * pictureDir_ptr  =  NULL;
    struct dirent * stream_dir = NULL;
    char fileName_buffer[255]     =  {0};

    mylist_pointer_type lst_picName_ptr = &list_picture_name;  

    if(NULL ==  creat_list(lst_picName_ptr) ) {
          perror("Creat list to big picture  error!");
          return -1;
    }

    pictureDir_ptr = opendir(PATH_PICTURE_DIRECTOR);
    if ( !pictureDir_ptr) {
        perror("Open  picture diretory fail!");
        return -1;
    }

    while (  (stream_dir = readdir(pictureDir_ptr) ) != NULL ) {
      //�洢�¸�Ŀ¼�� .bmp ��ʽ���ļ� �ļ���
      bzero(fileName_buffer, sizeof(fileName_buffer) );
      sprintf(fileName_buffer, "./%s/%s", PATH_PICTURE_DIRECTOR, stream_dir->d_name);

      if( strstr(fileName_buffer, ".bmp") )  {
        push_front(lst_picName_ptr,  fileName_buffer);
      }

    }

    /* �ر�ͼƬ�ļ�Ŀ¼*/ 
    closedir(pictureDir_ptr);

    // �����߳�ִ���Զ�ˢͼ
    tag_dispMode = MODE_MANUAL_DISPLAY;
    if( 0 != pthread_create(&threadID_autoDispPicture, NULL, autoDisplay_picture_thrd, NULL) )
    {
        perror("Create auto display picture thread fail!");
    }

    return 0;
}
// �Զ�ˢͼ�̺߳���
void *autoDisplay_picture_thrd(void *arg) {
        // ��ˢͼƬ 
        //       1.  ȫͼ��ʾ
        //       2. 8*4
        //       3. 16*8
        //   ��Ч��
        //     ����Χ���м�

    if ( !size(list_picture_name) ) {
        pictureplayer_init();
    }
    show_zoomBMP_init();

    // ���������ָ��
    node_pointer_type index_disp_ndptr = list_picture_name.header;
    // const int MAX_PIC_DISP_NUM    = 20;
    int matric_disp[MAX_PIC_DISP_NUM][MAX_PIC_DISP_NUM] = {0};

    while (1) {

          if (tag_dispMode == MODE_AUTO_DISPLAY) {

             int pic_num_x, pic_num_y;

            for (pic_num_x = 1; pic_num_x <= 16; pic_num_x *=  2) {
                 index_disp_ndptr = list_picture_name.header;
                if (tag_dispMode != MODE_AUTO_DISPLAY)  break;
                if(pic_num_x == 2 || pic_num_x == 4) continue;

                memset(matric_disp, 0, sizeof(matric_disp));         

                (pic_num_x == 1)? (pic_num_y = 1) : (pic_num_y = pic_num_x / 2);
                int pic_size_x = (WIDTH_LCD  / pic_num_x);
                int pic_size_y = (HEIGHT_LCD / pic_num_y);

                int x, y, offset_x, offset_y;
                int ok = 0;

                x = -1;
                y = 0;
                while(tag_dispMode == MODE_AUTO_DISPLAY) {
                    ok = 0;

                   while( (++x) < pic_num_x && !matric_disp[x][y] && tag_dispMode == MODE_AUTO_DISPLAY) {
                      matric_disp[x][y] = 1; ok = 1;

                      offset_x = x * pic_size_x;
                      offset_y = y * pic_size_y;

                      show_bmp_zoom(index_disp_ndptr->data, offset_x, offset_y, pic_size_x, pic_size_y);
                      index_disp_ndptr = get_next_node(list_picture_name, index_disp_ndptr);
                    }
                    (x > 0) ? (--x) : (x);

                    while ((++y) < pic_num_y && !matric_disp[x][y] && tag_dispMode == MODE_AUTO_DISPLAY) {
                      matric_disp[x][y] = 1; ok = 1;

                      offset_x = x * pic_size_x;
                      offset_y = y * pic_size_y;

                      show_bmp_zoom(index_disp_ndptr->data, offset_x, offset_y, pic_size_x, pic_size_y);
                      index_disp_ndptr = get_next_node(list_picture_name, index_disp_ndptr);
                    }
                    (y > 0) ? (--y) : (y);

                    while ((--x) >= 0 && !matric_disp[x][y] && tag_dispMode == MODE_AUTO_DISPLAY) {
                      matric_disp[x][y] = 1; ok = 1;


                      offset_x = x * pic_size_x;
                      offset_y = y * pic_size_y;

                      show_bmp_zoom(index_disp_ndptr->data, offset_x, offset_y, pic_size_x, pic_size_y);
                      index_disp_ndptr = get_next_node(list_picture_name, index_disp_ndptr);
                    }
                    ++x;

                    while ((--y) >= 0 && !matric_disp[x][y] && tag_dispMode == MODE_AUTO_DISPLAY) {
                      matric_disp[x][y] = 1; ok = 1;

                      offset_x = x * pic_size_x;
                      offset_y = y * pic_size_y;

                      show_bmp_zoom(index_disp_ndptr->data, offset_x, offset_y, pic_size_x, pic_size_y);
                      index_disp_ndptr = get_next_node(list_picture_name, index_disp_ndptr);
                    }
                    ++y;

                    if (pic_num_x == 1 ) {
                        x = -1;  matric_disp[0][0] = 0;
                        if (index_disp_ndptr->front == list_picture_name.trailer) {
                            break;                          
                        }
                        usleep(FREQ_FRESH_AUTO_big_uSEC);
                        continue;
                    }
                   if (!ok) {
                      break;
                   }

                  usleep(FREQ_FRESH_AUTO_small_uSEC);
                }

             }

          }
        
     }

    // �ر���ʾ�� ---> ������ʾ����
    close_lcd_show_zoomBMP();
    
}

void next_displayPicture() {

    puts("Display next picture !");

    ndptr_previousDisplay = get_next_node(list_picture_name, ndptr_previousDisplay);
    display_pictureFile(ndptr_previousDisplay->data);

    // ���Ϲ���ѡ��ͼ
    display_BGP( BGP_selectFunction_pictureDisplayer );
}

void previous_displayPicture() {

    puts("Display previous  picture !");
    
    ndptr_previousDisplay = get_front_node(list_picture_name, ndptr_previousDisplay);

    display_pictureFile(ndptr_previousDisplay->data);

    // ���Ϲ���ѡ��ͼ
    display_BGP( BGP_selectFunction_pictureDisplayer );
}

void auto_displayPicture() { 

    puts("auto display picture!");

    tag_dispMode = MODE_AUTO_DISPLAY;
}

void back_displayPicture() {

    // �ر���ʾ�� ---> ������ʾ����
    close_lcd_show_zoomBMP();

    // ����������
    status_system = G_MAIN;
}

// ˢͼ��ɱ�־λ
static int tag_contents_freshed = 0;

int do_contents_displayer() {

    puts("Picture contents !");

    //������ x �� y ����ֵ
    tsEvent_value_t   tsEvent_value;
    // ����Ҫ�򿪵�ͼ��λ��
    int pos_touch_pic = 0;

    while(1)
    {
        tag_contents_freshed = 0;
        contents_displayer();
        while (!tag_contents_freshed) continue;

        //��ȡ�����¼�������ֵ
        getCoordinate_ts( &tsEvent_value);

        if ( tsEvent_value.event_type == MODE_EVENT_TOUCH) {

          pos_touch_pic =  (tsEvent_value.touch_coordinate.x - DISPLAY_START_X) / MENU_PIC_SIZE_X + \
                                     ( ((tsEvent_value.touch_coordinate.y - DISPLAY_START_Y) / MENU_PIC_SIZE_Y) * (MENU_PIC_NUM_X) );
           
          printf("Touch to open the %d picture!\n", pos_touch_pic);

          ndptr_previousDisplay = get_elem_next_pos_nd(list_picture_name, first_disp_contents_ndptr, pos_touch_pic);

          tag_dispMode = MODE_MANUAL_DISPLAY;
          display_pictureFile(ndptr_previousDisplay->data);

          return 0;

        } else if( tsEvent_value.event_type == MODE_EVENT_SLIP) {
              // �ϻ�
               if (check_slipDirct_V(tsEvent_value) < 0) {
                      puts("Contens backward !");
                      first_disp_contents_ndptr =  get_elem_next_pos_nd(list_picture_name, first_disp_contents_ndptr, MENU_PIC_NUM_X);
               } else if  (check_slipDirct_V(tsEvent_value) > 0) {
                      puts("Contens forward !");
                      first_disp_contents_ndptr =  get_elem_front_pos_nd(list_picture_name, first_disp_contents_ndptr, MENU_PIC_NUM_X);
               }
        }
    }
}

void * disp_contents_pic(void * arg) {

        // �����̣߳�ȷ���߳̽���ʱ�����߳���Դ
       pthread_detach(pthread_self() );

        show_zoomBMP_init();

        int x, y, offset_x, offset_y;

        x =  (*((int *)arg));
        free(arg);

        node_pointer_type index_disp_ndptr = get_elem_next_pos_nd(list_picture_name, first_disp_contents_ndptr, x);
        
        y =  (x  /   MENU_PIC_NUM_X);
        x =  (x % MENU_PIC_NUM_X);

        offset_x = DISPLAY_START_X + (x * MENU_PIC_SIZE_X);

        for (y; y < MENU_PIC_NUM_Y; y += CONTENT_FRESH_INTERVAL_Y )  {

            offset_y = DISPLAY_START_Y + (y * MENU_PIC_SIZE_Y);

            show_bmp_zoom(index_disp_ndptr->data, offset_x, offset_y, MENU_PIC_SIZE_X, MENU_PIC_SIZE_Y);

            index_disp_ndptr =  get_elem_next_pos_nd(list_picture_name, index_disp_ndptr, CONTENT_THREAD_NUM);
        }
        // puts("fresh done !");

        tag_contents_freshed = 1;
        pthread_exit(NULL);
        return;
}

// Ŀ¼����
void contents_displayer() {
      if (first_disp_contents_ndptr == NULL) {
            first_disp_contents_ndptr  =  list_picture_name.header;
       }

      // �����߳�ˢͼ ÿ���߳����ˢһ��
      int x = 0;
      for (x = 0; x < CONTENT_THREAD_NUM; x++) {

            // �����߳�ִ���Զ�ˢͼ
            pthread_t threadID;

            int * thread_arg = (int *)malloc(sizeof(int));  //�߳����ͷŸ��ڴ�
            if (thread_arg == NULL) {
                perror("Malloc memory error !");
                return;
            }
            *thread_arg = x;

            if( 0 != pthread_create(&threadID, NULL, disp_contents_pic, (void *)(thread_arg)) )   {
                perror("Create auto display picture thread fail!");
            }
      }
}

// // Ŀ¼����   ---->  ˢͼЧ������
// void contents_displayer() {
//     if (first_disp_contents_ndptr == NULL) {
//         first_disp_contents_ndptr  =  list_picture_name.header;
//     }
//     node_pointer_type index_disp_ndptr = first_disp_contents_ndptr;

//     show_bmp_init();

//     int x, y, offset_x, offset_y;
//     for (y = 0; y < MENU_PIC_NUM_Y; y++) {

//         offset_y = DISPLAY_START_Y + (y * MENU_PIC_SIZE_Y);

//         for (x = 0; x < MENU_PIC_NUM_X; x++) {
//             offset_x = DISPLAY_START_X + (x * MENU_PIC_SIZE_X);

//             show_bmp_zoom(index_disp_ndptr->data, offset_x, offset_y, MENU_PIC_SIZE_X, MENU_PIC_SIZE_Y);

//             index_disp_ndptr = get_next_node(list_picture_name, index_disp_ndptr);
//         }
//     }

//     // �ر���ʾ�� ---> ������ʾ����
//     close_lcd_show();
//     // ������
//     // first_disp_contents_ndptr = index_disp_ndptr;
// }

void picture_displayer() {

    //������ x �� y ����ֵ
    tsEvent_value_t   tsEvent_value;
    // ������ʼ��
    ndptr_previousDisplay = list_picture_name.header;
    // �Զ�ˢͼģʽ
    auto_displayPicture();    

    while(1)
    {
          //��ȡ�����¼�������ֵ
        getCoordinate_ts( &tsEvent_value);
            
          // ���Ϊ�Զ�ˢͼģʽֹͣ�Զ�ˢͼ
         if (tsEvent_value.event_type !=  MODE_EVENT_WRONG && tag_dispMode == MODE_AUTO_DISPLAY) {

                tag_dispMode = MODE_MANUAL_DISPLAY;
        } 
        if (tag_dispMode == MODE_MANUAL_DISPLAY) {
                 // ���Ϲ���ѡ��ͼ
                display_BGP( BGP_selectFunction_pictureDisplayer );
        }

         if ( tsEvent_value.event_type == MODE_EVENT_TOUCH) {

                 // ���Ϲ���ѡ��ͼ
                display_BGP( BGP_selectFunction_pictureDisplayer );

                // ��ⴥ���¼�
                if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.NEXT) ) {

                      // ��һ�� �¼�
                      next_displayPicture();

                } else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.PREVIOUS) ) {

                      // ��һ�� �¼�
                      previous_displayPicture();

                }  else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.AUTO_DISPLAY) ) {

                    // �Զ�ˢͼ �¼�
                    if (tag_dispMode != MODE_AUTO_DISPLAY) {
                        tag_dispMode = MODE_AUTO_DISPLAY;
                        auto_displayPicture();          
                    }

                }  else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.CONTENTS) ) {

                    // Ŀ¼��ʾ
                    do_contents_displayer();

                }    else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.BACK) ) {

                    // �˳� �¼�
                    back_displayPicture();
                    break;
                }  
          }  else if( tsEvent_value.event_type == MODE_EVENT_SLIP) {
                    if (check_slipDirct_H(tsEvent_value) < 0) {
                         // ��һ�� �¼�
                          previous_displayPicture();
                      } else if (check_slipDirct_H(tsEvent_value) > 0) { 
                           // ��һ�� �¼�
                          next_displayPicture();
                     }
          }
     }
}

// int  main(int argc, char const *argv[])
// {
//     pictureplayer_init();

//     picture_displayer(); 

  
//   return 0;
// }