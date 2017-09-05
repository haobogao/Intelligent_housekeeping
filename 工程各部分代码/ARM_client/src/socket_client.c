#include "socket_client.h"
#include "protcol_data.h"
#include "debug.h"

/*
	BUG 记录:
		send  与 recv 数据大小不一致---> 定长发送
		接收协议数据时 ； 大小为 sizeof(协议指针)

		
 */
static int socket_fd  		=  -1;

void  *recv_process_prctData_from_server(void * arg);

int socket_client_init() {

	//创建socket
	if(0 > (socket_fd = socket(AF_INET,SOCK_STREAM,0) ) ) {
		perror("Socket initialize error !");
		return -1;
	}

    	//构造服务器地址
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port =  htons(PORT);
	addr.sin_addr.s_addr = inet_addr(SERVICE_IP);

	printf("Start to connect server: %s, port : %d\n", SERVICE_IP, PORT);
	
	//连接服务器
	if(-1 == connect(socket_fd, (struct sockaddr *) &addr,sizeof(struct sockaddr))) {
		perror("Connect service failed ! \n");
		socket_fd  		=  -1;
		return -1;
	}

	printf("Connect to %s sucess! \n",SERVICE_IP);

	// 创建线程收来自服务器的数据
	pthread_t recv_prctFram_thread_id;
	if(0 != pthread_create(&recv_prctFram_thread_id,NULL, recv_process_prctData_from_server, NULL) ) {
		perror("Can not  create thread to recv prct data !\n");
		return -1;
	}

	return socket_fd;
}

//发送文件
int send_file_to_serv(char *file_path_dir, char * file_name) {
	if(socket_fd == -1){
		if (0 >  socket_client_init() ) {
			puts("Can not connect to server !");
			return -1;
		}
	}

	char file_path[255] = {0};
	sprintf(file_path, "%s/%s", file_path_dir, file_name);

	FILE *file_p = fopen(file_path, "r");
	if(file_p ==  NULL){
		printf("Open file : %s error!\n", file_path);
		return	-1;
	}

	int file_size = 0;
	fseek(file_p,0,SEEK_END);
	file_size = ftell(file_p);
	fseek(file_p,0,SEEK_SET);	

	printf("Start send file to server : %s\n", file_path);
	char send_buff[BUFF_SIZE_SEND] = {0};
	int len_send = 0;

	// 发送文件名
	bzero(send_buff, sizeof(send_buff));
	strcpy(send_buff, file_name);
	len_send = send(socket_fd, send_buff, BUFF_SIZE_SEND, 0);
	if (len_send < 0) {
		perror("Send file name failed ! \n");
		return -1;
	}
#ifdef  _DEBUG_M_	
	printf("文件名： %s\n", send_buff);
#endif
	//5. 发送文件大小
	bzero(send_buff, sizeof(send_buff));
	sprintf(send_buff, "%d\n", file_size);
	len_send = send(socket_fd, send_buff, BUFF_SIZE_SEND, 0);
	if (len_send < 0) {
		perror("Send file size failed ! \n");
		return -1;
	}
#ifdef  _DEBUG_M_
	printf("Send file_size : %s\n", send_buff);
#endif
	//6. 读取文件内容
	//7. 发送文件内容
	// printf("sizeof buff:  %d\n", sizeof(send_buff));

	int total_send = 0, ret = 0;

	bzero(send_buff, sizeof(send_buff));
	while( (len_send = fread(send_buff, sizeof(char), sizeof(send_buff), file_p)) > 0 )
	{
		if ((ret = send(socket_fd, send_buff, len_send, 0))  < 0) {
			perror("Send file failed ! \n");
			return -1;
		}

		total_send += ret;
		// printf("Send data lenth: %d/%d\n", ret, total_send);

		bzero(send_buff,sizeof(send_buff));
	}

	printf("Send file finished : %s\n", file_path);

	fclose(file_p);

	return 0;

}

// 发送信息给服务器
int send_msg_to_server(const void * msg, unsigned int msg_len ) {
	if(socket_fd == -1){
		if (0 >  socket_client_init() ) {
			puts("Can not connect to server !");
			return -1;
		}
	}
	
	// if (send(socket_fd, msg, msg_len, 0) < 0) {
	// 	perror("Send massage to server failed ! \n");
	// 	return -1;
	// }

	return send(socket_fd, msg, msg_len, 0);
}

//接收speech Recognise结果
int recv_speechRecgResult_from_serv(char * msg_buff,  unsigned int buff_size) {

	if(socket_fd == -1){
		if (0 >  socket_client_init() ) {
			puts("Can not connect to server !");
			return -1;
		}
	}

	int len_recv = recv(socket_fd, msg_buff, buff_size, 0);
	if (len_recv < 0) {
		perror("Receive data failed !\n ");
		return -1;
	}

	return len_recv;
}

// 接收来自服务器的数据 线程
void  *recv_process_prctData_from_server(void * arg) {

	if(socket_fd == -1){
		if (0 >  socket_client_init() ) {
			puts("Can not connect to server !");
			return;
		}
	}
	
	protocol_ptr_t  pctND_p = create_protc_nd();
	while (1) {
		
		// 接收协议帧
		unsigned int  msg_len = 0;
		bzero(pctND_p, sizeof(protocol_t));
		msg_len = recv(socket_fd, pctND_p, sizeof(protocol_t), 0);
		if (msg_len <= 0) {
			continue;
		}

#ifdef  _DEBUG_M_		
		// net2host_prctND(pctND_p);
		printf("\n\nReceived data in recv thread from server : size %d\n", msg_len);
		printf("Before endian transfore !\n");
		printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
#endif
		
		if (msg_len != sizeof(protocol_t)) {
			puts("Recv data size is not  protocol frame size !");
			printf("Recv data size : %d\n", msg_len);
			// return -1;
		} else {

			// 处理数据
			prctData_process_client(pctND_p);	
		}

	}

	// 释放内存
	free_prctND_ptr(pctND_p);

	return 0;

}


//关闭socket
int close_socket_client() {

	// 关闭socket
	close(socket_fd);
	return 0;
}

// int main(int argc, char const *argv[])
// {
// 	char msg_buff[BUFF_SIZE_SEND] = {0};
// 	socket_client_init();
// 	if( !send_file_to_serv("." , "data")  ){
// 		printf("Send file sucess! \n");
// 		memset(msg_buff, 0, sizeof(msg_buff) );
// 		recv_result_from_serv(msg_buff, BUFF_SIZE_SEND);
// 		printf("Receive data: \n%s\n", msg_buff);
// 	}

// 	return 0;
// }
