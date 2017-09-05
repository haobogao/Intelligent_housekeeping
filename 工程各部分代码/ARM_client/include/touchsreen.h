#ifndef __TOUCHSREEN_H__
#define __TOUCHSREEN_H__

#include "tslib.h"
#include <stdio.h>
#include <math.h>
#include "tsOperation_block.h"

#ifndef 	PATH_TOUCHSREEN
#define 	PATH_TOUCHSREEN 	"/dev/event0"
#endif

#define 	MODE_EVENT_SLIP			1
#define 	MODE_EVENT_TOUCH		2
#define 	MODE_EVENT_WRONG		0

#ifndef	COORDINAT_T
#define COORDINAT_T	
typedef struct  coordinate
{
	int x, y;
}coordinate_t;
#endif

typedef struct  vct
{
	coordinate_t  start_coordinate;
	coordinate_t  stop_coordinate;
}slip_vector_t;

typedef struct ts_event_value
{
	int event_type;
	slip_vector_t slip_vector;
	coordinate_t  touch_coordinate;
	struct timeval	tv;
}tsEvent_value_t;

typedef struct ts_sample sample_ts_type;

int getCoordinate_ts(tsEvent_value_t * tsEvent_value_ptr);

// 左：负值  右: 正值
int check_slipDirct_H(tsEvent_value_t 	tsEvent_value);

// 垂直方向滑动事件的判断
// 上：负值  下: 正值
int check_slipDirct_V(tsEvent_value_t 	tsEvent_value);

#endif