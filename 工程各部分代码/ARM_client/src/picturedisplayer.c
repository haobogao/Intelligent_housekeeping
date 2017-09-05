#include "display_picture.h"
#include "system_config.h"
/*
      bug 记录:
          list push_back() 出现问题
          show_bmp   文件路径不对
          .bmp 与  small.bmp 区分 顺序不对
          图片显示初始位置

          pthread_join  杀线程杀不死
          上下切换    链表尾处理
          触屏事件阻塞

          自动刷图: 大小图同时刷 16号

          26号：
          多线程刷目录图时 传参 ---> 因为传的是指针，值在变，传了过后也在变
                多线程共用全局变量  关闭lcd时改变了全局变量，
          27号：
              目录功能：
                  同时多次打开LCD屏会产生系统错误  64个线程同时执行打开显示屏操作，产生了系统错误
                  线程创建失败 ---> 每次线程结束后都没有显式回收线程资源  ---> 资源溢出 ---> 线程分离

      修改记录:
          将建立链表操作独立，在系统初始化时完成
          
          增加图片播放器函数
          添加滑动处理
          功能选择背景图
        
        增加目录功能
          去小图，统一大图   ----->   8/26
           动画效果修改
           完善目录刷图功能   ---> 多线程

      功能添加:
           .jpg图片显示   ---> jpg动图显示
          手动模式下，增加图片目录功能
              滑图 --- 图片跟着手滑
              划屏切图
              双手缩图
              点目录中的图可以直接打开该图

              目录功能  ---> 8月25

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

// 全局变量，用于辅助完成图片播放
static  my_list  list_picture_name;
static node_pointer_type  ndptr_previousDisplay ;
// static int tag_displayPicture_mode =  DISPLAY_BIGPICTURE;
// 目录显示的第一张图
static node_pointer_type  first_disp_contents_ndptr = NULL;

// 自动切图线程
static pthread_t threadID_autoDispPicture = 0;
static int  tag_dispMode = MODE_AUTO_DISPLAY;

//图片播放事件具有的属性 变量类型
typedef struct {
      tsBlock_function_t  NEXT    ;                   //下一张
      tsBlock_function_t  PREVIOUS  ;           //上一张
      tsBlock_function_t  AUTO_DISPLAY;   //自动切图
      tsBlock_function_t  CONTENTS;           //目录
      tsBlock_function_t  BACK    ;                 //返回
}tsBlock_pictureDisplayer_t;

//图片播放 事件各动作区块坐标
tsBlock_pictureDisplayer_t  tsBlock_event_pictureDisplayer = {

    {480, (480 - 60),  637, 691},   //下一张
    {480, (480 - 60),  367, 420},   //上一张
    {480, (480 - 60),  502, 556},   //自动切图
    {480, (480 - 60),   99, 154},   //目录
    {480, (480 - 60),  233, 287},   //返回 
};


void picture_displayer() ;
// 自动刷图线程函数
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

// 图片播放器初始化: 打开图片文件夹，建立存储图片名字的链表
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
      //存储下该目录下 .bmp 格式的文件 文件名
      bzero(fileName_buffer, sizeof(fileName_buffer) );
      sprintf(fileName_buffer, "./%s/%s", PATH_PICTURE_DIRECTOR, stream_dir->d_name);

      if( strstr(fileName_buffer, ".bmp") )  {
        push_front(lst_picName_ptr,  fileName_buffer);
      }

    }

    /* 关闭图片文件目录*/ 
    closedir(pictureDir_ptr);

    // 创建线程执行自动刷图
    tag_dispMode = MODE_MANUAL_DISPLAY;
    if( 0 != pthread_create(&threadID_autoDispPicture, NULL, autoDisplay_picture_thrd, NULL) )
    {
        perror("Create auto display picture thread fail!");
    }

    return 0;
}
// 自动刷图线程函数
void *autoDisplay_picture_thrd(void *arg) {
        // 轮刷图片 
        //       1.  全图显示
        //       2. 8*4
        //       3. 16*8
        //   特效：
        //     由外围向中间

    if ( !size(list_picture_name) ) {
        pictureplayer_init();
    }
    show_zoomBMP_init();

    // 遍历链表的指针
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

    // 关闭显示屏 ---> 批量显示已完
    close_lcd_show_zoomBMP();
    
}

