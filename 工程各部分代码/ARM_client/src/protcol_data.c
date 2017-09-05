/*
	BUG 记录
		host2net_prctND 大小端判断引起错误
 */
#include "protcol_data.h"
#include "debug.h"

// 创建协议数据节点
protocol_ptr_t  create_protc_nd() {
	return malloc( sizeof(protocol_t) );
}

// 填充协议内容
int fill_prctND_ptr(protocol_ptr_t  pctND_p, char dst, char cmd, char ui, char reserve, unsigned int value) {
	
	pctND_p->command.c_command.dst 	= 	dst;
	pctND_p->command.c_command.cmd = 	cmd;
	pctND_p->command.c_command.ui 	= 	ui;
	pctND_p->command.c_command.reserve = 	reserve;

	pctND_p->value 	= 	value;

	return 0;
}

// 释放协议数据节点内存
void free_prctND_ptr(protocol_ptr_t  pctND_p) {

	free(pctND_p);
}

int host2net_prctND(protocol_ptr_t  prctND_ptr) {

#ifdef  _DEBUG_M_
	printf("Transfore data!\n");
#endif
	prctND_ptr->command.u_command = htonl(prctND_ptr->command.u_command);
	prctND_ptr->value = htonl(prctND_ptr ->value);

	return 0;
}

int net2host_prctND(protocol_ptr_t  prctND_ptr) {
	
	prctND_ptr->command.u_command = ntohl(prctND_ptr->command.u_command);
	prctND_ptr->value = ntohl(prctND_ptr->value);

	return 0;
}

// 按协议格式发送收据给服务器
int send_data_to_server(char dst, char cmd, char ui, char reserve, unsigned int value)  {

	protocol_ptr_t  pctND_p = create_protc_nd();

	fill_prctND_ptr(pctND_p, dst, cmd, ui, reserve, value);

	host2net_prctND(pctND_p);
#ifdef  _DEBUG_M_
	printf("Send protocol data to server after transfore!\n");
	printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
#endif
	if (0 > send_msg_to_server(pctND_p, sizeof(protocol_t) ) ) {
		perror("Send protocol data error !\n ");
		free_prctND_ptr(pctND_p);
		return -1;
	}

	free_prctND_ptr(pctND_p);
	return 0;		
}

// 发送协议帧给服务器
int  send_prctdata_to_server(protocol_ptr_t  pctND_p) {

	// if (PC_SERVER != pctND_p->command.c_command.dst) {
	// 	puts("The destination of protcol data is not server !");
	// 	return -1;
	// }

#ifdef  _DEBUG_M_
	printf("Send protocol data to server before transfore!\n");
	printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
#endif

	ssize_t  send_len = 0;
	host2net_prctND(pctND_p);
#ifdef  _DEBUG_M_
	printf("Send protocol data to server after transfore!\n");
	printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
#endif
	if (0 > (send_len = send_msg_to_server(pctND_p, sizeof(protocol_t)) )  ) {
		perror("Send protocol data error !\n ");
		return -1;
	}

#ifdef  _DEBUG_M_
	printf("Sended protocol data lenth : %d\n", send_len);
#endif

	return 0;
}


// 协议数据处理 对发给板子的数据进行处理
int  prctData_process_client(protocol_ptr_t  pctND_p) {
	// 分析数据
	net2host_prctND(pctND_p);

#ifdef  _DEBUG_M_
	printf("Client recieved protcol data (after endian transfore): \n");
	printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
	printf("value : %u\n", (unsigned int)pctND_p->value);
#endif

	//给出相应的动作
	if (BOARD == pctND_p->command.c_command.dst) {
		
		switch (pctND_p->command.c_command.cmd) {
			case reslt_voice_recog: {
				recv_result_recog();
				break;
			} case led: {
				puts("Recieved led operate command !");
				led_operate(pctND_p->value);
				break;
			} case ui_contr: {
				puts("Recieved UI controll command !");
				system_UI_control(pctND_p->value);
				break;
			} default : {
				puts("Recv protcol data cmd is error !");
				printf("Protcol data cmd is ： %d\n", (int)pctND_p->command.c_command.cmd);
				break;
			}

		}

	} else {
		printf("ERROE data ! \nProtcol data dst is ： %d\n", (int)pctND_p->command.c_command.dst);
		return -1;
	}

	return 0;
}

