#ifndef  __SOCKET_H__
#define __SOCKET_H__ 

#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>

#ifndef SERVICE_IP
#define SERVICE_IP 			"192.168.200.129"
#endif

#ifndef	  PORT
#define PORT 				23333
#endif

#ifndef BUFF_SIZE_SEND
#define BUFF_SIZE_SEND 			1024
#endif

int socket_client_init();

//发送文件
int send_file_to_serv(char *file_path_dir, char * file_name);

//接收speech Recognise结果
int recv_speechRecgResult_from_serv(char * msg_buff,  unsigned int buff_size);

// 发送信息给服务器
int  send_msg_to_server(const void * msg, unsigned int msg_len );

//关闭socket
int close_socket_client();

#endif