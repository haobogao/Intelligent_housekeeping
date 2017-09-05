#include "touchsreen.h"

/*
	BUG :
		触屏速率调整
		滑动坐标起始点为无穷小

	处理中间 pressure == 0 的情况
	遇 pressure == 0 返回
	舍弃后来的值

	坐标防抖动:
		起始点
		pressure == 0 的点 不要

	   舍弃第一个点？
	   点太少

	   短滑动 像素点距离
	   	sleep == 0 的点
		
 */
#define  	WRONG_RET 		0
#define	TRUE_RET			1

// 滑动边界像素点数
#define	DISTANCE_SLIP		30

#define 	uSEC_P_SEC			1000000
#define 	FREQ_TOUCH_uSEC		(2 * 1000 *100)
// 采样时间间隔
#define 	FREQ_SAMPLE_uSEC		(40 * 1000)

int check_time_dist_touch(struct timeval ta, struct timeval tb) {
	// printf("time distance : %ld\n", abs( (ta.tv_sec* uSEC_P_SEC + ta.tv_usec)  - (tb.tv_sec* uSEC_P_SEC + tb.tv_usec) ));
	if ( abs( (ta.tv_sec* uSEC_P_SEC + ta.tv_usec)  - (tb.tv_sec* uSEC_P_SEC + tb.tv_usec) )< FREQ_TOUCH_uSEC ) {
		return 0;
	} else {
		return 1;
	}
}

int check_time_dist_sample(struct timeval ta, struct timeval tb) { 
	if ( abs( (ta.tv_sec* uSEC_P_SEC + ta.tv_usec)  - (tb.tv_sec* uSEC_P_SEC + tb.tv_usec) )< FREQ_SAMPLE_uSEC ) {
		return 1;
	} else {
		return 0;
	}
}

int getCoordinate_ts(tsEvent_value_t * tsEvent_value_ptr) {

	//打开触摸屏设备文件  
	struct tsdev *ts = ts_open(PATH_TOUCHSREEN, 0);

	//配置触摸屏 
	ts_config(ts);

	coordinate_t  touch_coordinate, loose_coordinate, prev_coordinate;
	int 	tag_wrong_ret = TRUE_RET;
	int 	tag_first_ret     =  0;
	struct timeval  timeval_previous;

	sample_ts_type ts_sample;
	ts_sample.x = ts_sample.y = 0;

	// 获取松手时的点 --- 	消抖后
	do {
		prev_coordinate.x = ts_sample.x;
		prev_coordinate.y = ts_sample.y;

		// 读取坐标
		ts_read(ts, &ts_sample, 1); //这里会有一个阻塞
		
		// 如果此次返回值时间与 形参tsEvent_value_ptr 中的时间相差过小		 
		if ( !tag_first_ret)  {
			tag_first_ret++;

			touch_coordinate.x =  ts_sample.x;
			touch_coordinate.y =  ts_sample.y;

			timeval_previous = ts_sample.tv;	//第一次的时间值
			if ( !check_time_dist_touch(tsEvent_value_ptr->tv,  ts_sample.tv) ) {
				tag_wrong_ret = WRONG_RET;

			}
		}

		// printf("gets coordinate : (%d, %d), pressure: %d\n", ts_sample.x, ts_sample.y, ts_sample.pressure);
	} while (ts_sample.pressure != 0 || (check_time_dist_sample(timeval_previous, ts_sample.tv) && tag_wrong_ret != WRONG_RET) );

	if (tag_wrong_ret  == WRONG_RET) {
		tsEvent_value_ptr->event_type = MODE_EVENT_WRONG;
		tsEvent_value_ptr->tv =  ts_sample.tv;
		puts("Too fast touch !\n");
		return -1;
	}

	// 保存松手坐标值
	loose_coordinate.x =  ts_sample.x;
	loose_coordinate.y =  ts_sample.y;

	// 判断起止点的距离，进而判断事件类型
	if ( (abs(touch_coordinate.x - loose_coordinate.x)  >  DISTANCE_SLIP) ||  (abs(touch_coordinate.y - loose_coordinate.y)  > DISTANCE_SLIP) ) {
		tsEvent_value_ptr->event_type   = 	MODE_EVENT_SLIP;
		tsEvent_value_ptr->slip_vector.start_coordinate = touch_coordinate;
		tsEvent_value_ptr->slip_vector.stop_coordinate = loose_coordinate;

		printf("Slip event: (%d, %d) --> (%d, %d)\n", tsEvent_value_ptr->slip_vector.start_coordinate.x, tsEvent_value_ptr->slip_vector.start_coordinate.y, tsEvent_value_ptr->slip_vector.stop_coordinate.x, tsEvent_value_ptr->slip_vector.stop_coordinate.y);

	} else {
		tsEvent_value_ptr->event_type = 	MODE_EVENT_TOUCH;
		tsEvent_value_ptr->touch_coordinate = 	loose_coordinate;

		printf("Touch event : (%d, %d)\n", tsEvent_value_ptr->touch_coordinate.x, tsEvent_value_ptr->touch_coordinate.y);
	}

	//时间记录
	tsEvent_value_ptr->tv =  ts_sample.tv;

	//4.关闭触摸屏设备文件 ts_close()
	ts_close(ts);

	return 0;
}

// 左：负值  右: 正值
int check_slipDirct_H(tsEvent_value_t 	tsEvent_value) {
	if ( tsEvent_value.event_type != MODE_EVENT_SLIP) {
		puts("The touch screen event is not slip !");

		return 0;
	} 

	return  (tsEvent_value.slip_vector.stop_coordinate.x - tsEvent_value.slip_vector.start_coordinate.x);
}
// 垂直方向滑动事件的判断
// 上：负值  下: 正值
int check_slipDirct_V(tsEvent_value_t 	tsEvent_value) {
	if ( tsEvent_value.event_type != MODE_EVENT_SLIP) {
		puts("The touch screen event is not slip !");

		return 0;
	} 

	return  (tsEvent_value.slip_vector.stop_coordinate.y - tsEvent_value.slip_vector.start_coordinate.y);
}

/*int main(int argc, char const *argv[])
{
	tsEvent_value_t tsEvent_value;

	while (1) {
		getCoordinate_ts(&tsEvent_value);

		if (tsEvent_value.event_type != MODE_EVENT_WRONG) {
			if ( tsEvent_value.event_type == MODE_EVENT_TOUCH)
			{
				printf("touch! get touch_coordinate: ");
				printf("(%d, %d)\n", tsEvent_value.touch_coordinate.x, tsEvent_value.touch_coordinate.y);
			} else {
				printf("slip! get slip_vector: ");
				printf("(%d, %d) and (%d, %d)\n", tsEvent_value.slip_vector.start_coordinate.x, tsEvent_value.slip_vector.start_coordinate.y, tsEvent_value.slip_vector.stop_coordinate.x, tsEvent_value.slip_vector.stop_coordinate.y);
			}
			printf("\n\n\n");
		}
	}
	return 0;
}*/