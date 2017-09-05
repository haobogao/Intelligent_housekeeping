#include "my_showBMP.h"

//读出图片头信息函数  
//参数   int bmpfd ： bmp图片文件描述符
//返回值： 图片头信息类型的结构体
bmp  readbmpfile(int bmpfd)
{ 
    bmp  m;        //定义一个位图结构
        
    //读取头文件信息
     read(bmpfd, m.file.bfType, sizeof(char)*2);
    read(bmpfd, &m.file.bfSize, sizeof(long));
    read(bmpfd, &m.file.bfReserverd1, sizeof(short int));
    read(bmpfd, &m.file.bfReserverd2, sizeof(short int));   
    read(bmpfd, &m.info.biSize, sizeof(long));  
    read(bmpfd, &m.file.bfbfOffBits, sizeof(long));
    read(bmpfd, &m.info.biWidth, sizeof(long));
    read(bmpfd, &m.info.biHeight, sizeof(long));
    read(bmpfd, &m.info.biPlanes, sizeof(short));
    read(bmpfd, &m.info.biBitcount, sizeof(short));
    read(bmpfd, &m.info.biCompression, sizeof(long));
    read(bmpfd, &m.info.biSizeImage, sizeof(long));
    read(bmpfd, &m.info.biXPelsPermeter, sizeof(long));
    read(bmpfd, &m.info.biYPelsPermeter, sizeof(long));
    read(bmpfd, &m.info.biClrUsed, sizeof(long));
    read(bmpfd, &m.info.biClrImportant, sizeof(long));  
    
/*  printf("类型为%s\n", m.file.bfType);
    printf("文件长度为%ld\n", m.file.bfSize);
    printf("保留字1为%d\n",m.file.bfReserverd1);
    printf("保留字2为%d\n",m.file.bfReserverd2);
    printf("偏移量为%ld\n",m.file.bfbfOffBits);
    printf("此结构大小为%ld\n",m.info.biSize);
    printf("位图的宽度为%ld\n",m.info.biWidth);
    printf("位图的高度为%ld\n",m.info.biHeight);
    printf("目标设备位图数%hd\n",m.info.biPlanes);
    printf("颜色深度为%d\n",m.info.biBitcount);
    printf("位图压缩类型%hd\n",m.info.biCompression);
    printf("位图大小%ld\n",m.info.biSizeImage);
    printf("位图水平分辨率为%ld\n",m.info.biXPelsPermeter);
    printf("位图垂直分辨率为%ld\n",m.info.biYPelsPermeter);
    printf("位图实际使用颜色数%ld\n",m.info.biClrUsed);
    printf("位图显示中比较重要颜色数%ld\n",m.info.biClrImportant); */

    return m;
}

//指定位置（offset_x, int offset_y）显示指定图片 -- >  char *bmp_pathname
int show_shapeBmp(int offset_x, int offset_y, char *bmp_pathname);


