/*
	BUG 记录：
		send 的数据大小： 不是 sizeof(指针)
 */
#include "protcol_data.h"
#include "debug.h"

protocol_ptr_t  create_protc_nd() {
	return malloc( sizeof(protocol_t) );
}

// 填充协议内容
int fill_prctND(protocol_ptr_t  pctND_p, char dst, char cmd, char ui, char reserve, unsigned int value) {
	
	pctND_p->command.c_command.dst 	= 	dst;
	pctND_p->command.c_command.cmd 	= 	cmd;
	pctND_p->command.c_command.ui 	= 	ui;
	pctND_p->command.c_command.reserve = 	reserve;

	pctND_p->value 	= 	value;

	return 0;
}

void free_prctND(protocol_ptr_t  pctND_p) {

	free(pctND_p);
}

// 是否为大端模式
int host_is_bigEndian() {
	unsigned int da = 0x01;

	return (da >> 1) ? 1 : 0;
}

int host2net_prctND(protocol_ptr_t  pctND_p) {

	// if (host_is_bigEndian) {
	// 	return 0;
	// }

	pctND_p->command.u_command = htonl(pctND_p->command.u_command);
	pctND_p->value = htonl(pctND_p ->value);

	return 0;
}

int net2host_prctND(protocol_ptr_t  pctND_p) {

	// if ( !host_is_bigEndian) {
	// 	return 0;
	// }

	pctND_p->command.u_command = ntohl(pctND_p->command.u_command);
	pctND_p->value = ntohl(pctND_p->value);

	return 0;
}

int send_data_toWX(char dst, char cmd, char ui, char reserve, unsigned int value)  {
	protocol_ptr_t  pctND_p = create_protc_nd();

	fill_prctND(pctND_p, dst, cmd, ui, reserve, value);

	host2net_prctND(pctND_p);
	if (0 > websocket_send(pctND_p, sizeof(protocol_t) ) ) {
		free_prctND(pctND_p);
		return -1;
	} else {
		free_prctND(pctND_p);
		return 0;
	}

}

int send_data_toBoard(char dst, char cmd, char ui, char reserve, unsigned int value)  {

	protocol_ptr_t  pctND_p = create_protc_nd();

	fill_prctND(pctND_p, dst, cmd, ui, reserve, value);

	host2net_prctND(pctND_p);
#ifdef  _DEBUG_M_
	printf("Send protocol data to client after transfore!\n");
	printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
	printf("value : %u\n", (unsigned int)pctND_p->value);
#endif
	if (0 > send_msg_to_client(pctND_p, sizeof(protocol_t) ) ) {
		perror("Send data to client error !n");
		free_prctND(pctND_p);
		return -1;
	}

	free_prctND(pctND_p);
	return 0;		
}

// 发送协议帧给板子
int  send_prctdata_toBoard(protocol_ptr_t  pctND_p) {
	if (BOARD != pctND_p->command.c_command.dst) {
		puts("Send data to board error ! The destination of protcol data is not board !");
		printf("The destination of des: %d(int)\n",  (int)pctND_p->command.c_command.dst);
		return -1;
	}

#ifdef  _DEBUG_M_
	printf("Send protocol data to board! (before transfore)\n");
	printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
	printf("value : %u\n", (unsigned int)pctND_p->value);
#endif
	
	host2net_prctND(pctND_p);
#ifdef  _DEBUG_M_
	printf("Send protocol data to client after transfore!\n");
	printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
	printf("value : %u\n", (unsigned int)pctND_p->value);
#endif
	if (0 < send_msg_to_client(pctND_p, sizeof(protocol_t) ) ) {
		return -1;
	}

	return 0;
}

// 发送协议帧给微信
int  send_prctdata_toWX(protocol_ptr_t  pctND_p) {
	if (WX_APP != pctND_p->command.c_command.dst) {
		puts("Send data to WX_APP error ! The destination of protcol data is not WX_APP !");
		printf("The destination of des: %d(int)\n",  (int)pctND_p->command.c_command.dst);
		return -1;
	}

	host2net_prctND(pctND_p);
	websocket_send(pctND_p, sizeof(protocol_t) );
	// send_data_toWX(pctND_p->command.c_command.dst, pctND_p->command.c_command.cmd, pctND_p->command.c_command.ui, pctND_p->command.c_command.reserve, pctND_p->value);

	return 0;	
}

// 协议数据处理
int  prctData_process_server(protocol_ptr_t  pctND_p) {

#ifdef  _DEBUG_M_
	printf("Client sended protcol data : (before transfore)\n");
	printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
	printf("value : %u\n", (unsigned int)pctND_p->value);
#endif
	// 分析数据
	net2host_prctND(pctND_p);

#ifdef  _DEBUG_M_
	printf("Client sended protcol data : (after transfore)\n");
	printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
	printf("value : %u\n", (unsigned int)pctND_p->value);
#endif

	//给出相应的动作
	if (BOARD == pctND_p->command.c_command.dst) {
		send_prctdata_toBoard(pctND_p); 
		switch (pctND_p->command.c_command.cmd) {
			case led:
				printf("Sended led command data to board !\n");
				break;
			case reslt_voice_recog:
				printf("Sended reslt_voice_recog data to board !\n");
				break;
			case ui_contr:
				printf("Sended UI command data to board !\n");
				break;
			default:
				break;	
		}
		// printf("send board data : %lld\n", (long long)(*pctND_p));
	} else if (WX_APP == pctND_p->command.c_command.dst) {
		send_prctdata_toWX(pctND_p);
		switch (pctND_p->command.c_command.cmd) {
			case temperature:
				printf("Sended temperature value data to WeiXinAPP !\n");
				break;
			case humidity:
				printf("Sended humidity value data to WeiXinAPP !\n");
				break;
			case status_board:
				printf("Sended board UI status data to WeiXinAPP !\n");
				break;
			default:
				break;	
		}
	}else if (PC_SERVER == pctND_p->command.c_command.dst) {
		if (speech_recognize == pctND_p->command.c_command.cmd) {
			start_speech_recognize();
		}
	}else {
		printf("Process data error in process_server! \nProtcol data dst is ： %d(int)\n", (int)pctND_p->command.c_command.dst);
		
		return -1;
	}

	return 0;
}
