#include "socket_server.h"
#include "debug.h"

static int socket_fd 	= 	-1;
static int con_fd 	= 	-1;

#ifndef	  PORT_SOCKET
#define  PORT_SOCKET				23333
#endif

#ifndef  BUFF_SIZE_RECV
#define BUFF_SIZE_RECV 				1024
#endif

void  *recv_process_prctData_from_client(void * arg);

int socket_server_init()	 {

	if (socket_fd != -1)  return -1;

	//1. 创建socket
	if((socket_fd = socket(AF_INET,SOCK_STREAM,0))==-1) {
		printf("Socket error\n");
		return -1;
	}

	//2. 构造绑定的地址
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port =  htons(PORT_SOCKET);
	server_addr.sin_addr.s_addr =  htons(INADDR_ANY);	// inet_addr("127.0.0.1");
	
	//让服务器可以连续使用
	int sinsize=1;
	setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&sinsize,sizeof(int));

	//3. 绑定地址
   	if( bind(socket_fd, (struct sockaddr *) (&server_addr), sizeof(struct sockaddr)) == -1) {
		perror("Bind error\n");
		return -1;
	} 

	//4. 监听
	if( -1 == listen(socket_fd, 5) ) {
		perror("Listen error ! \n");
		return -1;
	}

	// 创建接收来自 socket 客户端的协议数据线程
	pthread_t recv_prctFram_thread_id;
	if(0 != pthread_create(&recv_prctFram_thread_id,NULL, recv_process_prctData_from_client, NULL) ) {
		perror("Can not  create thread to recv prct data !\n");
		return -1;
	}

	return 0;
}

int wait_connect() {
	if(socket_fd == -1){
	     socket_server_init();
	}

	printf("\nWait for client connect !\n");

	//5. accept
	struct sockaddr_in addr_recv;

	int sin_size = sizeof(struct sockaddr_in);
	if( (con_fd =  accept(socket_fd, (struct sockaddr*)&addr_recv, &sin_size) ) == -1) {
		perror("Accept error !\n");
		return -1;
	}
	
        	printf("\nClient connected : IP:%s\n", inet_ntoa(addr_recv.sin_addr));  
	
	return 0;
}

void  *recv_process_prctData_from_client(void * arg) {

	if(con_fd == -1) {
		wait_connect();
	}
	
	protocol_t * pctND_p = create_protc_nd();

	while (1) {
		
		// 接收协议帧
		unsigned int  msg_len = 0;
		bzero(pctND_p, sizeof(protocol_t));

		//接收协议数据数据帧
		msg_len  = 	recv(con_fd, pctND_p, sizeof(protocol_t), 0);

		// printf("收到协议数据的大小: %d\n", msg_len);		
		
		if (msg_len < 0) {
			perror("Recieve protocol data failed !");
			return;
		} else if (msg_len == 0) { continue; }

#ifdef  _DEBUG_M_		
		printf("\n\nReceived data in recv thread from client : size %d\n", msg_len);
		puts("Command data before transfor :");
		printf("dst : %d  cmd: %d ui: %d, reverse: %d\n", (int)pctND_p->command.c_command.dst, (int)pctND_p->command.c_command.cmd, (int)pctND_p->command.c_command.ui, (int)pctND_p->command.c_command.reserve);
#endif	
	
		if (msg_len != sizeof(protocol_t)) {

			puts("Recv data size is not  protocol frame size !");
			printf("Recv data size : %d\n", msg_len);

		} else {

#ifdef  _DEBUG_M_		
			// 处理数据
			if (prctData_process_server(pctND_p) < 0) {
				puts("Process protocol data error ! \n");
				return;
			}
#else
			// 处理数据
			prctData_process_server(pctND_p);
#endif		
		}

	}

	// 释放内存
	free_prctND(pctND_p);

	return 0;

}

int recv_file_from_client(char * file_path_dir, char * recvd_fileName) {

	if(con_fd == -1) {
		wait_connect();
	}
	
	char msg_buff[ BUFF_SIZE_RECV ]  =  {0};
	unsigned int  msg_len = 0;

	// 收文件名
	bzero(msg_buff, sizeof(msg_buff));
	msg_len = recv(con_fd, msg_buff, BUFF_SIZE_RECV, 0);
	if (msg_len < 0) {
		perror("Receive file name failed ! \n");

		return -1;
	}

#ifdef  _DEBUG_M_	
	printf("\n文件名： %s, 数组大小: %d\n", msg_buff, strlen(msg_buff));
#endif	

	sprintf(recvd_fileName, "client_%s", msg_buff);

	printf("\nStart to recieved file: %s\n", recvd_fileName);

	//收文件的大小
	bzero(msg_buff, sizeof(msg_buff));
	msg_len = recv(con_fd, msg_buff, BUFF_SIZE_RECV, 0);
	if (msg_len < 0) {
		perror("Receive file size failed ! \n");

		return -1;
	}
	
#ifdef  _DEBUG_M
	printf("文件大小： %s\n", msg_buff);
#endif	
	//转换收到的文件大小为整形
	int file_size = 0;
	sscanf(msg_buff,"%d",&file_size);

	//开始接收文件内容
	char file_path[255] = {0}; 
	sprintf(file_path, "%s/%s", file_path_dir, recvd_fileName);
	FILE *file_p = fopen(file_path, "w+");
	if (NULL == file_p ) {

		perror("Open file failed ! ");
		printf("File name : %s\n", file_path);

		return -1;
	}

	bzero(msg_buff, sizeof(msg_buff));
	int total_len = 0;

	while((msg_len = recv(con_fd, msg_buff, BUFF_SIZE_RECV, 0)) > 0)  {
		
		fwrite(msg_buff, sizeof(char), msg_len, file_p);
		total_len += msg_len;
		if(total_len >= file_size)  {
			break;
		}	

		bzero(msg_buff, sizeof(msg_buff));
		msg_len = 0;
	}

	printf("Receive file done : %s\n\n", recvd_fileName);
	fclose(file_p);

	return 0;
}


// int send_msg_to_client(const char * result) {

// 	if(con_fd == -1){
// 		wait_connect();
// 	}

// 	send(con_fd, result, strlen(result), 0);// 可能需要发送定长
// 	printf("Send data to clientt : %s\n", result);
// 	return 0;
// }

int send_msg_to_client(const void * msg, unsigned int msg_len ) {
	if(con_fd == -1){
		wait_connect();
	}
	
#ifdef  _DEBUG_M_		
	printf("Send data to clientt : %s， length : %u\n", (char *)msg, msg_len);
#endif

	return send(con_fd, msg, msg_len, 0);	// 可能需要发送定长;
}

int close_socket_server() {

	close(con_fd);
	close(socket_fd);
	
	socket_fd = -1;
	con_fd = -1;

	return 0;
}

// int main(int argc, char const *argv[])
// {
// 	char recvd_fileName[255] = {0};

// 	socket_server_init( );
// 	if( !recv_file_from_client("./", recvd_fileName)  ) {
// 		send_msg_to_client("Receive data !!! _server! ");
// 	}

// 	return 0;
// }