// 自定义实现显示bmp格式图片 在LCD显示屏上
//参数：  char *bmp_pathname  图片路径字符串
//返回值： 0 -- 操作成功   非0-- 操作失败
int show_bmp(char *bmp_pathname)
{
    int bmpfd;
    int lcdfd;
    int index;
    int x,y;
    ssize_t retu;
    
    int *mem_lcd;
    // 定义数组用于存储读取到的bmp的颜色值
    char bmpbuf[SIZE_LCD*4] = {0};     
    // 定义数组存储转换之后的颜色值
    int lcdbuf[SIZE_LCD] = {0};   
    // 定义中间变量
    int tembuf[SIZE_LCD] = {0};
    //定义一个图片头信息结构变量
    bmp bmp_header;          

    //打开要显示的图片 
    if((bmpfd = open(bmp_pathname, O_RDONLY)) == -1)
    {
        perror("open bmp error!\n");
        return -1;
    }
    
    //打开液晶屏
    if((lcdfd = open(LCD_FILEPATH, O_RDWR)) == -1)
    {
        perror("open lcd error!\n");
        return -1;
    }
    
     // 映射lcd
    mem_lcd = (int *)mmap(NULL, SIZE_LCD*4, PROT_READ | PROT_WRITE, MAP_SHARED, lcdfd, 0);
    
    //读取图片头文件信息
    bmp_header = readbmpfile(bmpfd); 
    
    //判断图片大小 并作出相应地动作
    if((bmp_header.info.biWidth > WIDTH_LCD) || (bmp_header.info.biHeight > HEIGHT_LCD) )
    {
        puts("The pictrue is too large for LCD!");
        return -2;
    }
    else if( (bmp_header.info.biWidth < WIDTH_LCD) || (bmp_header.info.biHeight < HEIGHT_LCD) )
    {
        show_shapeBmp(DISPLAY_START_X, DISPLAY_START_Y, bmp_pathname);
        return -9;
    }

    
    if(bmp_header.info.biBitcount == 24)
    {
        // 读取图片的颜色值，从55字节开始读,前面已读54字节
        retu = read(bmpfd, bmpbuf, (bmp_header.file.bfSize - 54)); 
        if(retu == -1)
        {
            puts("read bit information erro!");
            return -3;
        }
        
        // 将读取的颜色值每个像素点3字节 转换成每个像素点4字节
        // 完成大小端转换, 图片本身存储的大小端格式与LCD显示的格式相反 
        //0x00bmpbuf[2]bmpbuf[1]bmpbuf[0]   ABGR
        for(index = 0; index < SIZE_LCD; index++) {
            lcdbuf[index] = bmpbuf[3*index] | bmpbuf[3*index+1]<<8 | bmpbuf[3*index+2]<<16 | 0x00<<24;
        }
        
        // 发现转换之后的颜色值还是不对，颠倒的，翻转回来
        // 完成上下对称转换
        for(x=0; x< WIDTH_LCD; x++)
        {
            for(y=0; y< HEIGHT_LCD; y++)
            {
                // 将颜色值写入到lcd中      
                *(mem_lcd+ y*WIDTH_LCD +  x)  =  lcdbuf[(bmp_header.info.biHeight-1-y)*bmp_header.info.biWidth+x];          
            }
        }
    }
    else if(bmp_header.info.biBitcount == 32)
    {
        // 读取图片的颜色值，从55字节开始读
        retu = read(bmpfd, lcdbuf, (bmp_header.file.bfSize - 54)); // bmpbuf[0] bmpbuf[1] bmpbuf[2] 图片上其中一个点的RGB值
        if(retu == -1)
        {
            puts("read color information erro!");
        }

        // 发现转换之后的颜色值还是不对，颠倒的，翻转回来
        // 完成上下对称转换
        for(x=0; x< WIDTH_LCD; x++) {
            for(y=0; y< HEIGHT_LCD; y++) {
                // 将颜色值写入到lcd中      
                *(mem_lcd+ y*WIDTH_LCD +  x)  =  lcdbuf[(bmp_header.info.biHeight-1-y)*bmp_header.info.biWidth+x];
            }
        }
        
    }
    else
    {
        // 非 24位或32位的bmp图片
        puts("bmp file form is fault!");
        return -4;
    }       
    
    // 关闭所有的资源，退出程序
    close(bmpfd);
    close(lcdfd);
    munmap(mem_lcd, SIZE_LCD*4);
    return 0;
}