void next_displayPicture() {

    puts("Display next picture !");

    ndptr_previousDisplay = get_next_node(list_picture_name, ndptr_previousDisplay);
    display_pictureFile(ndptr_previousDisplay->data);

    // 放上功能选择图
    display_BGP( BGP_selectFunction_pictureDisplayer );
}

void previous_displayPicture() {

    puts("Display previous  picture !");
    
    ndptr_previousDisplay = get_front_node(list_picture_name, ndptr_previousDisplay);

    display_pictureFile(ndptr_previousDisplay->data);

    // 放上功能选择图
    display_BGP( BGP_selectFunction_pictureDisplayer );
}

void auto_displayPicture() { 

    puts("auto display picture!");

    tag_dispMode = MODE_AUTO_DISPLAY;
}

void back_displayPicture() {

    // 关闭显示屏 ---> 批量显示已完
    close_lcd_show_zoomBMP();

    // 返回主程序
    status_system = G_MAIN;
}

// 刷图完成标志位
static int tag_contents_freshed = 0;

int do_contents_displayer() {

    puts("Picture contents !");

    //触摸屏 x 、 y 坐标值
    tsEvent_value_t   tsEvent_value;
    // 触屏要打开的图的位置
    int pos_touch_pic = 0;

    while(1)
    {
        tag_contents_freshed = 0;
        contents_displayer();
        while (!tag_contents_freshed) continue;

        //获取触屏事件的坐标值
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
              // 上滑
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

        // 分离线程，确保线程结束时回收线程资源
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

// 目录功能
void contents_displayer() {
      if (first_disp_contents_ndptr == NULL) {
            first_disp_contents_ndptr  =  list_picture_name.header;
       }

      // 创建线程刷图 每个线程完成刷一列
      int x = 0;
      for (x = 0; x < CONTENT_THREAD_NUM; x++) {

            // 创建线程执行自动刷图
            pthread_t threadID;

            int * thread_arg = (int *)malloc(sizeof(int));  //线程中释放该内存
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

// // 目录功能   ---->  刷图效果不好
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

//     // 关闭显示屏 ---> 批量显示已完
//     close_lcd_show();
//     // 调试用
//     // first_disp_contents_ndptr = index_disp_ndptr;
// }

void picture_displayer() {

    //触摸屏 x 、 y 坐标值
    tsEvent_value_t   tsEvent_value;
    // 变量初始化
    ndptr_previousDisplay = list_picture_name.header;
    // 自动刷图模式
    auto_displayPicture();    

    while(1)
    {
          //获取触屏事件的坐标值
        getCoordinate_ts( &tsEvent_value);
            
          // 如果为自动刷图模式停止自动刷图
         if (tsEvent_value.event_type !=  MODE_EVENT_WRONG && tag_dispMode == MODE_AUTO_DISPLAY) {

                tag_dispMode = MODE_MANUAL_DISPLAY;
        } 
        if (tag_dispMode == MODE_MANUAL_DISPLAY) {
                 // 放上功能选择图
                display_BGP( BGP_selectFunction_pictureDisplayer );
        }

         if ( tsEvent_value.event_type == MODE_EVENT_TOUCH) {

                 // 放上功能选择图
                display_BGP( BGP_selectFunction_pictureDisplayer );

                // 检测触屏事件
                if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.NEXT) ) {

                      // 下一张 事件
                      next_displayPicture();

                } else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.PREVIOUS) ) {

                      // 上一张 事件
                      previous_displayPicture();

                }  else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.AUTO_DISPLAY) ) {

                    // 自动刷图 事件
                    if (tag_dispMode != MODE_AUTO_DISPLAY) {
                        tag_dispMode = MODE_AUTO_DISPLAY;
                        auto_displayPicture();          
                    }

                }  else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.CONTENTS) ) {

                    // 目录显示
                    do_contents_displayer();

                }    else if ( touchAction_check( tsEvent_value.touch_coordinate, tsBlock_event_pictureDisplayer.BACK) ) {

                    // 退出 事件
                    back_displayPicture();
                    break;
                }  
          }  else if( tsEvent_value.event_type == MODE_EVENT_SLIP) {
                    if (check_slipDirct_H(tsEvent_value) < 0) {
                         // 上一张 事件
                          previous_displayPicture();
                      } else if (check_slipDirct_H(tsEvent_value) > 0) { 
                           // 下一张 事件
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