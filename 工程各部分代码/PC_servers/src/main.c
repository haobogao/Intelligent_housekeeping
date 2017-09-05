#include <stdio.h>
#include "websocket.h"
#include "protcol_data.h"
#include <pthread.h>
#include "socket_server.h"

void * recv_websocket(void * arg){

	init_websocket_v12();

}

int test_pro();

int main(int argc, char const *argv[])
{

	pthread_t nopoll_thread_id;
	pthread_create(&nopoll_thread_id,NULL,recv_websocket,NULL);
	voice_recognise_init();
	socket_server_init();

	while(1) {
		// test_pro();
	}

	return 0;
}

int test_pro(){
	printf("============================================\n \
			1. 温度测试\n \
			2. 湿度测试\n \
			3. 光照测试\n \
			4. MQ_2测试\n \
			5. 字符串测试\n \
			6. 单字符测序\n\
			7. 状态测试（二进制）\n");
	printf("============================================\n");
	fflush(stdin);
	int tmp = 0;
	scanf("%d",&tmp);

	protocol_ptr_t  pctND_p = create_protc_nd();

	switch(tmp){
		case 1 :{
			fill_prctND(pctND_p,WX_APP,temperature,0,0,40);
			break;
		}
		case 2 :{
			fill_prctND(pctND_p,WX_APP,humidity,0,0,50);
			break;
		}
		case 3:{
			fill_prctND(pctND_p,WX_APP,light,0,0,60);
			break;
		}
		case 4:{
			fill_prctND(pctND_p,WX_APP,MQ_2,0,0,0xff);
			break;
		}
		case 5:{
			// fill_prctND(pctND_p,WX_APP,WX_APP_FUNC_STATUS,0,0,0xff);
			websocket_send("abcdefyq",8);
			return 0;
			break;
		}
		case 6:{
			websocket_send("A",1);
			return 0;
			break;
		}
		case 7:{
			fill_prctND(pctND_p,WX_APP,status_board,0,0,LCD_OK|TEM_OK|H_OK|L_OK);
			break;
		}
		default:{
			printf("cmd error!\n");
			return -1;
		}
	}

	// printf("0x%8x,0x%8x\n", *(unsigned int *)pctND_p,*((unsigned int *)pctND_p+1));
	// host2net(pctND_p);
	printf("send data to WX: \ndst : %d  cmd: %d ui: %d, reverse: %d\nvalue: %u\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve, (unsigned int)pctND_p->value);
	send_prctdata_toWX(pctND_p);
	free_prctND(pctND_p);
	return 0;

}