//指定位置（offset_x, int offset_y）显示指定图片 -- >  char *bmp_pathname
//
int show_shapeBmp(int offset_x, int offset_y, char *bmp_pathname)
{
    int bmpfd;
    int lcdfd;
    int index;
    int x,y;
    ssize_t retu;
    
    int *mem_lcd;
    // 定义数组用于存储读取到的bmp的颜色值
    char bmpbuf[SIZE_LCD*4]; // char  占一个字节
    // 定义数组存储转换之后的颜色值
    int lcdbuf[SIZE_LCD];  //int  占4字节 
    //定义一个图片头信息结构变量
    bmp bmp_header;  
    
    //打开要显示的图片 
    if((bmpfd = open(bmp_pathname, O_RDONLY)) == -1)
    {
        perror("open bmp error!\n");
        printf("show bmp : %s\n", bmp_pathname );
        return -1;
    }
    
    //打开液晶屏
    if((lcdfd = open(LCD_FILEPATH, O_RDWR)) == -1)
    {
        perror("open lcd error!\n");
        return -1;
    }
    
    //读取图片头文件信息
    bmp_header = readbmpfile(bmpfd);    
    if(((bmp_header.info.biWidth + offset_x)  > WIDTH_LCD) || ((bmp_header.info.biHeight + offset_y) > HEIGHT_LCD) )
    {
        puts("The pictrue is too large for LCD or the picture can not display completely!");
        return -2;
    }
    
    // 映射lcd
    mem_lcd = (int *)mmap(NULL, SIZE_LCD*4, PROT_READ | PROT_WRITE, MAP_SHARED, lcdfd, 0);
    
    //每个象素24位的图片
    if(bmp_header.info.biBitcount == 24)
    {
        // 读取图片的颜色值，从55字节开始读
         // bmpbuf[0] bmpbuf[1] bmpbuf[2] 图片上其中一个点的RGB值
        retu = read(bmpfd, bmpbuf, (bmp_header.file.bfSize - 54));
        if(retu == -1)
        {
            perror("read color information erro!");
        }
        
        // 将读取的颜色值每个像素点3字节 转换成每个像素点4字节
        //0x00bmpbuf[2]bmpbuf[1]bmpbuf[0]   ABGR
        for(index = 0; index < SIZE_LCD; index++)
            lcdbuf[index] = bmpbuf[3*index]|bmpbuf[3*index+1]<<8|bmpbuf[3*index+2]<<16|0x00<<24;
        
        // 发现转换之后的颜色值还是不对，颠倒的，翻转回来
        //y 方向以中轴线对称
        for(x=0; x< bmp_header.info.biWidth; x++)
        {
            for(y=0; y< bmp_header.info.biHeight; y++)
            {
                // 将颜色值写入到lcd中      
                *(mem_lcd+(y+offset_y)*WIDTH_LCD + offset_x + x)  =  lcdbuf[(bmp_header.info.biHeight-1-y)*bmp_header.info.biWidth+x];
            }           
        }       
    }
    //每个象素32位的图片
    else if(bmp_header.info.biBitcount == 32)
    {
        // 读取图片的颜色值，从55字节开始读
        retu = read(bmpfd, lcdbuf, (bmp_header.file.bfSize - 54)); // bmpbuf[0] bmpbuf[1] bmpbuf[2] 图片上其中一个点的RGB值
        if(retu == -1)
        {
            puts("read color information erro!");
        }

        //y 方向以中轴线对称 并 将颜色值写入到lcd中
        for(x=0; x< bmp_header.info.biWidth; x++)
        {
            for(y=0; y< bmp_header.info.biHeight; y++)
            {
                *(mem_lcd+(y+offset_y)*WIDTH_LCD + offset_x + x)  =  lcdbuf[(bmp_header.info.biHeight-1-y)*bmp_header.info.biWidth+x];
            }           
        }
    }
    else
    {
        // 非 24位或32位的bmp图片
        puts("bmp file form is fault!");
    }   
    
    // 关闭所有的资源，退出程序
    close(bmpfd);
    close(lcdfd);
    munmap(mem_lcd, SIZE_LCD*4);
    return 0;
}


//获取图片的宽和高的信息
/*
    参数：
        path_BMP 图片地址
    返回值：
        成功： 宽度和高度的结构体
        失败： -1  
*/
bmp_bit_WidHight getWidthHight(char *path_BMP)
{
    int bmpfd = 0;
    //定义一个图片头信息结构变量
    bmp bmp_header;  
    bmp_bit_WidHight widthHight;
    
    //打开要显示的图片 
    if((bmpfd = open(path_BMP, O_RDONLY)) == -1)
    {
        perror("open number_bmp error!\n");
        return;
    }
    
    //读取图片头文件信息
    bmp_header = readbmpfile(bmpfd); //读取一个位图   
    
    //关闭文件
    close(bmpfd);

    widthHight.biWidth =  bmp_header.info.biWidth;
    widthHight.biHeight = bmp_header.info.biHeight;
    
    return widthHight;
}

/*
    图像缩放显示:
            将一张图片缩放到指定大小，显示在指定位置
            为避免频繁的I/O操作，I/O只打开一次，由调用该函数 的主函数执行关闭I/O操作
 */
static int lcdfd = -1;
static int *mem_lcd = NULL;

void close_lcd_show_zoomBMP() {

    if (lcdfd == -1 || mem_lcd == NULL) return;

     // 关闭所有的资源
    close(lcdfd);
    munmap(mem_lcd, SIZE_LCD*4);

    lcdfd = -1; 
    mem_lcd = NULL;

}

int show_zoomBMP_init() {
    if (lcdfd != -1 || mem_lcd != NULL) {
        // close_lcd_show_zoomBMP();
        return;
    }

    //打开液晶屏
    if((lcdfd = open(LCD_FILEPATH, O_RDWR)) == -1)    {
        perror("open lcd error!\n");
        return -1;
    }

     // 映射lcd
    mem_lcd = (int *)mmap(NULL, SIZE_LCD*4, PROT_READ | PROT_WRITE, MAP_SHARED, lcdfd, 0);
    
    return 0;
}

