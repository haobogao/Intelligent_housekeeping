#ifndef __TH_VALUE_DISPLAY_H__
#define __TH_VALUE_DISPLAY_H__

#include "touchsreen.h"
#include "my_showBMP.h"
#include "show_number_BMP.h"
#include "check_mac.h"
#include "keyBoard.h"
#include "sensor_zigbee.h"

#ifndef  PATH_TH_VALUE_DISP_BGP
#define PATH_TH_VALUE_DISP_BGP    "./TH_sensor/TH_sensor_BGP/TH_sensor_BGP.bmp"
#endif

//高温警报值文字 背景图片 文件路径
#define  PATH_WARNUPPER_BMP 	"./TH_sensor/TH_sensor_BGP/high_warn_T_modify.bmp"
//低温警报值文字 背景图片 文件路径
#define  PATH_WARNLOWER_BMP 	"./TH_sensor/TH_sensor_BGP/low_warn_T_modify.bmp"

#ifndef PATH_TEMPERATURE_UNIT_BMP
#define PATH_TEMPERATURE_UNIT_BMP   "./TH_sensor/TH_sensor_BGP/temperatureUnit.bmp"
#endif
//温湿度显示 功能函数
void do_TH_sensor_disp(void);

#endif