int show_bmp_zoom(char * bmp_pathname, int start_x, int start_y, int pic_size_x_zoomed, int pic_size_y_zoomed) {
    int bmpfd;
    ssize_t retu;
    
    // 定义数组用于存储读取到的bmp的颜色值
    char bmpbuf[SIZE_LCD*4] = {0}; // char  占一个字节
    // 定义数组存储转换之后的颜色值
    int lcdbuf[SIZE_LCD];  //int  占4字节 
    //定义一个图片头信息结构变量
    bmp bmp_header;  

    //避免频繁的I/O操作  加速刷图  同时多次打开LCD屏会产生系统错误
    if (lcdfd == -1 || NULL == mem_lcd) {
            show_zoomBMP_init();
    }

    //打开要显示的图片 
    if((bmpfd = open(bmp_pathname, O_RDONLY)) == -1)    {
        perror("open bmp error!\n");
        return -1;
    }
    
    // int lcdfd = -1;
    // int *mem_lcd = NULL;
    //打开液晶屏
    // if((lcdfd = open(LCD_FILEPATH, O_RDWR)) == -1)    {
    //     perror("open lcd error!\n");
    //     return -1;
    // }
    // 映射lcd
    // mem_lcd = (int *)mmap(NULL, SIZE_LCD*4, PROT_READ | PROT_WRITE, MAP_SHARED, lcdfd, 0);
    
    //读取图片头文件信息
    bmp_header = readbmpfile(bmpfd);    
    if(((pic_size_x_zoomed + start_x)  > WIDTH_LCD) || ((pic_size_y_zoomed + start_y) > HEIGHT_LCD) )    {
        puts("The pictrue is too large for LCD or the picture can not display completely!");
        return -2;
    } 
    if(( (bmp_header.info.biWidth)  < pic_size_x_zoomed) || ((bmp_header.info.biHeight) <  pic_size_y_zoomed) )    {
        puts("The pictrue smaller than zoomed pictrue!");
        return -2;
    }    
    
    // 读取图片的颜色值，从55字节开始读
    retu = read(bmpfd, bmpbuf, (bmp_header.file.bfSize - 54));
    if(retu == -1)    {
        perror("read color information erro!");
    }

        
     int index;
    //每个象素24位的图片
    if(bmp_header.info.biBitcount == 24)    {  

        // 将读取的颜色值每个像素点3字节 转换成每个像素点4字节
        //0x00bmpbuf[2]bmpbuf[1]bmpbuf[0]   ABGR
        for(index = 0; index < SIZE_LCD; index++)
            lcdbuf[index] = bmpbuf[3*index]|bmpbuf[3*index+1]<<8|bmpbuf[3*index+2]<<16|0x00<<24;

    }    else if(bmp_header.info.biBitcount != 32)    {
        // 非 24位或32位的bmp图片
        puts("bmp file form is fault!");
    }

    // 缩放因子
    int factor_zoom_x = (bmp_header.info.biWidth  / pic_size_x_zoomed);
    int factor_zoom_y = (bmp_header.info.biHeight / pic_size_y_zoomed);

    int x,y;
    int index_lcdBuff , index_memlcd;
    for(x = 0; x< pic_size_x_zoomed; x++)       {
        for(y = 0; y< pic_size_y_zoomed; y++)      {

            // 缩放 + 对称转换
            index_lcdBuff   =  ( (bmp_header.info.biHeight-1 -  ((y * factor_zoom_y) - 1 ))  * bmp_header.info.biWidth) +  ((x * factor_zoom_x) - 1);
            index_memlcd =  (y+start_y) * WIDTH_LCD  + start_x + x;

            *(mem_lcd + index_memlcd)  =  lcdbuf[ index_lcdBuff  ];
        }           
    }       
    
    // 关闭所有的资源
    close(bmpfd);
    // close(lcdfd);
    // munmap(mem_lcd, SIZE_LCD*4);

    return 0;
}


/* int main(int argc,char **argv)
{
    if(argc < 2)
    {
        printf("argc = %d\n", argc);
        puts("please input argv");
        return -1;
    }
    
    show_bmp(argv[1]);
    
    return 0;
} 
*/
